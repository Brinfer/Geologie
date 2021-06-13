/**
 * @file test_translatorLOG_repCalibrationPosition.c
 *
 * @brief Ensemble de test pour tester TranslatorLOG_translateForRepCalibrationPosition.
 *
 * @version 2.0
 * @date 11-06-2021
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
 * @brief Structure passee au fonction test.
 */
typedef struct {
    uint8_t nbCalibrationPositionInput;             /**< Le nombre de #CalibrationPosition passe en entree du test. */
    CalibrationPosition* calibrationPositionInput;  /**< Les #CalibrationPosition passee en entree du test. */
    uint16_t sizeTrameExpected;                     /**< La taille de la #Trame attendue en sortie du test. */
    Trame* trameExpected;                           /**< La #Trame attendue en sortie du test. */
} ParameterTestCalibrationData;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Les donnees de calibration du test A.
 */
static CalibrationPosition calibrationPosition_A[3] = {
    {.id = 0, .position = {.X = 0, .Y = 0 }},
    {.id = 255, .position = {.X = 4294967295, .Y = 4294967295 }},
    {.id = 170, .position = {.X = 2863311530, .Y = 2863311530 }}
};

/**
 * @brief La trame attendue du test A.
 */
static Trame trameExpected_A[31] = {
    REP_CALIBRATION_POSITIONS,  // CMD
    0x00, 0x1C,                 // Size - 28

    0x03,                       // Nb CalibrationPosition - 3

    // CalibrationPosition 0
    0x00,                       // ID CalibrationPosition
    0x00, 0x00, 0x00, 0x00,     // Position X 0
    0x00, 0x00, 0x00, 0x00,     // Position Y 0

     //CalibrationPosition 255
    0xFF,                        // ID CalibrationPosition
    0xFF, 0xFF, 0xFF, 0xFF,     // Position X 0
    0xFF, 0xFF, 0xFF, 0xFF,     // Position Y 0

     //CalibrationPosition 170
    0xAA,                       // ID CalibrationPosition
    0xAA, 0xAA, 0xAA, 0xAA,     // Position X 0
    0xAA, 0xAA, 0xAA, 0xAA      // Position Y 0
};

/**
 * @brief Les donnees de calibration du test B.
 */
static CalibrationPosition calibrationPosition_B[1] = {
    {.id = 170, .position = {.X = 2779096485, .Y = 1515870810 }}
};

/**
 * @brief La trame attendue du test B.
 */
static Trame trameExpected_B[13] = {
    REP_CALIBRATION_POSITIONS,  // CMD
    0x00, 0x0A,                 // Size - 10

    0x01,                       // Nb CalibrationPosition - 1

    // CalibrationPosition 0
    0xAA,                       // ID CalibrationPosition
    0xA5, 0xA5, 0xA5, 0xA5,     // Position X 0
    0x5A, 0x5A, 0x5A, 0x5A,     // Position Y 0
};

/**
 * @brief Ensemble des donnees de tests.
 */
static ParameterTestCalibrationData parameterTest[] = {
    {
        .nbCalibrationPositionInput = 3,
        .calibrationPositionInput = calibrationPosition_A,
        .sizeTrameExpected = 31,
        .trameExpected = trameExpected_A
    },
    {
        .nbCalibrationPositionInput = 1,
        .calibrationPositionInput = calibrationPosition_B,
        .sizeTrameExpected = 13,
        .trameExpected = trameExpected_B
    },
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Execute les tests de TranslatorLOG_translateForRepCalibrationPosition.
 *
 * @return int 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int test_TranslatorLOG_run_translateForRepCalibrationPosition(void);

/**
 * @brief La fonction test permettant de verifier le bon fonctionnement de TranslatorLOG_translateForRepCalibrationPosition.
 *
 * @param state Les donnees de test #ParameterTestCalibrationData.
 */
static void test_TranslatorLOG_translateForRepCalibrationPosition(void** state);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Ensemble des tests a executer.
 */
static const struct CMUnitTest testsAllBeaconsData[] = {
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForRepCalibrationPosition, &(parameterTest[0])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForRepCalibrationPosition, &(parameterTest[1])),
};


extern int test_TranslatorLOG_run_translateForRepCalibrationPosition(void) {
    return cmocka_run_group_tests_name("Test of the module translatorLOG for function TranslatorLOG_translateForRepCalibrationPosition", testsAllBeaconsData, NULL, NULL);
}

static void test_TranslatorLOG_translateForRepCalibrationPosition(void** state) {
    ParameterTestCalibrationData* parameter = (ParameterTestCalibrationData*) *state;

    /* Test trame sizeResult */
    uint16_t sizeResult = TranslatorLOG_getTrameSize(REP_CALIBRATION_POSITIONS, parameter->nbCalibrationPositionInput);
    assert_int_equal(parameter->sizeTrameExpected, sizeResult);

    Trame currentResult[sizeResult];
    TranslatorLOG_translateForRepCalibrationPosition(parameter->nbCalibrationPositionInput, parameter->calibrationPositionInput, currentResult);

    /* Test trame */
    assert_memory_equal(parameter->trameExpected, currentResult, sizeResult);
}
