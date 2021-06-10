/**
 * @file managerLOG.c
 *
 * @brief Permet le demarrage et l’extinction de GEOLOGIE. A aussi pour role de creer tous les objets
 *  et de demarrer les differentes machines à etats.
 * Cette objet permet aussi de stopper les machines a etats et de detruire tous les objets
 * instancies à l’extinction.
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

#include "managerLOG.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "../tools.h"
#include "../UI/ui.h"
#include "../Geographer/geographer.h"
#include "../CommGeologie/PostmanLOG/postmanLOG.h"
#include "../CommGeologie/DispatcherLOG/dispatcherLOG.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void ManagerLOG_startGEOLOGIE(void) {
    /* New */

    LOG("Initialization of GEOLOGIE%s", "\n");

    int8_t returnError = EXIT_SUCCESS;

    returnError = UI_new();
    assert(returnError >= 0);

    returnError = PostmanLOG_new();
    assert(returnError >= 0);

    returnError = Geographer_new();
    assert(returnError >= 0);

    returnError = DispatcherLOG_new();
    assert(returnError >= 0);

    /* Start */

    LOG("Start of GEOLOGIE%s", "\n");

    returnError = PostmanLOG_start();
    assert(returnError >= 0);

    returnError = Geographer_askSignalStartGeographer();
    assert(returnError >= 0);


    returnError = UI_askSignalBeginningGEOLOGIE();
    assert(returnError >= 0);
}

extern void ManagerLOG_stopGEOLOGIE(void) {
    /* Stop */

    LOG("Stop of GEOLOGIE%s", "\n");

    int8_t returnError = EXIT_SUCCESS;

    returnError = PostmanLOG_stop();
    assert(returnError >= 0);

    returnError = Geographer_askSignalStopGeographer();
    assert(returnError >= 0);

    returnError = UI_askSignalEndingGEOLOGIE();
    assert(returnError >= 0);

    /* Free */

    LOG("Destruction of GEOLOGIE%s", "\n");

    returnError = DispatcherLOG_free();
    assert(returnError >= 0);

    returnError = Geographer_free();
    assert(returnError >= 0);

    returnError = PostmanLOG_free();
    assert(returnError >= 0);

    returnError = UI_free();
    assert(returnError >= 0);
}
