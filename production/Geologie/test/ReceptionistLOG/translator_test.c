/**
 * @file translator_test.c
 *
 * @brief Ensemble de fonction permettant de tester le module translator.
 *
 * @version 1.0
 * @date 6 may 2021
 * @author GAUTIER Pierre-Louis
 * @copyright ESD 2-clauses
 *
 * @see ReceptionistLOG/translator.h
 * @see ReceptionistLOG/translator.c
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <limits.h>

#include "cmocka.h"

#include "Mathematician/Mathematician.h"
#include "ReceptionistLOG/translator.c"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @struct ParametersTestPosition
 *
 * @brief Structure des donnees passees en parametre des fonction de test pour la conversion des Position
 * en tableau d'octet.
 */
typedef struct {
    Position position;            /**< La Position a convertir */
    unsigned char arrayBite[8];    /**< Le resultat de la conversion attendue */
} ParametersTestPosition;

/**
 * @struct ParametersTestBeacon
 *
 * @brief Structure des donnees passees en parametre des fonction de test pour la conversion des BeaconData
 * en tableau d'octet et inversement.
 */
typedef struct {
    BeaconData beacon;            /**< Le BeaconData */
    unsigned char arrayBite[32];   /**< Le tableau d'octets */
} ParametersTestBeacon;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Tableau contenant les donnees de test pour la conversion des Position.
 *
 */
ParametersTestPosition parametersTestPositionToBytes[] = {
    //                                                          | <---------X---------> | <---------Y---------> |
    {.position = {.X = 0, .Y = 0 },               .arrayBite = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }},
    {.position = {.X = -1, .Y = 1 },              .arrayBite = { 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01 }},
    {.position = {.X = 1, .Y = -1 },              .arrayBite = { 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF }},
    {.position = {.X = -12345, .Y = 12345 },      .arrayBite = { 0xFF, 0xFF, 0xCF, 0xC7, 0x00, 0x00, 0x30, 0x39 }},
    {.position = {.X = 12345, .Y = -12345 },      .arrayBite = { 0x00, 0x00, 0x30, 0x39, 0xFF, 0xFF, 0xCF, 0xC7 }},
    {.position = {.X = INT_MAX, .Y = INT_MIN },   .arrayBite = { 0x7F, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00 }},
    {.position = {.X = INT_MIN, .Y = INT_MAX },   .arrayBite = { 0x80, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF }},
};

/**
 * @brief Tableau contenant les donnees de test pour la conversion des BeaconData.
 *
 */
ParametersTestBeacon parametersTestBeaconToByte[] = {
    // Beacon A
    {
        .beacon = {
            .ID = "Aa",
            .position = {.X = 12345, .Y = -12345 },
            .power = 12345,
            .attenuationCoefficient = 1.05,
            .nbCoefficientAttenuations = 3,                     // Fixed in the prod code
            .attenuationCoefficientsArray = { -1.05, 1, -1 }
        },
        .arrayBite = {
            0x41, 0x61, 0x00,                                   // ID
            0x00, 0x00, 0x30, 0x39,                             // position X
            0xFF, 0xFF, 0xCF, 0xC7,                             // position Y
            0x00, 0x00, 0x30, 0x39,                             // power
            0x3F, 0x86, 0x66, 0x66,                             // attenuationCoefficient
            0x03,                                               // NB attenuationCoefficients calculated
            0xBF, 0x86, 0x66, 0x66,                             // attenuationCoefficients calculated 1
            0x3F, 0x80, 0x00, 0x00,                             // attenuationCoefficients calculated 2
            0xBF, 0x80, 0x00, 0x00                              // attenuationCoefficients calculated 3
        }
    },

    // Beacon B
    {
        .beacon = {
            .ID = "#}",
            .position = {.X = -12345, .Y = 12345 },
            .power = -12345,
            .attenuationCoefficient = 1,
            .nbCoefficientAttenuations = 3,                     // Fixed in the prod code
            .attenuationCoefficientsArray = { 666, -666, 0 }
        },
        .arrayBite = {
            0x23, 0x7D, 0x00,                                   // ID
            0xFF, 0xFF, 0xCF, 0xC7,                             // position X
            0x00, 0x00, 0x30, 0x39,                             // position Y
            0xFF, 0xFF, 0xCF, 0xC7,                             // power
            0x3F, 0x80, 0x00, 0x00,                             // attenuationCoefficient
            0x03,                                               // NB attenuationCoefficients calculated
            0x44, 0x26, 0x80, 0x00,                             // attenuationCoefficients calculated 1
            0xC4, 0x26, 0x80, 0x00,                             // attenuationCoefficients calculated 2
            0x00, 0x00, 0x00, 0x00                              // attenuationCoefficients calculated 3
        }
    },

    // Beacon C
    {
        .beacon = {
            .ID = {127, 10, 0},
            .position = {.X = -0, .Y = 0 },
            .power = 0.05,
            .attenuationCoefficient = -1.05,
            .nbCoefficientAttenuations = 3,                     // Fixed in the prod code
            .attenuationCoefficientsArray = { -1, 0, 1 }
        },
        .arrayBite = {
            0x7F, 0x0A, 0x00,                                   // ID
            0x00, 0x00, 0x00, 0x00,                             // position X
            0x00, 0x00, 0x00, 0x00,                             // position Y
            0x00, 0x00, 0x00, 0x00,                             // power
            0xBF, 0x86, 0x66, 0x66,                             // attenuationCoefficient
            0x03,                                               // NB attenuationCoefficients calculated
            0xBF, 0x80, 0x00, 0x00,                             // attenuationCoefficients calculated 1
            0x00, 0x00, 0x00, 0x00,                             // attenuationCoefficients calculated 2
            0x3F, 0x80, 0x00, 0x00                              // attenuationCoefficients calculated 3
        }
    },
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Teste la conversion d'une Position en un tableau d'octet.
 *
 * @param state
 */
static void test_Translator_convertPositionToByte(void** state);

/**
 * @brief Teste la conversion d'un BeaconData en un tableau d'octet.
 *
 * @param state
 */
static void test_Translator_convertBeaconDataToByte(void** state);

/**
 * @brief Teste la conversion d'un tableau d'octet en une Position.
 *
 * @param state
 */
static void test_Translator_convertByteToPosition(void** state);

/**
 * @brief
 * TODO
 * @param current
 */
static void checkExpectedPosition(Position* current);

/**
 * @brief Suite de test de la conversion des structures de donnees en tableau d'octet.
 *
 */
static const struct CMUnitTest tests[] = {
    // Position
    cmocka_unit_test_prestate(test_Translator_convertPositionToByte, &(parametersTestPositionToBytes[0])),
    cmocka_unit_test_prestate(test_Translator_convertPositionToByte, &(parametersTestPositionToBytes[1])),
    cmocka_unit_test_prestate(test_Translator_convertPositionToByte, &(parametersTestPositionToBytes[2])),
    cmocka_unit_test_prestate(test_Translator_convertPositionToByte, &(parametersTestPositionToBytes[3])),
    cmocka_unit_test_prestate(test_Translator_convertPositionToByte, &(parametersTestPositionToBytes[4])),
    cmocka_unit_test_prestate(test_Translator_convertPositionToByte, &(parametersTestPositionToBytes[5])),
    cmocka_unit_test_prestate(test_Translator_convertPositionToByte, &(parametersTestPositionToBytes[6])),

    cmocka_unit_test_prestate(test_Translator_convertByteToPosition, &(parametersTestPositionToBytes[0])),
    cmocka_unit_test_prestate(test_Translator_convertByteToPosition, &(parametersTestPositionToBytes[1])),
    cmocka_unit_test_prestate(test_Translator_convertByteToPosition, &(parametersTestPositionToBytes[2])),
    cmocka_unit_test_prestate(test_Translator_convertByteToPosition, &(parametersTestPositionToBytes[3])),
    cmocka_unit_test_prestate(test_Translator_convertByteToPosition, &(parametersTestPositionToBytes[4])),
    cmocka_unit_test_prestate(test_Translator_convertByteToPosition, &(parametersTestPositionToBytes[5])),
    cmocka_unit_test_prestate(test_Translator_convertByteToPosition, &(parametersTestPositionToBytes[6])),


    // BeaconData
    cmocka_unit_test_prestate(test_Translator_convertBeaconDataToByte, &(parametersTestBeaconToByte[0])),
    cmocka_unit_test_prestate(test_Translator_convertBeaconDataToByte, &(parametersTestBeaconToByte[1])),
    cmocka_unit_test_prestate(test_Translator_convertBeaconDataToByte, &(parametersTestBeaconToByte[2]))
};

/**
 * @brief Lance la suite de test du module translator.
 *
 * @return 0 en cas de succee ou le numero du test qui a echoue.
 */
int translator_run_tests() {
    return cmocka_run_group_tests_name("Test of the module translator", tests, NULL, NULL);
}

static void test_Translator_convertPositionToByte(void** state) {
    ParametersTestPosition* param = (ParametersTestPosition*) *state;

    unsigned char result[8];
    Translator_convertPositionToByte(&(param->position), result);

    assert_memory_equal(result, param->arrayBite, 8);
}

static void test_Translator_convertBeaconDataToByte(void** state) {
    ParametersTestBeacon* param = (ParametersTestBeacon*) *state;

    unsigned char result[32];
    Translator_convertBeaconDataToByte(&(param->beacon), result);

    assert_memory_equal(result, param->arrayBite, 32);
}

static void test_Translator_convertByteToPosition(void** state) {
    ParametersTestPosition* param = (ParametersTestPosition*) *state;
    expect_memory(checkExpectedPosition, current, &(param->position), sizeof(param->position));

    Position result;
    Translator_convertByteToPosition(param->arrayBite, &result);
    checkExpectedPosition(&result);
}

static void checkExpectedPosition(Position* current) {
    check_expected(current);
}
