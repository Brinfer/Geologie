/**
 * @file ui.c
 *
 * @brief Permet de faire le lien avec l’utilisateur  en lui indiquant l’état du logiciel (démarré ou éteint).
 *
 * @version 2.0
 * @date 03-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ui.h"

#include <stdlib.h>
#include <stdint.h>

#include "tools.h"
#include "Led/led.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Affiche le bon demarrage du logiciel.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t signalBeginningGEOLOGIE(void);

/**
 * @brief Affiche la bonne extinction du logiciel.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t signalEndingGEOLOGIE(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


extern int8_t UI_new(void) {
    int8_t returnError = EXIT_FAILURE;

    returnError = Led_new();
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}

extern int8_t UI_askSignalBeginningGEOLOGIE(void) {
    int8_t returnError = EXIT_FAILURE;

    returnError = signalBeginningGEOLOGIE();
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}


extern int8_t UI_askSignalEndingGEOLOGIE(void) {
    int8_t returnError = EXIT_FAILURE;

    returnError = signalEndingGEOLOGIE();
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}

extern int8_t UI_free(void) {
    int8_t returnError = EXIT_FAILURE;

    returnError = Led_free();
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int8_t signalBeginningGEOLOGIE(void) {
    int8_t returnError = EXIT_FAILURE;

    returnError = Led_ledOn();
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}

static int8_t signalEndingGEOLOGIE(void) {
    int8_t returnError = EXIT_FAILURE;

    returnError = Led_ledOff();
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}
