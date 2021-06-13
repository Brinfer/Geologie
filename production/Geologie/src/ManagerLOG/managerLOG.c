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
    if (returnError < 0) {
        LOG("[ManagerLOG] Fail to init UI ... Retry%s", "\n");

        UI_free();
        returnError = UI_new();

        if (returnError < 0) {
            LOG("[ManagerLOG] Fail to init UI ... Exit%s", "\n");
            exit(1);
        }
    }

    returnError = PostmanLOG_new();
    if (returnError < 0) {
        LOG("[ManagerLOG] Fail to init PostmanLOG ... Retry%s", "\n");

        PostmanLOG_free();
        returnError = PostmanLOG_new();

        if (returnError < 0) {
            LOG("[ManagerLOG] Fail to init PostmanLOG ... Exit%s", "\n");
            exit(1);
        }
    }

    returnError = Geographer_new();
    assert(returnError >= 0);

    returnError = DispatcherLOG_new();
    if (returnError < 0) {
        LOG("[ManagerLOG] Fail to init DispatcherLOG ... Retry%s", "\n");

        DispatcherLOG_free();
        returnError = DispatcherLOG_new();

        if (returnError < 0) {
            LOG("[ManagerLOG] Fail to init DispatcherLOG ... Exit%s", "\n");
            exit(1);
        }
    }

    /* Start */

    LOG("Start of GEOLOGIE%s", "\n");

    returnError = PostmanLOG_start();
    if (returnError < 0) {
        LOG("[ManagerLOG] Fail to start PostmanLOG ... Retry%s", "\n");

        returnError = PostmanLOG_start();

        if (returnError < 0) {
            LOG("[ManagerLOG] Fail to start PostmanLOG ... Exit%s", "\n");
            exit(1);
        }
    }

    returnError = Geographer_askSignalStartGeographer();
    assert(returnError >= 0);


    returnError = UI_askSignalBeginningGEOLOGIE();
    if (returnError < 0) {
        LOG("[ManagerLOG] Fail to start UI ... Retry%s", "\n");

        returnError = UI_askSignalBeginningGEOLOGIE();

        if (returnError < 0) {
            LOG("[ManagerLOG] Fail to start UI ... Exit%s", "\n");
            exit(1);
        }
    }
}

extern void ManagerLOG_stopGEOLOGIE(void) {
    /* Stop */

    LOG("Stop of GEOLOGIE%s", "\n");

    int8_t returnError = EXIT_SUCCESS;

    returnError = PostmanLOG_stop();
    if (returnError < 0) {
        LOG("[ManagerLOG] Fail to stop PostmanLOG ... Retry%s", "\n");
        returnError = PostmanLOG_stop();

        ERROR(returnError < 0, "[ManagerLOG] Fail to init PostmanLOG ... Continue");
    }

    returnError = Geographer_askSignalStopGeographer();
    assert(returnError >= 0);

    returnError = UI_askSignalEndingGEOLOGIE();
    if (returnError < 0) {
        LOG("[ManagerLOG] Fail to stop UI ... Retry%s", "\n");

        returnError = UI_askSignalEndingGEOLOGIE();
        ERROR(returnError < 0, "[ManagerLOG] Fail to stop UI ... Continue");
    }

    /* Free */

    LOG("Destruction of GEOLOGIE%s", "\n");

    returnError = DispatcherLOG_free();
    if (returnError < 0) {
        LOG("[ManagerLOG] Fail to destroy DispatcherLOG ... Retry%s", "\n");
        returnError = DispatcherLOG_free();
        ERROR(returnError < 0, "[ManagerLOG] Fail to destroy DispatcherLOG ... Continue");
    }

    returnError = Geographer_free();
    assert(returnError >= 0);

    PostmanLOG_free();
    if (returnError < 0) {
        LOG("[ManagerLOG] Fail to destroy PostmanLOG ... Retry%s", "\n");
        returnError = PostmanLOG_free();
        ERROR(returnError < 0, "[ManagerLOG] Fail to destroy PostmanLOG ... Continue");
    }

    returnError = UI_free();
    if (returnError < 0) {
        LOG("[ManagerLOG] Fail to destroy UI ... Retry%s", "\n");

        returnError = UI_free();
        ERROR(returnError < 0, "[ManagerLOG] Fail to destroy UI ... Continue");
    }
}
