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

    TRACE("%sInitialization of GEOLOGIE%s%s", "\033[47m\033[30m", "\033[0m", "\n");

    int8_t returnError = EXIT_SUCCESS;

    returnError = UI_new();
    STOP_ON_ERROR(returnError < 0);

    returnError = PostmanLOG_new();
    STOP_ON_ERROR(returnError < 0);

    returnError = Geographer_new();
    STOP_ON_ERROR(returnError < 0);

    returnError = DispatcherLOG_new();
    STOP_ON_ERROR(returnError < 0);

    /* Start */

    TRACE("%sStart of GEOLOGIE%s%s", "\033[47m\033[30m", "\033[0m", "\n");

    returnError = PostmanLOG_start();
    STOP_ON_ERROR(returnError < 0);

    returnError = Geographer_askSignalStartGeographer();
    STOP_ON_ERROR(returnError < 0);


    returnError = UI_askSignalBeginningGEOLOGIE();
    STOP_ON_ERROR(returnError < 0);

    returnError = DispatcherLOG_start();
    STOP_ON_ERROR(returnError < 0);
}

extern void ManagerLOG_stopGEOLOGIE(void) {
    /* Stop */

    TRACE("%sStop of GEOLOGIE%s%s", "\033[47m\033[30m", "\033[0m", "\n");

    int8_t returnError = EXIT_SUCCESS;

    //returnError = DispatcherLOG_stop();
    STOP_ON_ERROR(returnError < 0);


    returnError = PostmanLOG_stop();
    STOP_ON_ERROR(returnError < 0);



    returnError = Geographer_askSignalStopGeographer();
    STOP_ON_ERROR(returnError < 0);

    returnError = UI_askSignalEndingGEOLOGIE();
    STOP_ON_ERROR(returnError < 0);
    TRACE("UI_askSignalEndingGEOLOGIE %s", "\n");
/* Free */

    TRACE("%sDestruction of GEOLOGIE%s%s", "\033[47m\033[30m", "\033[0m", "\n");

    returnError = DispatcherLOG_free();
    STOP_ON_ERROR(returnError < 0);

    returnError = Geographer_free();
    STOP_ON_ERROR(returnError < 0);

    returnError = PostmanLOG_free();
    STOP_ON_ERROR(returnError < 0);

    returnError = UI_free();
    STOP_ON_ERROR(returnError < 0);
}
