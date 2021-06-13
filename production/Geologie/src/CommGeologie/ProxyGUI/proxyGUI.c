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
#include "../../tools.h"
#include "../com_common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Envoie le message aupres de PostmanLOG.
 *
 * @param trame La trame a envoyer.
 * @param size La taille de la trame a envoyer.
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t sendMsg(Trame* trame, uint16_t size);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int8_t ProxyGUI_new() {
    /* Nothing to do */
    return EXIT_SUCCESS;
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

extern int8_t ProxyGUI_setCalibrationPositions(CalibrationPosition* calibrationPositions, uint8_t nbCalibrationPosition) {
    LOG("[ProxyGUI] Send the calibration position.%s", "\n");

    int8_t returnError = EXIT_FAILURE;
    Trame* trame;

    uint16_t sizeTrame = TranslatorLOG_getTrameSize(REP_CALIBRATION_POSITIONS, nbCalibrationPosition);

    trame = malloc(sizeTrame);
    TranslatorLOG_translateForRepCalibrationPosition(sizeTrame, calibrationPositions, trame);

    returnError = sendMsg(trame, sizeTrame);

    return returnError;
}

extern int8_t ProxyGUI_signalEndCalibrationPosition() {
    LOG("[ProxyGUI] Signal the end of the calibration at a position.%s", "\n");

    int8_t returnError = EXIT_FAILURE;
    Trame* trame;

    u_int16_t sizeTrame = TranslatorLOG_getTrameSize(SIGNAL_END_CALIBRATION_POSITION, 0);

    trame = malloc(sizeTrame);
    TranslatorLOG_translateForSignalCalibrationEndPosition(trame);

    returnError = sendMsg(trame, sizeTrame);

    return returnError;
}

extern int8_t ProxyGUI_signalEndCalibration() {
    LOG("[ProxyGUI] Signal the end of the calibration.%s", "\n");

    int8_t returnError = EXIT_FAILURE;
    Trame* trame;

    u_int16_t sizeTrame = TranslatorLOG_getTrameSize(SIGNAL_CALIRATION_END, 0);

    trame = malloc(sizeTrame);
    TranslatorLOG_translateForSignalCalibrationEnd(trame);

    returnError = sendMsg(trame, sizeTrame);

    return returnError;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int8_t sendMsg(Trame* trame, uint16_t size) {
    int8_t returnError = PostmanLOG_sendMsg(trame, size);
    ERROR(returnError < 0, "[ProxyGUI] Can't send the message ... Abondement");

    return returnError;
}
