/**
 * @file main.c
 *
 * @brief Fichier main du de GEOLOGIE
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

#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>

#include "ManagerLOG/managerLOG.h"
#include "tools.h"

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
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Fonction main de GEOLOGIE
 *
 * @return int 0
 */
int main() {
    LOG("%s", "\033[2J\033[;H");
    setUp();

    ManagerLOG_startGEOLOGIE();

    pthread_cond_wait(&cond, &mutex);

    ManagerLOG_stopGEOLOGIE();

    tearDown();

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
    assert(returnErrorSignal != SIG_ERR);

    returnError = pthread_mutex_init(&mutex, NULL);
    assert(returnError >= 0);

    returnError = pthread_cond_init(&cond, NULL);
    assert(returnError >= 0);

    returnError = atexit(errorHandler);
    assert(returnError >= 0);
}

static void tearDown(void) {
    int8_t returnError = EXIT_SUCCESS;
    sig_t returnErrorSignal = NULL;

    returnErrorSignal = signal(SIGINT, intHandler);
    assert(returnErrorSignal != SIG_ERR);

    returnError = pthread_mutex_destroy(&mutex);
    assert(returnError >= 0);

    returnError = pthread_cond_destroy(&cond);
    assert(returnError >= 0);
}

static void intHandler(int _) {
    LOG("Press CTRL+C%s", "\n");

    int8_t returnError = EXIT_SUCCESS;

    returnError = pthread_cond_signal(&cond);
    assert(returnError >= 0);
}

static void errorHandler(void) {
    LOG("Catch an error%s", "\n");

    int8_t returnError = EXIT_SUCCESS;

    returnError = pthread_cond_signal(&cond);
    assert(returnError >= 0);
}
