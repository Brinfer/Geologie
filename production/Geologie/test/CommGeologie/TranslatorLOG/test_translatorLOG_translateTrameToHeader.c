/**
 * @file test_translatorLOG_translateTrameToHeader.c
 *
 * @brief Ensemble de test pour tester TranslatorLOG_translateTrameToHeader.
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

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "cmocka.h"

#include "CommGeologie/TranslatorLOG/translatorLOG.c"
#include "CommGeologie/com_common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Structure passee aux fonctions tests.
 */
typedef struct {
    Trame trameInput[SIZE_HEADER];  /**< La trame passee a TranslatorLOG_translateTrameToHeader */
    Header headerExpected;          /**< Le resultat attendue pars la fontion TranslatorLOG_translateTrameToHeader */
} ParameterTestHeader;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Ensemble des donnees de tests.
 */
static ParameterTestHeader parameterTest[] = {
    {
        .headerExpected = {.commande = ASK_CALIBRATION_POSITIONS, .size = 0},
        .trameInput = {
            0x01,           // CMD
            0x00, 0x00      // SIZE
        }
    },
    {
        .headerExpected = {.commande = SEND_EXPERIMENTAL_POSITIONS, .size = 255},
        .trameInput = {
            0x02,           // CMD
            0x00, 0xFF      // SIZE
        }
    },
    {
        .headerExpected = {.commande = SEND_EXPERIMENTAL_TRAJECTS, .size = 65280},
        .trameInput = {
            0x03,           // CMD
            0xFF, 0x00      // SIZE
        }
    },
    {
        .headerExpected = {.commande = SEND_MEMORY_PROCESSOR_LOAD, .size = 65535},
        .trameInput = {
            0x04,           // CMD
            0xFF, 0xFF      // SIZE
        }
    },
    {
        .headerExpected = {.commande = SEND_ALL_BEACONS_DATA, .size = 43690},
        .trameInput = {
            0x05,           // CMD
            0xAA, 0xAA      // SIZE
        }
    },
    {
        .headerExpected = {.commande = SIGNAL_END_CALIBRATION_POSITION, .size = 21845},
        .trameInput = {
            0x0C,           // CMD
            0x55, 0x55      // SIZE
        }
    },
};

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
 * @brief La fonction test permettant de verifier le bon fonctionnement de TranslatorLOG_translateTrameToHeader.
 *
 * @param state Les donnees de test #ParameterTestHeader.
 */
static void test_TranslatorLOG_translateTrameToHeader(void** state);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Ensemble des tests a executer.
 */
static const struct CMUnitTest tests[] = {
    cmocka_unit_test_prestate(test_TranslatorLOG_translateTrameToHeader, &(parameterTest[0])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateTrameToHeader, &(parameterTest[1])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateTrameToHeader, &(parameterTest[2])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateTrameToHeader, &(parameterTest[3])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateTrameToHeader, &(parameterTest[4])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateTrameToHeader, &(parameterTest[5])),
};


extern int test_TranslatorLOG_run_translateTrameToHeader(void) {
    return cmocka_run_group_tests_name("Test of the module translatorLOG for function TranslatorLOG_translateTrameToHeader", tests, NULL, NULL);
}

static void test_TranslatorLOG_translateTrameToHeader(void** state) {
    ParameterTestHeader* parameter = (ParameterTestHeader*) *state;

    Header currentResult;
    TranslatorLOG_translateTrameToHeader(parameter->trameInput, &currentResult);

    /* CMD */
    assert_int_equal(parameter->headerExpected.commande, currentResult.commande);

    /* Size */
    assert_int_equal(parameter->headerExpected.size, currentResult.size);
}
