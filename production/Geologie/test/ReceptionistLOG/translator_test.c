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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void test_TRANSLATOR_convertPosition(void** state);


static const struct CMUnitTest tests[] =
{
    cmocka_unit_test(test_TRANSLATOR_convertPosition)
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
    const Position positionTest_A = { .X = 0, .Y = 0 };
    const Position positionTest_B = { .X = -1, .Y = 1 };
    const Position positionTest_C = { .X = 1, .Y = -1 };
    const Position positionTest_D = { .X = 0.12, .Y = -123.45 };
    const Position positionTest_E = { .X = -123.45, .Y = 0.12 };
    const Position positionTest_F = { .X = -0.12, .Y = 123.45 };
    const Position positionTest_G = { .X = 123.45, .Y = -0.12 };

    //                                        | <---------X---------> | <---------Y---------> |
    const unsigned char expectedResult_A[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    const unsigned char expectedResult_B[8] = { 0xEF, 0x80, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00 };
    const unsigned char expectedResult_C[8] = { 0x3F, 0x00, 0x00, 0x00, 0xEF, 0x80, 0x00, 0x00 };
    const unsigned char expectedResult_D[8] = { 0x3D, 0xF5, 0xC2, 0x8F, 0xC2, 0xF6, 0xE6, 0x66 };
    const unsigned char expectedResult_E[8] = { 0xC2, 0xF6, 0xE6, 0x66, 0x3D, 0xF5, 0xC2, 0x8F };
    const unsigned char expectedResult_F[8] = { 0xED, 0xF5, 0xC2, 0x8F, 0x42, 0xF6, 0xE6, 0x66 };
    const unsigned char expectedResult_G[8] = { 0x42, 0xF6, 0xE6, 0x66, 0xED, 0xF5, 0xC2, 0x8F };

    /* Test de A */
    unsigned char result_A[8] = { 0 };
    TRANSLATOR_convertPosition(&positionTest_A, result_A);

    assert_memory_equal(result_A, expectedResult_A, 8);

    /* Test de B */
    unsigned char result_B[8] = { 0 };
    TRANSLATOR_convertPosition(&positionTest_B, result_B);

    assert_memory_equal(result_B, expectedResult_B, 8);

    /* Test de C */
    unsigned char result_C[8] = { 0 };
    TRANSLATOR_convertPosition(&positionTest_C, result_C);

    assert_memory_equal(result_C, expectedResult_C, 8);

    /* Test de D */
    unsigned char result_D[8] = { 0 };
    TRANSLATOR_convertPosition(&positionTest_D, result_D);

    assert_memory_equal(result_D, expectedResult_D, 8);

    /* Test de E */
    unsigned char result_E[8] = { 0 };
    TRANSLATOR_convertPosition(&positionTest_E, result_E);

    assert_memory_equal(result_E, expectedResult_E, 8);

    /* Test de F */
    unsigned char result_F[8] = { 0 };
    TRANSLATOR_convertPosition(&positionTest_F, result_F);

    assert_memory_equal(result_F, expectedResult_F, 8);

    /* Test de G */
    unsigned char result_G[8] = { 0 };
    TRANSLATOR_convertPosition(&positionTest_G, result_G);

    assert_memory_equal(result_G, expectedResult_G, 8);
}
