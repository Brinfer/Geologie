/**
 * @file proxyGUI.c
 * @author your name (you@domain.com)
 * @brief
 * @version 2.0
 * @date 2021-06-07
 *
 * @copyright Copyright (c) 2021
 *
 */
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

#include "proxyGUI.h"
#include "../TranslatorLOG/translatorLOG.h"
#include "../com_common.h"
#include <stdint.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





static int8_t keepGoing = 1;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





//static void performAction() {
    // switch () {
    // case ASK_CALIBRATION_POSITIONS:
    //     /* code */
    //     break;
    // case SEND_EXPERIMENTAL_POSITIONS:
    //     /* code */
    //     break;
    // case SEND_EXPERIMENTAL_TRAJECTS:
    //     /* code */
    //     break;
    // case SEND_MEMORY_PROCESSOR_LOAD:
    //     /* code */
    //     break;
    // case SEND_ALL_BEACONS_DATA:
    //     /* code */
    //     break;
    // case SEND_CURRENT_POSITION:
    //     /* code */
    //     break;
    // case REP_CALIBRATION_POSITIONS:
    //     /* code */
    //     break;
    // case SIGNAL_CALIBRATION_START:
    //     /* code */
    //     break;
    // case SIGNAL_CALIBRATION_POSITION:
    //     /* code */
    //     break;
    // case SIGNAL_CALIRATION_END:
    //     /* code */
    //     break;
    // default:
    //     break;
    // }

//}
/*
static int8_t sendMsgToQueue(MqMsg* msg) { //TODO pas besoin mutex, deja protege/a revoir
    int8_t returnError = EXIT_FAILURE;
    if (mq_send(descripteur, (char*) &msg, sizeof(msg), 0) == 0) {
        returnError = EXIT_SUCCESS;
    }
    return returnError;
}
*/

// static int8_t sendMsg() { //TODO pas besoin mutex, deja protege/a revoir
//     int8_t returnError = EXIT_FAILURE;

//     return returnError;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


extern int8_t ProxyGUI_new() {


    return 0;
}


extern int8_t ProxyGUI_free() {
    return 0;
}

extern int8_t ProxyGUI_start() {
    int8_t returnError = EXIT_FAILURE;
   
    return returnError;
}

extern int8_t ProxyGUI_stop() {
    int8_t returnError = EXIT_FAILURE;


    return returnError;
}


extern int8_t ProxyGUI_setCalibrationPositions(CalibrationPosition * calibrationPositions, u_int16_t size){
    int8_t returnError=EXIT_FAILURE;
    Trame * trame;
    Commande commande=REP_CALIBRATION_POSITIONS;
    uint16_t sizeTrame = TranslatorLog_getTrameSize(commande, NB_CALIBRATION_POSITIONS);
    trame=malloc(sizeTrame);
    TranslatorLog_translateForRepCalibrationPosition(size,calibrationPositions,trame);
    returnError = PostmanLOG_sendMsg(trame,sizeTrame); 

    return returnError;
}


extern int8_t ProxyGUI_signalEndCalibrationPosition(){
    int8_t returnError=EXIT_FAILURE;
    Trame * trame;
    Commande commande=SIGNAL_END_CALIBRATION_POSITION;
    u_int16_t sizeTrame = TranslatorLog_getTrameSize(commande, 0);
    trame=malloc(sizeTrame);
    Translator_translateForSignalCalibrationEnd(trame); 
    returnError = PostmanLOG_sendMsg(trame,sizeTrame); 

    return returnError;
}


extern int8_t ProxyGUI_signalEndCalibration(){
    int8_t returnError=EXIT_FAILURE;
    Trame * trame;
    Commande commande=SIGNAL_CALIRATION_END;
    u_int16_t sizeTrame = TranslatorLog_getTrameSize(commande, 0);
    trame=malloc(sizeTrame);
    Translator_translateForSignalCalibrationEnd(trame);
    returnError = PostmanLOG_sendMsg(trame,sizeTrame); 

    return returnError;
}


