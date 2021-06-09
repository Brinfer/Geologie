/**
 * @file test_translatorLOG_sendAllBeaconsData.c
 *
 * @brief Ensemble de test pour tester TranslatorLOG_translateForSendAllBeaconsData.
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
 * @brief Structure passee au fonction test.
 */
typedef struct {
    Trame* trameExpected;       /**< La trame attendue en resultat. */
    uint16_t sizeTrameExpected; /**< La taille de la trame attendue en resultat. */
    Date dateInput;             /**< La date donne en entree. */
    uint8_t nbBeaconInput;      /**< Le nombre de balises donnee en entree. */
    BeaconData* beaconDataInput; /**< Les donnees balises donnees en entree. */
} ParameterTestAllBeaconData;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Les donnees balises du test A.
 */
static BeaconData beaconsDataTest_A[3] = {
    {
        .ID = "A1",
        .position = {.X = 0, .Y = 0},
        .power = 0,
        .coefficientAverage = 2
    },
    {
        .ID = "A2",
        .position = {.X = 2295105740, .Y = 3431517320},
        .power = -100,
        .coefficientAverage = 3.5
    },
    {
        .ID = "A3",
        .position = {.X = 2857740885, .Y = 1437226410},
        .power = -55.55,
        .coefficientAverage = 2.5
    }
};

/**
 * @brief La trame attendue du test A.
 */
static Trame trameExpected_A[65] = {
    // Header
    SEND_ALL_BEACONS_DATA,  // CMD
    0x00, 0x3E,             // Size - 62

    0x03,                   // Nb BeaconsData - 3

    0x00, 0x00, 0x00, 0x00, // Date

    // Beacon A1
    0x41, 0x31, 0x00,       // ID
    0x00, 0x00, 0x00, 0x00, // Position X
    0x00, 0x00, 0x00, 0x00, // Position Y
    0x00, 0x00, 0x00, 0x00, // Power
    0x40, 0x00, 0x00, 0x00, // Attenuation Coefficient

    // Beacon A2
    0x41, 0x32, 0x00,       // ID
    0x88, 0xCC, 0x88, 0xCC, // Position X
    0xCC, 0x88, 0xCC, 0x88, // Position Y
    0xC2, 0xC8, 0x00, 0x00, // Power
    0x40, 0x60, 0x00, 0x00, // Attenuation Coefficient

    // Beacon A3
    0x41, 0x33, 0x00,       // ID
    0xAA, 0x55, 0xAA, 0x55, // Position X
    0x55, 0xAA, 0x55, 0xAA, // Position Y
    0xC2, 0x5E, 0x33, 0x33, // Power
    0x40, 0x20, 0x00, 0x00, // Attenuation Coefficient
};

/**
 * @brief Les donnees balises du test B.
 */
static BeaconData beaconsDataTest_B[1] = {
    {
        .ID = "B0",
        .position = { .X = 2863311530, .Y = 1431655765},
        .power = -25.75,
        .coefficientAverage = 4
    }
};

/**
 * @brief La trame attendue du test B.
 */
static Trame trameExpected_B[27] = {
    // Header
    SEND_ALL_BEACONS_DATA,  // CMD
    0x00, 0x18,             // Size - 24

    0x01,                   // Nb BeaconsData - 1

    0xA5, 0xA5, 0xA5, 0xA5,         // TimeStamp

    // Beacon B0
    0x42, 0x30, 0x00,       // ID
    0xAA, 0xAA, 0xAA, 0xAA, // Position X
    0x55, 0x55, 0x55, 0x55, // Position Y
    0xC1, 0xCE, 0x00, 0x00, // Power
    0x40, 0x80, 0x00, 0x00, // Attenuation Coefficient
};


/**
 * @brief Ensemble des donnees de tests.
 */
static ParameterTestAllBeaconData parameterTest[] = {
    {
        .nbBeaconInput = 3,
        .beaconDataInput = beaconsDataTest_A,
        .dateInput = 0,
        .sizeTrameExpected = 65,
        .trameExpected = trameExpected_A
    },
    {
        .nbBeaconInput = 1,
        .beaconDataInput = beaconsDataTest_B,
        .dateInput = 2779096485,
        .sizeTrameExpected = 27,
        .trameExpected = trameExpected_B
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Execute les tests de TranslatorLog_translateForSendAllBeaconsData.
 *
 * @return int 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int test_TranslatorLOG_run_translateForSendAllBeaconsData(void);

/**
 * @brief La fonction test permettant de verifier le bon fonctionnement de TranslatorLOG_translateForSendAllBeaconsData.
 *
 * @param state Les donnees de test #ParameterTestAllBeaconData.
 */
static void test_TranslatorLOG_translateForSendAllBeaconsData(void** state);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Ensemble des tests a executer.
 */
static const struct CMUnitTest testsAllBeaconsData[] = {
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendAllBeaconsData, &(parameterTest[0])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendAllBeaconsData, &(parameterTest[1])),
};


extern int test_TranslatorLOG_run_translateForSendAllBeaconsData(void) {
    return cmocka_run_group_tests_name("Test of the module translatorLOG for function TranslatorLOG_translateForSendAllBeaconsData", testsAllBeaconsData, NULL, NULL);
}

static void test_TranslatorLOG_translateForSendAllBeaconsData(void** state) {
    ParameterTestAllBeaconData* parameter = (ParameterTestAllBeaconData*) *state;

    /* Test trame sizeResult */
    uint16_t sizeResult = TranslatorLOG_getTrameSize(SEND_ALL_BEACONS_DATA, parameter->nbBeaconInput);
    assert_int_equal(parameter->sizeTrameExpected, sizeResult);

    Trame currentResult[sizeResult];
    TranslatorLOG_translateForSendAllBeaconsData(parameter->nbBeaconInput, parameter->beaconDataInput, parameter->dateInput, currentResult);

    /* Test trame */
    assert_memory_equal(parameter->trameExpected, currentResult, sizeResult);
}
