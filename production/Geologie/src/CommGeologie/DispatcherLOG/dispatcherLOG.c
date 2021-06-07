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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////



static pthread_t myThreadListen;




static int8_t keepGoing = 1;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * @fn static void* readMsg();
 * @brief methode appelee par un thread pour recevoir les action a faire
 *
 * cette metode lira la socket et effectuera les actions
 */
static void* readMsg();

/**
 * @fn static void dispatch(Action_GEOGRAPHER action, MqMsg* msg);
 * @brief methode appelee par order pour executer les actions
 *
 * @param action action a effectuee
 * @param msg structure contenant les donnees a utiliser pour effectuer l'action
 */
static void dispatch(Trame* trame, Header* Header);



static void dispatch(Trame* trame, Header* header) {
    switch (header->commande)     {
    case ASK_CALIBRATION_POSITIONS:
        /* code */
        Geographer_askCalibrationPositions();
        break;
    case SEND_EXPERIMENTAL_POSITIONS:
        /* code */
        break;
    case SEND_EXPERIMENTAL_TRAJECTS:
        /* code */
        break;
    case SEND_MEMORY_PROCESSOR_LOAD:
        /* code */
        break;
    case SEND_ALL_BEACONS_DATA:
        /* code */
        break;
    case SEND_CURRENT_POSITION:
        /* code */
        break;
    case REP_CALIBRATION_POSITIONS:
        /* code */
        break;
    case SIGNAL_CALIBRATION_POSITION:;
        /* code */
        CalibrationPositionId calibrationPositionId= TranslatorLog_translateForSignalCalibrationPosition(trame);
        Geographer_validatePosition(calibrationPositionId);
        break;
    case SIGNAL_CALIRATION_END:
        /* code */
        break;
    default:
        break;
    }


    free(trame);
    free(header);
}



static void readHeader(Header* header) {
    Trame headerTrame[SIZE_HEADER];
    PostmanLOG_readMsg(headerTrame, SIZE_HEADER); //on lit la trame contenant le header
    TranslatorLog_translateTrameToHeader(headerTrame, header); //on traduit la trame header en header
}



static void* readMsg() {
    TRACE("readMsg%s","\n");
    while (keepGoing) {
        Header header;
        readHeader(&header); //on lit d'abord le header et on le traduit

        Trame* trame;
        trame = malloc(header.size);
        PostmanLOG_readMsg(trame, header.size); //on lit ensuite toute la trame //TODO mettre un mutex sur lecture/ecriture de trame et header
        dispatch(trame,&header);
    }
    return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


extern int8_t DispatcherLOG_new() {


    return 0;
}


extern int8_t DispatcherLOG_free() {
    return 0;
}

extern int8_t DispatcherLOG_start() {
    int8_t returnError = EXIT_FAILURE;

    if (returnError == EXIT_SUCCESS) {
        returnError = pthread_create(&myThreadListen, NULL, &readMsg, NULL);
        // premier thread pour recevoir
        if (keepGoing != EXIT_SUCCESS) {
            keepGoing = 0;
        }
    }

    return returnError;
}

extern int8_t DispatcherLOG_stop() {
    int8_t returnError = EXIT_FAILURE;

    if (returnError == EXIT_SUCCESS) {
        returnError = pthread_join(myThreadListen, NULL);

    }
    return returnError;
}

extern int8_t DispatcherLOG_setConnectionState(ConnectionState connectionState) {

    if(connectionState==CONNECTED){
        Geographer_signalConnectionEstablished();
    }else{
        Geographer_signalConnectionDown();
    }

    return 0;
}
