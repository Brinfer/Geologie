/**
 * @file ui.c
 *
 * @brief Permet de faire le lien avec l’utilisateur  en lui indiquant l’etat de GEOLOGIE (demarre ou eteint).
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

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "tools.h"
#include "Led/led.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Affiche le bon demarrage de GEOLOGIE.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t signalBeginningGEOLOGIE(void);

/**
 * @brief Affiche la bonne extinction de GEOLOGIE.
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
    int8_t returnError = EXIT_SUCCESS;

    returnError = Led_new();
    ERROR(returnError < 0, "[UI] Error when setting up the LED");

    return returnError;
}

extern int8_t UI_askSignalBeginningGEOLOGIE(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = signalBeginningGEOLOGIE();
    ERROR(returnError < 0, "[UI] Error when signaling the begining");

    return returnError;
}

extern int8_t UI_askSignalEndingGEOLOGIE(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = signalEndingGEOLOGIE();
    ERROR(returnError < 0, "[UI] Error when signaling the end");

    return returnError;
}

extern int8_t UI_free(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = Led_free();
    assert(returnError >= 0);

    return returnError;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int8_t signalBeginningGEOLOGIE(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = Led_ledOn();

    return returnError;
}

static int8_t signalEndingGEOLOGIE(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = Led_ledOff();
    assert(returnError >= 0);

    return returnError;
}
