/**
 * @file test_translatorLOG.c
 *
 * @brief Permet de lancer l'ensemble des tests pour le module TranslatorLOG.
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
#define NB_SUITE_TESTS_TRANSLATOR_LOG (7)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Execute les tests de TranslatorLOG_translateTrameToHeader.
 *
 * @return int32_t 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int32_t test_TranslatorLOG_run_translateTrameToHeader(void);

/**
 * @brief Execute les tests de TranslatorLOG_translateForSendCurrentPosition.
 *
 * @return int32_t 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int32_t test_TranslatorLOG_run_translateForSendCurrentPosition(void);

/**
 * @brief Execute les tests de TranslatorLOG_translateForSendExperimentalPositions.
 *
 * @return int32_t 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int32_t test_TranslatorLOG_run_translateForSendExperimentalPositions(void);

/**
 * @brief Execute les tests de TranslatorLOG_translateForSendMemoryAndProcessorLoad.
 *
 * @return int32_t 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int32_t test_TranslatorLOG_run_translateForSendMemoryAndProcessorLoad(void);

/**
 * @brief Execute les tests de TranslatorLOG_translateForSendAllBeaconsData.
 *
 * @return int32_t 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int32_t test_TranslatorLOG_run_translateForSendAllBeaconsData(void);

/**
 * @brief Execute les tests de TranslatorLOG_translateForSendExperimentalTrajects.
 *
 * @return int32_t 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int32_t test_TranslatorLOG_run_translateForSendExperimentalTrajects(void);

/**
 * @brief Execute les tests de TranslatorLOG_translateForSendCalibrationData.
 *
 * @return int32_t 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int32_t test_TranslatorLOG_run_translateForSendCalibrationData(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Suite des tests a executer.
 */
static int32_t (*suite_tests[])(void) = {
    test_TranslatorLOG_run_translateTrameToHeader,
    test_TranslatorLOG_run_translateForSendCurrentPosition,
    test_TranslatorLOG_run_translateForSendExperimentalPositions,
    test_TranslatorLOG_run_translateForSendMemoryAndProcessorLoad,
    test_TranslatorLOG_run_translateForSendAllBeaconsData,
    test_TranslatorLOG_run_translateForSendExperimentalTrajects,
    test_TranslatorLOG_run_translateForSendCalibrationData
};

/**
 * @brief Fonction lançant la suite des tests pour TranslatorLOG.
 *
 * @return int32_t 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int32_t translatorLOG_run_tests(void) {
    for (int32_t i = 0; i < NB_SUITE_TESTS_TRANSLATOR_LOG; i++) {
        suite_tests[i]();
    }

    return EXIT_SUCCESS;
}
