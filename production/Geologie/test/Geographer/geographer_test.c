/**
 * @file geographer_test.c
 *
 * @brief Ensemble de test pour Geographer
 *
 * @version 1.0
 * @date 11-06-2021
 * @author LECENNE Gabriel
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <limits.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include "cmocka.h"

#include "Geographer/geographer.c"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Barrière de synchronisation en le thread de test (pour l'objet actif) et le thread de scénariob.
static pthread_barrier_t barrier_scenario;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Lance la suite de test du module geographer.
 *
 * @return int32_t 0 en cas de succes ou le numero du test qui a echoue.
 */
int32_t geographer_run_tests();

int32_t setUp(void** state);

int32_t tearDown(void** state);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t setUp(void** state){
	pthread_barrier_init(&barrier_scenario, NULL, 2);
	return 0; 
}

static int tear_down(void **state) {
	pthread_barrier_destroy(&barrier_scenario);
	return 0;
}

extern int geographer_run_tests(void) {
    return cmocka_run_group_tests_name("Test of the Geographer module", NULL, NULL, NULL);
}


