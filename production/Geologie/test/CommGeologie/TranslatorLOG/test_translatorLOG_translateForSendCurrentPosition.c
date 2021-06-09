/**
 * @file test_translatorLOG_translateTrameToHeader.c
 *
 * @brief Ensemble de test pour tester TranslatorLog_translateForSendCurrentPosition
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

#include <float.h>
#include <limits.h>
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

#define SIZE_POSITION (8)

#define SIZE_TIMESTAMP (4)

typedef struct {
    Trame trameExpected[SIZE_HEADER + SIZE_POSITION + SIZE_TIMESTAMP];
    Position positionInput;
    Date dateInput;
} ParameterTestCurrentPosition;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static ParameterTestCurrentPosition parameterTest[] = {
    {
        .positionInput = { .X = 0, .Y = 0},
        .dateInput = 0x00000000,
        .trameExpected = {
            // Header
            SEND_CURRENT_POSITION,          // CMD
            0x00, 0x0C,                     // Size - 12

            // Data
            0x00, 0x00, 0x00, 0x00,         // TimeStamp
            0x00, 0x00, 0x00, 0x00,         // Position X
            0x00, 0x00, 0x00, 0x00,         // Position Y
        }
    },
    {
        .positionInput = { .X = 0xFFFFFFFF, .Y = 0xFFFFFFFF},
        .dateInput = 0xFFFFFFFF,
        .trameExpected = {
            // Header
            SEND_CURRENT_POSITION,          // CMD
            0x00, 0x0C,                     // Size - 12

            // Data
            0xFF, 0xFF, 0xFF, 0xFF,         // TimeStamp
            0xFF, 0xFF, 0xFF, 0xFF,         // Position X
            0xFF, 0xFF, 0xFF, 0xFF,         // Position Y
        }
    },
    {
        .positionInput = { .X = 0xAAAAAAAA, .Y = 0x55555555},
        .dateInput = 0xA5A5A5A5,
        .trameExpected = {
            // Header
            SEND_CURRENT_POSITION,          // CMD
            0x00, 0x0C,                     // Size - 12

            // Data
            0xA5, 0xA5, 0xA5, 0xA5,         // TimeStamp
            0xAA, 0xAA, 0xAA, 0xAA,         // Position X
            0x55, 0x55, 0x55, 0x55,         // Position Y
        }
    },
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int test_TranslatorLOG_run_translateForSendCurrentPosition(void);

static void test_TranslatorLOG_translateForSendCurrentPosition(void** state);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const struct CMUnitTest testsCurrentPosition[] = {
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendCurrentPosition, &(parameterTest[0])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendCurrentPosition, &(parameterTest[1])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendCurrentPosition, &(parameterTest[2])),
};


extern int test_TranslatorLOG_run_translateForSendCurrentPosition(void) {
    return cmocka_run_group_tests_name("Test of the module translatorLOG for function TranslatorLog_translateForSendCurrentPosition", testsCurrentPosition, NULL, NULL);
}

static void test_TranslatorLOG_translateForSendCurrentPosition(void** state) {
    ParameterTestCurrentPosition* parameter = (ParameterTestCurrentPosition*) *state;

    /* Test trame sizeResult */
    uint16_t sizeResult = TranslatorLog_getTrameSize(SEND_CURRENT_POSITION, 0);
    assert_int_equal(SIZE_HEADER + SIZE_POSITION + SIZE_TIMESTAMP, sizeResult);

    Trame currentResult[sizeResult];
    TranslatorLog_translateForSendCurrentPosition(&(parameter->positionInput), parameter->dateInput, &currentResult);

    /* Test trame */
    assert_memory_equal(&(parameter->trameExpected), &currentResult, sizeResult);
}
