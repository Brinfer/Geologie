/**
 * @file test_translatorLOG_sendExperimentalTrajects.c
 *
 * @brief Ensemble de test pour tester TranslatorLOG_translateForSendExperimentalTrajects.
 *
 * @version 2.0
 * @date 10-06-2021
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
 */
#define SIZE_POSITION (8)

/**
 * @brief Structure passee aux fonctions tests.
 */
typedef struct {
    uint8_t nbExperimentalTrajectInput;                /**< Le nombre de position experimentale passe a TranslatorLOG_translateForSendExperimentalTrajects. */
    ExperimentalTraject* experimentalTrajectsInput;    /**< Un pointeur vers le tableau de position experimentale passee a TranslatorLOG_translateForSendExperimentalTrajects */
    Trame* trameExpected;                              /**< La trame attendue en resultat retournee par TranslatorLOG_translateForSendExperimentalTrajects*/
    uint8_t sizeTrameExpected;                         /**< La taille de la trame attendue en resultat retournee par TranslatorLOG_translateForSendExperimentalTrajects. */
} ParameterTestExperimentalTraject;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Les position des trajets experimentaux du test A.
 */
static Position traject_A[2] = {
    {.X = 0, .Y = 0},
    {.X = 2863311530 , .Y = 1431655765 }
};

/**
 * @brief Les trajets experimentations du test A.
 */
static ExperimentalTraject experimentalTrajectsTest_A[1] = {
    {.id = 1, .nbPosition = 2, .traject = traject_A }
};

/**
 * @brief La trame attendue du test A.
 */
static Trame trameExpected_A[22] = {
    // Header
    SEND_EXPERIMENTAL_TRAJECTS, // CMD
    0x00, 0x13,                 // Size - 19

    0x01,                       // NB ExperimentalTraject - 1

    // ExperimentalTraject T1
    0x01,                       // ID
    0x02,                       // NB Position - 2
    0x00, 0x00, 0x00, 0x00,     // Position 1 X
    0x00, 0x00, 0x00, 0x00,     // Position 1 Y
    0xAA, 0xAA, 0xAA, 0xAA,     // Position 2 X
    0x55, 0x55, 0x55, 0x55,     // Position 2 Y
};

/**
 * @brief Les position des trajets experimentaux du test B.
 */
static Position traject_B1[1] = {
    {.X = 3431517320, .Y = 2295105740},
};

/**
 * @brief Les position des trajets experimentaux du test B.
 */
static Position traject_B2[3] = {
    {.X = 2857740885, .Y = 1437226410 },
    {.X = 4278255360 , .Y = 16711935 },
    {.X = 2882400001 , .Y = 285138106 }
};

/**
 * @brief Les trajets experimentations du test B.
 */
static ExperimentalTraject experimentalTrajectsTest_B[2] = {
    {.id = 255, .nbPosition = 1, .traject = traject_B1 },
    {.id = 0, .nbPosition = 3, .traject = traject_B2 }
};

/**
 * @brief La trame attendue du test B.
 */
static Trame trameExpected_B[40] = {
    // Header
    SEND_EXPERIMENTAL_TRAJECTS, // CMD
    0x00, 0x25,                 // Size - 19

    0x02,                       // NB ExperimentalTraject - 2

    // ExperimentalTraject T1
    0xFF,                       // ID
    0x01,                       // NB Position - 1
    0xCC, 0x88, 0xCC, 0x88,     // Position 1 X
    0x88, 0xCC, 0x88, 0xCC,     // Position 1 Y

    // ExperimentalTraject T1
    0x00,                       // ID
    0x03,                       // NB Position - 3
    0xAA, 0x55, 0xAA, 0x55,     // Position 1 X
    0x55, 0xAA, 0x55, 0xAA,     // Position 1 Y
    0xFF, 0x00, 0xFF, 0x00,     // Position 2 X
    0x00, 0xFF, 0x00, 0xFF,     // Position 2 Y
    0xAB, 0xCD, 0xEF, 0x01,     // Position 2 X
    0x10, 0xFE, 0xDC, 0xBA,     // Position 2 Y
};


/**
 * @brief Ensemble des donnees de tests.
 */
static ParameterTestExperimentalTraject parameterTest[] = {
    {
        .nbExperimentalTrajectInput = 1,
        .experimentalTrajectsInput = experimentalTrajectsTest_A,
        .trameExpected = trameExpected_A,
        .sizeTrameExpected = 22
    },
    {
        .nbExperimentalTrajectInput = 2,
        .experimentalTrajectsInput = experimentalTrajectsTest_B,
        .trameExpected = trameExpected_B,
        .sizeTrameExpected = 40
    },
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Execute les tests de TranslatorLOG_translateForSendExperimentalTrajects.
 *
 * @return int 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int test_TranslatorLOG_run_translateForSendExperimentalTrajects(void);

/**
 * @brief La fonction test permettant de verifier le bon fonctionnement de TranslatorLOG_translateForSendExperimentalTrajects.
 *
 * @param state Les donnees de test #ParameterTestExperimentalTraject.
 */
static void test_TranslatorLOG_translateForSendExperimentalTrajects(void** state);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Ensemble des tests a executer.
 */
static const struct CMUnitTest testsExperimentalTraject[] = {
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendExperimentalTrajects, &(parameterTest[0])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendExperimentalTrajects, &(parameterTest[1])),
};


extern int test_TranslatorLOG_run_translateForSendExperimentalTrajects(void) {
    return cmocka_run_group_tests_name("Test of the module translatorLOG for function TranslatorLOG_translateForSendExperimentalTrajects", testsExperimentalTraject, NULL, NULL);
}

static void test_TranslatorLOG_translateForSendExperimentalTrajects(void** state) {
    ParameterTestExperimentalTraject* parameter = (ParameterTestExperimentalTraject*) *state;

    /* Size */
    uint16_t sizeResult = TranslatorLOG_getTrameSizeExperimentalTraject(parameter->experimentalTrajectsInput, parameter->nbExperimentalTrajectInput);
    assert_int_equal(parameter->sizeTrameExpected, sizeResult);

    Trame currentResult[sizeResult];
    TranslatorLOG_translateForSendExperimentalTrajects(parameter->experimentalTrajectsInput, parameter->nbExperimentalTrajectInput, currentResult);

    /* Test trame */
    assert_memory_equal(parameter->trameExpected, currentResult, sizeResult);
}
