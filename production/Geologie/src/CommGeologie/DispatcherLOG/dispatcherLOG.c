/**
 * @file dispatcherLOG.c
 *
 * @brief Recoit les trames sur la socket et execute des actions
 *
 * @version 1.0
 * @date 06-06-2021
 * @author BRIENT Nathan
 * @copyright Geo-Boot
 * @license BSD 2-clauses
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

#include <errno.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

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
//                                              Prototypes de fonctions
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int8_t DispatcherLOG_new() {
    int8_t returnError = EXIT_SUCCESS;

    returnError = pthread_mutex_init(&myMutex, NULL);
    ERROR(returnError < 0, "[DispatcheurLOG] Error when creating the mutex.");

    if (returnError < 0) {
        setKeepGoing(false);
    }

    return returnError;
}

extern int8_t DispatcherLOG_free() {
    int8_t returnError = EXIT_SUCCESS;

    returnError = pthread_mutex_destroy(&myMutex);
    ERROR(returnError < 0, "[DispatcheurLOG] Error when destroying the mutex.");

    return returnError;
}

extern int8_t DispatcherLOG_start() {
    int8_t returnError = EXIT_FAILURE;
    setKeepGoing(true);

    returnError = pthread_create(&myThreadListen, NULL, &readMsg, NULL);

    if (returnError >= 0) {
        setKeepGoing(false);
    } else {
        ERROR(true, "[DispatcheurLOG] Error when creating the processus.");
        returnError = pthread_create(&myThreadListen, NULL, &readMsg, NULL);
        ERROR(returnError < 0, "[DispatcheurLOG] Error when creating the processus ... Abondement.");
    }

    return returnError;
}

extern int8_t DispatcherLOG_stop() {
    int8_t returnError = EXIT_SUCCESS;
    setKeepGoing(false);

    returnError = pthread_join(myThreadListen, NULL);
    if (returnError >= 0) {
        setKeepGoing(false);
    } else {
        ERROR(true, "[DispatcheurLOG] Error when joining the processus.");
        returnError = pthread_join(myThreadListen, NULL);
        ERROR(returnError < 0, "[DispatcheurLOG] Error when joining the processus ... Abondement.");
    }

    return returnError;
}

extern int8_t DispatcherLOG_setConnectionState(ConnectionState connectionState) {
    int8_t returnError = EXIT_FAILURE;

    if (connectionState == CONNECTED) {
        Geographer_signalConnectionEstablished();
        returnError = DispatcherLOG_start();
        ERROR(returnError < 0, "[DispatcheurLOG] Error when starting the processus.");

    } else {
        Geographer_signalConnectionDown();
        returnError = DispatcherLOG_stop();
        ERROR(returnError < 0, "[DispatcheurLOG] Error when stopping the processus.");
    }

    return returnError;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
}

static int16_t readHeader(Header* header) {
    Trame headerTrame[SIZE_HEADER];
    int16_t returnError;

    returnError = PostmanLOG_readMsg(headerTrame, SIZE_HEADER);//on lit la trame contenant le header

    if (returnError >= 0) {
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
            ERROR(true, "[DispatcheurLOG] Can't read the header ... Stop the processus.");
            setKeepGoing(false);
        } else {
            Trame trame[header.size];

            returnError = PostmanLOG_readMsg(trame, header.size);

            if (returnError < 0) {
                ERROR(true, "[DispatcheurLOG] Can't read the message.");
                setKeepGoing(true);
            } else {
                dispatch(trame, &header);
            }
        }
    }

    return 0;
}
