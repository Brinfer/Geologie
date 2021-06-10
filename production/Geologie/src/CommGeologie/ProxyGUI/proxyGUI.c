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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "proxyGUI.h"
#include "../TranslatorLOG/translatorLOG.h"
#include "../com_common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


extern int8_t ProxyGUI_new() {
    /* Nothing to do */
    return EXIT_SUCCESS;

    return 0;
}

extern int8_t ProxyGUI_free() {
    /* Nothing to do */
    return EXIT_SUCCESS;
}

extern int8_t ProxyGUI_start() {
    /* Nothing to do */
    return EXIT_SUCCESS;
}

extern int8_t ProxyGUI_stop() {
    /* Nothing to do */
    return EXIT_SUCCESS;
}

extern int8_t ProxyGUI_setCalibrationPositions(CalibrationPosition* calibrationPositions, u_int16_t size) {
    int8_t returnError = EXIT_FAILURE;
    Trame* trame;
    Commande commande = REP_CALIBRATION_POSITIONS;
    uint16_t sizeTrame = TranslatorLOG_getTrameSize(commande, size);
    trame = malloc(sizeTrame);
    TranslatorLOG_translateForRepCalibrationPosition(size, calibrationPositions, trame);
    returnError = PostmanLOG_sendMsg(trame, sizeTrame);

    return returnError;
}

extern int8_t ProxyGUI_signalEndCalibrationPosition() {
    int8_t returnError = EXIT_FAILURE;
    Trame* trame;
    Commande commande = SIGNAL_END_CALIBRATION_POSITION;
    u_int16_t sizeTrame = TranslatorLOG_getTrameSize(commande, 0);
        trame = malloc(sizeTrame);
    TranslatorLOG_translateForSignalCalibrationEndPosition(trame);
    returnError = PostmanLOG_sendMsg(trame, sizeTrame);

    return returnError;
}

extern int8_t ProxyGUI_signalEndCalibration() {
    int8_t returnError = EXIT_FAILURE;
    Trame* trame;
    Commande commande = SIGNAL_CALIRATION_END;
    u_int16_t sizeTrame = TranslatorLOG_getTrameSize(commande, 0);
    trame = malloc(sizeTrame);
    TranslatorLOG_translateForSignalCalibrationEnd(trame);
    returnError = PostmanLOG_sendMsg(trame, sizeTrame);

    return returnError;
}
