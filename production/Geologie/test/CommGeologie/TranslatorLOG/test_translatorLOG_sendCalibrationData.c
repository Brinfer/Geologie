/**
 * @file test_translatorLOG_sendExperimentalTrajects.c
 *
 * @brief Ensemble de test pour tester TranslatorLOG_translateForSendCalibrationData.
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
 * @brief Structure passee aux fonctions tests.
 */
typedef struct {
    CalibrationData* calibrationDataInput;  /**< Les donnees de #CalibrationData passees en entree. */
    uint8_t nbCalibrationDataInput;         /**< Le nombre de #CalibrationData passes en entree. */
    Trame* trameExpected;                   /**< La #Trame attendue en sortie de test. */
    uint16_t sizeTrameExpected;             /**< La taille de la #Trame attendue en sortie de test. */
} ParameterTestCalibrationData;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Les donnees de coefficient de la balise 1 du test A.
 */
static BeaconCoefficients beaconsCoefficient_A1[3] = {
    {
        .beaconId = "A1",
        .positionId = 0,
        .attenuationCoefficient = 2,
    },
    {
        .beaconId = "A1",
        .positionId = 255,
        .attenuationCoefficient = 2.5,
    },
    {
        .beaconId = "A1",
        .positionId = 126,
        .attenuationCoefficient = 4,
    }
};

/**
 * @brief Les donnees de coefficient de la balise 2 du test A.
 */
static BeaconCoefficients beaconsCoefficient_A2[1] = {
    {
        .beaconId = "A2",
        .positionId = 75,
        .attenuationCoefficient = 3.5,
    }
};

/**
 * @brief Les donnees de calibration du test A.
 */
static CalibrationData calibrationsData_A[2] = {
    {
        .beaconId = "A1",
        .beaconCoefficient = beaconsCoefficient_A1,
        .nbCoefficient = 3,
        .coefficientAverage = 2.833,
    },
    {
        .beaconId = "A2",
        .beaconCoefficient = beaconsCoefficient_A2,
        .nbCoefficient = 1,
        .coefficientAverage = 3.5,
    }
};

/**
 * @brief La trame attendue du test A.
 */
static Trame trameExpected_A[40] = {
    SEND_CALIBRATION_DATA,  //CMD
    0x00, 0x25,             // Size - 37

    0x02,                   // NB Balise - 2

    // Balise A1
    0x41, 0x31, 0x00,       // Balise ID
    0x40, 0x35, 0x4F, 0xDF, // Coefficient attenuation average
    0x03,                   // NB Calibration position - 3
    0x00,                   // Position ID 1
    0x40, 0x00, 0x00, 0x00, // Coefficient attenuation Position 1
    0xFF,                   // Position ID 2
    0x40, 0x20, 0x00, 0x00, // Coefficient attenuation Position 2
    0x7E,                   // Position ID 3
    0x40, 0x80, 0x00, 0x00, // Coefficient attenuation Position 3

    // Balise A2
    0x41, 0x32, 0x00,       // Balise ID
    0x40, 0x60, 0x00, 0x00, // Coefficient attenuation average
    0x01,                   // NB Calibration position - 3
    0x4B,                   // Position ID 1
    0x40, 0x60, 0x00, 0x00  // Coefficient attenuation Position 1
};

/**
 * @brief Les donnees de coefficient de la balise 1 du test B.
 */
static BeaconCoefficients beaconsCoefficient_B1[1] = {
    {
        .beaconId = "B1",
        .positionId = 85,
        .attenuationCoefficient = 3.725,
    },
};

/**
 * @brief Les donnees de calibration du test A.
 */
static CalibrationData calibrationsData_B[1] = {
    {
        .beaconId = "B1",
        .beaconCoefficient = beaconsCoefficient_B1,
        .nbCoefficient = 1,
        .coefficientAverage = 3.725,
    },
};

/**
 * @brief La trame attendue du test B.
 */
static Trame trameExpected_B[17] = {
    SEND_CALIBRATION_DATA,  //CMD
    0x00, 0x0E,             // Size - 14

    0x01,                   // NB Balise - 2

    // Balise A1
    0x42, 0x31, 0x00,       // Balise ID
    0x40, 0x6E, 0x66, 0x66, // Coefficient attenuation average
    0x01,                   // NB Calibration position - 3
    0x55,                   // Position ID 1
    0x40, 0x6E, 0x66, 0x66  // Coefficient attenuation Position 1
};

/**
 * @brief Ensemble des donnees de tests.
 */
static ParameterTestCalibrationData parameterTest[] = {
    {
        .nbCalibrationDataInput = 2,
        .calibrationDataInput = calibrationsData_A,
        .sizeTrameExpected = 40,
        .trameExpected = trameExpected_A
    },
    {
        .nbCalibrationDataInput = 1,
        .calibrationDataInput = calibrationsData_B,
        .sizeTrameExpected = 17,
        .trameExpected = trameExpected_B
    },
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Execute les tests de TranslatorLOG_translateForSendCalibrationData.
 *
 * @return int 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int test_TranslatorLOG_run_translateForSendCalibrationData(void);

/**
 * @brief La fonction test permettant de verifier le bon fonctionnement de TranslatorLOG_translateForSendCalibrationData.
 *
 * @param state Les donnees de test #ParameterTestCalibrationData.
 */
static void test_TranslatorLOG_translateForSendCalibrationData(void** state);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Ensemble des tests a executer.
 */
static const struct CMUnitTest testsExperimentalTraject[] = {
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendCalibrationData, &(parameterTest[0])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendCalibrationData, &(parameterTest[1])),
};


extern int test_TranslatorLOG_run_translateForSendCalibrationData(void) {
    return cmocka_run_group_tests_name("Test of the module translatorLOG for function TranslatorLOG_translateForSendCalibrationData", testsExperimentalTraject, NULL, NULL);
}

static void test_TranslatorLOG_translateForSendCalibrationData(void** state) {
    ParameterTestCalibrationData* parameter = (ParameterTestCalibrationData*) *state;

    /* Size */
    uint16_t sizeResult = TranslatorLOG_getTrameSizeCalibrationData(parameter->calibrationDataInput, parameter->nbCalibrationDataInput);
    assert_int_equal(parameter->sizeTrameExpected, sizeResult);

    /* */
    Trame currentResult[sizeResult];
    TranslatorLOG_translateForSendCalibrationData(parameter->calibrationDataInput, parameter->nbCalibrationDataInput, currentResult);

    /* Test trame */
    assert_memory_equal(parameter->trameExpected, currentResult, sizeResult);
}
