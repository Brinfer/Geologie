/**
 * @file test_translatorLOG_translateTrameToHeader.c
 *
 * @brief Ensemble de test pour tester TranslatorLOG_translateForSendExperimentalPositions.
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

#include "CommGeologie/TranslatorLOG/translatorLOG.h"
#include "CommGeologie/com_common.h"
#include "common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief La taille d'une #Position en octet.
 *
 */
#define SIZE_POSITION (8)

/**
 * @brief Structure passee au fonction test.
 */
typedef struct {
    uint8_t nbExperimentalPositionInput;                /**< Le nombre de position experimentale passe a TranslatorLOG_translateForSendExperimentalPositions. */
    ExperimentalPosition* experimentalPositionInput;    /**< Un pointeur vers le tableau de position experimentale passee a TranslatorLOG_translateForSendExperimentalPositions */
    Trame* trameExpected;                               /**< La trame attendue en resultat retournee par TranslatorLOG_translateForSendExperimentalPositions*/
    uint8_t sizeTrameExpected;                          /**< La taille de la trame attendue en resultat retournee par TranslatorLOG_translateForSendExperimentalPositions. */
} ParameterTestExperimentalPosition;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Les positions experimentations du test A.
 */
static ExperimentalPosition experimentalPositionTest_A[5] = {
    {.id = 0, .position = {.X = 0, .Y = 0} },
    {.id = 1, .position = {.X = 1, .Y = 1} },
    {.id = 170, .position = {.X = 2857740885, .Y = 1437226410} },
    {.id = 255, .position = {.X = 4278255360, .Y = 16711935} },
    {.id = 85, .position = {.X = 1431633920, .Y = 21845} },
};

/**
 * @brief La trame attendue du test A.
 */
static Trame trameExpected_A[49] = {
    // Header
    SEND_EXPERIMENTAL_POSITIONS,    // CMD
    0x00, 0x2E,                     // Size - 46

    0x05,                           // NB ExperimentalPosition - 5

    // ExperimentalPosition 0
    0x00,                           // ID
    0x00, 0x00, 0x00, 0x00,         // Position X
    0x00, 0x00, 0x00, 0x00,         // Position Y

    // ExperimentalPosition 1
    0x01,                           // ID
    0x00, 0x00, 0x00, 0x01,         // Position X
    0x00, 0x00, 0x00, 0x01,         // Position Y

    // ExperimentalPosition AA
    0xAA,                           // ID
    0xAA, 0x55, 0xAA, 0x55,         // Position X
    0x55, 0xAA, 0x55, 0xAA,         // Position Y

    // ExperimentalPosition FF
    0xFF,                           // ID
    0xFF, 0x00, 0xFF, 0x00,         // Position X
    0x00, 0xFF, 0x00, 0xFF,         // Position Y

    // ExperimentalPosition 55
    0x55,                           // ID
    0x55, 0x55, 0x00, 0x00,         // Position X
    0x00, 0x00, 0x55, 0x55,         // Position Y
};

/**
 * @brief Les positions experimentations du test B.
 */
static ExperimentalPosition experimentalPositionTest_B[2] = {
    {.id = 204, .position = {.X = 2295105740, .Y = 3431517320} },
    {.id = 136, .position = {.X = 2882400016, .Y = 33479866} },
};

/**
 * @brief La trame attendue du test B.
 */
static Trame trameExpected_B[22] = {
    // Header
    SEND_EXPERIMENTAL_POSITIONS,    // CMD
    0x00, 0x13,                     // Size - 19

    0x02,                           // NB ExperimentalPosition - 2

    // ExperimentalPosition 204
    0xCC,                           // ID
    0x88, 0xCC, 0x88, 0xCC,         // Position X
    0xCC, 0x88, 0xCC, 0x88,         // Position Y

    // ExperimentalPosition 136
    0x88,                           // ID
    0xAB, 0xCD, 0xEF, 0x10,         // Position X
    0x01, 0xFE, 0xDC, 0xBA,         // Position Y
};

/**
 * @brief Ensemble des donnees de tests.
 */
static ParameterTestExperimentalPosition parameterTest[] = {
    {
        .nbExperimentalPositionInput = 5,
        .experimentalPositionInput = experimentalPositionTest_A,
        .trameExpected = trameExpected_A,
        .sizeTrameExpected = 49
    },
    {
        .nbExperimentalPositionInput = 2,
        .experimentalPositionInput = experimentalPositionTest_B,
        .trameExpected = trameExpected_B,
        .sizeTrameExpected = 22
    }
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Execute les tests de TranslatorLOG_translateForSendExperimentalPositions.
 *
 * @return int 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int test_TranslatorLOG_run_translateForSendExperimentalPositions(void);

/**
 * @brief La fonction test permettant de verifier le bon fonctionnement de TranslatorLOG_translateForSendExperimentalPositions.
 *
 * @param state Les donnees de test #ParameterTestExperimentalPosition.
 */
static void test_TranslatorLOG_translateForSendExperimentalPositions(void** state);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Ensemble des tests a executer.
 */
static const struct CMUnitTest testsExperimentalPosition[] = {
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendExperimentalPositions, &(parameterTest[0])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendExperimentalPositions, &(parameterTest[1])),
};


extern int test_TranslatorLOG_run_translateForSendExperimentalPositions(void) {
    return cmocka_run_group_tests_name("Test of the module translatorLOG for function TranslatorLOG_translateForSendExperimentalPositions", testsExperimentalPosition, NULL, NULL);
}

static void test_TranslatorLOG_translateForSendExperimentalPositions(void** state) {
    ParameterTestExperimentalPosition* parameter = (ParameterTestExperimentalPosition*) *state;

    /* Size */
    uint16_t sizeResult = TranslatorLOG_getTrameSize(SEND_EXPERIMENTAL_POSITIONS, parameter->nbExperimentalPositionInput);
    assert_int_equal(parameter->sizeTrameExpected, sizeResult);

    Trame trameResult[sizeResult];
    TranslatorLOG_translateForSendExperimentalPositions(parameter->nbExperimentalPositionInput, parameter->experimentalPositionInput, trameResult);

    /* Test trame */
    assert_memory_equal(parameter->trameExpected, trameResult, sizeResult);
}
