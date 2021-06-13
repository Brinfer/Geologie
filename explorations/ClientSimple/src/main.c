/**
 * @file mockClient.c
 *
 * @brief Simule un client simple.
 *
 * @version 2.0
 * @date 13-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "Comm/client.h"
#include "View/view.h"
#include "tools.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief La variable condition que le main thread devra attendre pour reprendre son execution.
 *
 */
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

/**
 * @brief Le mutex associe a la variable condition.
 *
 */
static pthread_mutex_t viewMutex = PTHREAD_MUTEX_INITIALIZER;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Gere le CTRL+C.
 *
 * @param _ Le numero du signal, ici est ignore.
 */
static void intHandler(int _);

/**
 * @brief Gere les erreurs.
 */
static void errorHandler(void);

/**
 * @brief Met en place l'environnement de GEOLOGIE.
 */
static void setUp(void);

/**
 * @brief Demantele l'environnement de GEOLOGIE.
 */
static void tearDown(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[]) {

    setUp();

    Client_new();
    Client_start();

    View_new();
    View_start();

    atexit(errorHandler);

    pthread_cond_wait(&cond, &viewMutex);

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void setUp(void) {
    int8_t returnError = EXIT_SUCCESS;
    sig_t returnErrorSignal = NULL;

    returnErrorSignal = signal(SIGINT, intHandler);
    if (returnErrorSignal == SIG_ERR) {
        ERROR(true, "[Main] Error when seting up the signal");
        returnError = -1;
    } else {
        returnError = pthread_mutex_init(&viewMutex, NULL);
        ERROR(returnError < 0, "[Main] Error when creating the mutex");

        if (returnError >= 0) {
            returnError = pthread_cond_init(&cond, NULL);
            ERROR(returnError < 0, "[Main] Error when creating the condition");
        }
    }

    if (returnError < 0) {
        LOG("[Main] Error when setting up the Client ... Exit%s", "\n");
        exit(1);
    }
}

static void tearDown(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = pthread_mutex_destroy(&viewMutex);
    ERROR(returnError < 0, "[Main] Error when destroying the mutex");

    returnError -= pthread_cond_destroy(&cond);
    ERROR(returnError < 0, "[Main] Error when destroying the condition");

    ERROR(returnError < 0, "[Main] Error when creating the condition");
}

static void intHandler(int _) {
    TRACE("Press CTRL+C%s", "\n");

    int8_t returnError = EXIT_SUCCESS;

    returnError = pthread_cond_signal(&cond);
    ERROR(returnError < 0, "[Main] Error when intercepting the signal");
}

static void errorHandler(void) {
    TRACE("Catch an error%s", "\n");

    pthread_cond_signal(&cond); // in case of the function is called by an exit

    Client_stop();
    Client_free();

    View_stop();
    View_free();

    tearDown();
}
