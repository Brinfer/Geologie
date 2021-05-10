/**
 * @file translator_test.c
 *
 * @version 1.0
 * @date 6 may 2021
 * @author GAUTIER Pierre-Louis
 * @copyright ESD 2-clauses
 *
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

#include "Geographer/geographer.h"
#include "ReceptionistLOG/translator.c"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int set_up(void** state);
static int tear_down(void** state);

typedef struct {
    Position positionTested;
    unsigned char expectedResult[8];
} ParametersTestPosition;

typedef struct {
    BeaconData beaconTested;
    unsigned char expectedResult[32];
} ParametersTestBeacon;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ParametersTestPosition parametersTestPosition[] = {
    //                                                                     | <---------X---------> | <---------Y---------> |
    {.positionTested = {.X = 0, .Y = 0 },               .expectedResult = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }},
    {.positionTested = {.X = 0.05, .Y = 0.05 },         .expectedResult = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }},
    {.positionTested = {.X = -1, .Y = 1 },              .expectedResult = { 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01 }},
    {.positionTested = {.X = 1, .Y = -1 },              .expectedResult = { 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF }},
    {.positionTested = {.X = 1.05, .Y = -1.05 },        .expectedResult = { 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF }},
    {.positionTested = {.X = -12345, .Y = 12345 },      .expectedResult = { 0xFF, 0xFF, 0xCF, 0xC7, 0x00, 0x00, 0x30, 0x39 }},
    {.positionTested = {.X = 12345, .Y = -12345 },      .expectedResult = { 0x00, 0x00, 0x30, 0x39, 0xFF, 0xFF, 0xCF, 0xC7 }},
    {.positionTested = {.X = INT_MAX, .Y = INT_MIN },   .expectedResult = { 0x7F, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00 }}
};

ParametersTestBeacon parametersTestBeacon[] = {
    // Beacon A
    {
        .beaconTested = {
            .ID = "Aa",
            .position = {.X = 12345, .Y = -12345 },
            .power = 12345,
            .attenuationCoefficient = 1.05,
            .nbCoefficientAttenuations = 3,                     // Fixed in the prod code
            .attenuationCoefficients = { -1.05, 1, -1 }
        },
        .expectedResult = {
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
        .beaconTested = {
            .ID = "#}",
            .position = {.X = -12345, .Y = 12345 },
            .power = -12345,
            .attenuationCoefficient = 1,
            .nbCoefficientAttenuations = 3,                     // Fixed in the prod code
            .attenuationCoefficients = { 666, -666, 0 }
        },
        .expectedResult = {
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
        .beaconTested = {
            .ID = {127, 10, 0},
            .position = {.X = -0.05, .Y = 0.05 },
            .power = 0.05,
            .attenuationCoefficient = -1.05,
            .nbCoefficientAttenuations = 3,                     // Fixed in the prod code
            .attenuationCoefficients = { -1, 0, 1 }
        },
        .expectedResult = {
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

static void test_TRANSLATOR_convertPosition(void** state);
static void test_TRANSLATOR_convertBeaconData(void** state);

static const struct CMUnitTest tests[] =
{
    cmocka_unit_test_prestate(test_TRANSLATOR_convertPosition, &(parametersTestPosition[0])),
    cmocka_unit_test_prestate(test_TRANSLATOR_convertPosition, &(parametersTestPosition[1])),
    cmocka_unit_test_prestate(test_TRANSLATOR_convertPosition, &(parametersTestPosition[2])),
    cmocka_unit_test_prestate(test_TRANSLATOR_convertPosition, &(parametersTestPosition[3])),
    cmocka_unit_test_prestate(test_TRANSLATOR_convertPosition, &(parametersTestPosition[4])),
    cmocka_unit_test_prestate(test_TRANSLATOR_convertPosition, &(parametersTestPosition[5])),
    cmocka_unit_test_prestate(test_TRANSLATOR_convertPosition, &(parametersTestPosition[6])),
    cmocka_unit_test_prestate(test_TRANSLATOR_convertPosition, &(parametersTestPosition[7])),

    cmocka_unit_test_prestate(test_TRANSLATOR_convertBeaconData, &(parametersTestBeacon[0])),
    cmocka_unit_test_prestate(test_TRANSLATOR_convertBeaconData, &(parametersTestBeacon[1])),
    cmocka_unit_test_prestate(test_TRANSLATOR_convertBeaconData, &(parametersTestBeacon[2]))
};

int translator_run_tests() {
    return cmocka_run_group_tests_name("Test of the module translator", tests, set_up, tear_down);
}

static int set_up(void** state) {
    return 0;
}

static int tear_down(void** state) {
    return 0;
}

static void test_TRANSLATOR_convertPosition(void** state) {
    ParametersTestPosition* param = (ParametersTestPosition*) *state;

    unsigned char result[8];
    TRANSLATOR_convertPosition(&(param->positionTested), result);

    assert_memory_equal(result, param->expectedResult, 8);
}

static void test_TRANSLATOR_convertBeaconData(void** state) {
    ParametersTestBeacon* param = (ParametersTestBeacon*) *state;

    unsigned char result[32];
    TRANSLATOR_convertBeaconData(&(param->beaconTested), result);

    assert_memory_equal(result, param->expectedResult, 32);
}
