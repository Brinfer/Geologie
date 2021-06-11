/**
 * @file translatorBeacon_test.c
 *
 * @brief Ensemble de test pour TranslatorBeacon
 *
 * @version 2.0
 * @date 08-06-2021
 * @author GAUTIER Pierre-Louis
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

#include "TranslatorBeacon/translatorBeacon.c"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief La taille de la trame.
 */
#define SIZE_TRAME (23)

/**
 * @brief Structure passee au fonction test.
 */
typedef struct {
    uint8_t inputData[SIZE_TRAME];
    BeaconSignal resultExpected;
} TestData;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Les donnees balises des tests.
 */
TestData parametersTestData[] = {
    {
        .inputData = {
            0, 0, 0, 0, 0,              // Not used
            'A', 'A',                   // BeaconId
            '0', '0', '0', '0', '1',    // Beacon Position X
            0,                          // Not used
            '0', '0', '0', '0', '1',    // Beacon Position Y
            0x18, 0x1A,                 // UUID, default value of the UUID
            0x01,                       // RSSI
        },
        .resultExpected = {.name = {'A', 'A', '\0'}, .uuid = { 0x18, 0x1A }, .rssi = 1, .position = { 1, 1 }}
    },
    {
        .inputData = {
            0, 0, 0, 0, 0,              // Not used
            '2', 'B',                   // BeaconId
            '5', '4', '3', '2', '1',    // Beacon Position X
            0,                          // Not used
            '1', '2', '3', '4', '5',    // Beacon Position Y
            0x02, 0x00,                 // UUID
            0xFE,                       // RSSI
        },
        .resultExpected = {.name = {'2', 'B', '\0'}, .uuid = { 0x02, 0x00 }, .rssi = -2, .position = { 54321, 12345 }}
    },
    {
        .inputData = {
            0, 0, 0, 0, 0,              // Not used
            'C', '3',                   // BeaconId
            '3', '3', '0', '0', '0',    // Beacon Position X
            0,                          // Not used
            '0', '0', '0', '3', '3',    // Beacon Position Y
            0x00, 0x03,                 // UUID
            0x00,                       // RSSI
        },
        .resultExpected = {.name = {'C', '3', '\0'}, .uuid = { 0x00, 0x03 }, .rssi = 0, .position = { 33000, 33 }}
    },
    {
        .inputData = {
            0, 0, 0, 0, 0,              // Not used
            '4', '4',                   // BeaconId
            '0', '0', '4', '0', '0',    // Beacon Position X
            0,                          // Not used
            '9', '6', '0', '7', '9',    // Beacon Position Y
            0xFF, 0xFF,                 // UUID
            0x9C,                       // RSSI
        },
        .resultExpected = {.name = {'4', '4', '\0'}, .uuid = { 0xFF, 0xFF }, .rssi = 100, .position = { 400, 96079 }}
    },
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Lance la suite de test du module translator.
 *
 * @return int32_t 0 en cas de succee ou le numero du test qui a echoue.
 */
int32_t translatorBeacon_run_tests();

/**
 * @brief Fonction execute avant chaque debut de test.
 *
 * @param state Parametre passe pour mettre en place les tests, ici ignore.
 * @return int32_t 0 en cas de succes, -1 sinon.
 */
int32_t setUp(void** state);

/**
 * @brief Fonction execute apres chaque test.
 *
 * @param state Parametre passe pour mettre en place les tests, ici ignore.
 * @return int32_t 0 en cas de succes, -1 sinon.
 */
int32_t tearDown(void** state);

/**
 * @brief Fonction test de #TranslatorBeacon_translateChannelToBeaconsSignal.
 *
 * @param state Les donnees #TestData passe au test.
 */
void test_translationToByte(void** state);


/**
 * @brief Suite de test de la conversion des tableau d'octet e, structure.
 */
static const struct CMUnitTest tests[] = {
    cmocka_unit_test_prestate(test_translationToByte, &(parametersTestData[0])),
    cmocka_unit_test_prestate(test_translationToByte, &(parametersTestData[1])),
    cmocka_unit_test_prestate(test_translationToByte, &(parametersTestData[2])),
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t setUp(void** state) {
    return TranslatorBeacon_new();
}

int32_t tearDown(void** state) {
    return TranslatorBeacon_free();
}

int32_t translatorBeacon_run_tests() {
    return cmocka_run_group_tests_name("Test of the module translator", tests, setUp, tearDown);
}

void test_translationToByte(void** state) {
    TestData* testData = (TestData*) *state;

    BeaconSignal* expectedResult = &(testData->resultExpected);
    BeaconSignal currentResult;

    BeaconsChannel inputData;
    inputData.length = SIZE_TRAME;
    memcpy(inputData.data, testData->inputData, SIZE_TRAME);

    currentResult = TranslatorBeacon_translateChannelToBeaconsSignal(&inputData);

    /* Beacon ID */
    assert_string_equal(expectedResult->name, currentResult.name);

    /* Beacon Position */
    assert_int_equal(expectedResult->position.X, currentResult.position.X);
    assert_int_equal(expectedResult->position.Y, currentResult.position.Y);

    /* UUID */
    assert_memory_equal(expectedResult->uuid, currentResult.uuid, DEVICE_UUID_LENGTH);

    /* RSSI */
    assert_int_equal(expectedResult->rssi, currentResult.rssi);
}
