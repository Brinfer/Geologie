/**
 * @file dispatcherLOG.c
 *
 * @version 1.0
 * @date 06/06/21
 * @author Nathan BRIENT
 * @copyright BSD 2-clauses
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MQ_MAX_MESSAGES (10)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <unistd.h> // Macros, type Posix and co
#include <pthread.h>
#include <mqueue.h>
#include <time.h>
#include <errno.h>

#include "dispatcherLOG.h"
#include <stdint.h>
#include "../com_common.h"
#include <stdio.h>
#include "../../tools.h"
#include <stdbool.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief variable static pour le thread
 *
 */
static pthread_t myThreadListen;

/**
 * @brief variable static pour la boucle while dans le thread
 *
 */
static bool keepGoing = false;

/**
 * @brief Le mutex de dispatcher
 *
 */
static pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief methode appelee par un thread pour recevoir les action a faire
 *
 * cette metode lira la socket et effectuera les actions
 */
static void* readMsg();

/**
 * @brief methode appelee par order pour executer les actions
 *
 * @param action action a effectuee
 * @param msg structure contenant les donnees a utiliser pour effectuer l'action
 */
static void dispatch(Trame* trame, Header* Header);

/**
 * @brief Retourne l'indication du thread de dispatcherLOG s'il doit continuer ou non sa routine.
 *
 * @return true Le thread de dispatcherLOG doit continuer sa routine.
 * @return false Le thread de dispatcherLOG doit arreter sa routine.
 */
static bool getKeepGoing(void);

/**
 * @brief Modifie l'indication du thread de dispatcherLOG s'il doit continuer ou non sa routine.
 *
 * @param newValue
 */
static void setKeepGoing(bool newValue);

/**
 * @brief Lis la trame contenant, la traduit et la met dans header
 *
 * @param header variable ou on mettra le header une fois lu et exporté
 */
static int16_t readHeader(Header* header);

static bool getKeepGoing(void) {
    bool returnValue;
    pthread_mutex_lock(&myMutex);
    returnValue = keepGoing;
    pthread_mutex_unlock(&myMutex);

    return returnValue;
}

static void setKeepGoing(bool newValue) {
    pthread_mutex_lock(&myMutex);
    keepGoing = newValue;
    pthread_mutex_unlock(&myMutex);
}

static void dispatch(Trame* trame, Header* header) {
    switch (header->commande) {
        case ASK_CALIBRATION_POSITIONS:
            Geographer_askCalibrationPositions();
            break;
        case SIGNAL_CALIBRATION_POSITION:;
            CalibrationPositionId calibrationPositionId = TranslatorLOG_translateForSignalCalibrationPosition(trame);
            Geographer_validatePosition(calibrationPositionId);
            break;
        default:
        case SEND_EXPERIMENTAL_TRAJECTS:
        case SEND_MEMORY_PROCESSOR_LOAD:
        case SEND_ALL_BEACONS_DATA:
        case SEND_CURRENT_POSITION:
        case REP_CALIBRATION_POSITIONS:
        case SEND_EXPERIMENTAL_POSITIONS:
        case SIGNAL_CALIRATION_END:
            break;
    }

    free(trame);
    free(header);
}

static int16_t readHeader(Header* header) {
    Trame* headerTrame;
    headerTrame = malloc(SIZE_HEADER);
    int16_t returnError;
    returnError = PostmanLOG_readMsg(headerTrame, SIZE_HEADER);//on lit la trame contenant le header

    if (returnError == 0) {
        TranslatorLOG_translateTrameToHeader(headerTrame, header); //on traduit la trame header en header
    }

    return returnError;
}

static void* readMsg() {
    TRACE("readMsg%s", "\n");
    while (getKeepGoing()) {
        Header header;
        int16_t returnError;

        returnError = readHeader(&header);

        if (returnError < 0) {
            LOG("Error on reading communication%s", "\n");
            DispatcherLOG_stop();
        } else {
            Trame* trame;
            trame = malloc(header.size);

            PostmanLOG_readMsg(trame, header.size);

            dispatch(trame, &header);
        }
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


extern int8_t DispatcherLOG_new() {
    int8_t returnError = EXIT_SUCCESS;

    returnError = pthread_mutex_init(&myMutex, NULL);
    setKeepGoing(false);

    return returnError;
}

extern int8_t DispatcherLOG_free() {
    /* Nothing to do */
    return EXIT_SUCCESS;
}

extern int8_t DispatcherLOG_start() {
    int8_t returnError = EXIT_FAILURE;
    setKeepGoing(true);

    returnError = pthread_create(&myThreadListen, NULL, &readMsg, NULL);
        // premier thread pour recevoir
    if (returnError != EXIT_SUCCESS) {
        setKeepGoing(false);
    }

    return returnError;
}

extern int8_t DispatcherLOG_stop() {
    int8_t returnError = EXIT_SUCCESS;
    setKeepGoing(false);

    returnError = pthread_join(myThreadListen, NULL);

    return returnError;
}

extern int8_t DispatcherLOG_setConnectionState(ConnectionState connectionState) {

    if (connectionState == CONNECTED) {
        Geographer_signalConnectionEstablished();
        DispatcherLOG_start();

    } else {
        Geographer_signalConnectionDown();
        DispatcherLOG_stop();
    }

    return 0;
}