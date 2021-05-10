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
    char expectedResult[8];
} ParametersTestPosition;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ParametersTestPosition parametersTestPosition[] = {
    //                                                                 | <---------X---------> | <---------Y---------> |
    {.positionTested = {.X = 0, .Y = 0 },           .expectedResult = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }},
    {.positionTested = {.X = -1, .Y = 1 },          .expectedResult = { 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01 }},
    {.positionTested = {.X = 1, .Y = -1 },          .expectedResult = { 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF }},
    {.positionTested = {.X = -12345, .Y = 12345 },  .expectedResult = { 0xFF, 0xFF, 0xCF, 0xC7, 0x00, 0x00, 0x30, 0x39 }},
    {.positionTested = {.X = 12345, .Y = -12345 },  .expectedResult = { 0x00, 0x00, 0x30, 0x39, 0xFF, 0xFF, 0xCF, 0xC7 }},
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void test_TRANSLATOR_convertPosition(void** state);


static const struct CMUnitTest tests[] =
{
    cmocka_unit_test_prestate(test_TRANSLATOR_convertPosition, &(parametersTestPosition[0])),
    cmocka_unit_test_prestate(test_TRANSLATOR_convertPosition, &(parametersTestPosition[1])),
    cmocka_unit_test_prestate(test_TRANSLATOR_convertPosition, &(parametersTestPosition[2])),
    cmocka_unit_test_prestate(test_TRANSLATOR_convertPosition, &(parametersTestPosition[3])),
    cmocka_unit_test_prestate(test_TRANSLATOR_convertPosition, &(parametersTestPosition[4])),

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

    const Position* positionTest = &(param->positionTested);
    const char* expectedResult = &(param->expectedResult);

    char result[8];
    TRANSLATOR_convertPosition(positionTest, &result);

    assert_memory_equal(&result, expectedResult, 8);
}


    /* Test de F */
    unsigned char result_F[8];
    TRANSLATOR_convertPosition(&positionTest_F, result_F);

    assert_memory_equal(result_F, expectedResult_F, 8);

    /* Test de G */
    unsigned char result_G[8];
    TRANSLATOR_convertPosition(&positionTest_G, result_G);

    assert_memory_equal(result_G, expectedResult_G, 8);
}
