/**
 * @file test_translatorLOG.c
 *
 * @brief Permet de lancer l'ensemble des tests pour le module TranslatorLOG
 *
 * @version 2.0
 * @date 09-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Nombre de suites de tests a excuter.
 */
#define NB_SUITE_TESTS_TRANSLATOR_LOG (4)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Execute les tests de TranslatorLOG_translateTrameToHeader.
 *
 * @return int 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int test_TranslatorLOG_run_translateTrameToHeader(void);

/**
 * @brief Execute les tests de TranslatorLOG_translateForSendCurrentPosition.
 *
 * @return int 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int test_TranslatorLOG_run_translateForSendCurrentPosition(void);

/**
 * @brief Execute les tests de TranslatorLOG_translateForSendExperimentalPositions.
 *
 * @return int 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int test_TranslatorLOG_run_translateForSendExperimentalPositions(void);

/**
 * @brief Execute les tests de TranslatorLOG_translateForSendMemoryAndProcessorLoad.
 *
 * @return int 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int test_TranslatorLOG_run_translateForSendMemoryAndProcessorLoad(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Suite des tests a executer.
 *
 */
static int (*suite_tests[])(void) = {
   test_TranslatorLOG_run_translateTrameToHeader,
   test_TranslatorLOG_run_translateForSendCurrentPosition,
   test_TranslatorLOG_run_translateForSendExperimentalPositions,
   test_TranslatorLOG_run_translateForSendMemoryAndProcessorLoad
};

/**
 * Fonction lançant la suite des tests.
 */
extern int translatorLOG_run_tests() {
    for (int i = 0; i < NB_SUITE_TESTS_TRANSLATOR_LOG; i++) {
        suite_tests[i]();
    }

    return EXIT_SUCCESS;
}
