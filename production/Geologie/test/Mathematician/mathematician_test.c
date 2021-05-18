/**
 * @file mathematician_test.c
 *
 * @brief Ensemble de fonction permettant de tester le module Mathematician.
 *
 * @version 1.0
 * @date 11/05/2021
 * @author BRIENT Nathan
 * @copyright ESD 2-clauses
 *
 * @see ReceptionistLOG/Mathematician.h
 * @see ReceptionistLOG/Mathematician.c
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

#include "Mathematician/Mathematician.c"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define EPSILON (0.0001)
/**
 * @struct ParametersTestCalculDistancePosition
 *
 * @brief Structure des donnees passees en parametre des fonction de test pour la conversion des Position
 * en tableau d'octet.
 */
typedef struct {
    Position positionTested[2];            /**< La Position a convertir */
    float expectedResult;    /**< Le resultat de la conversion attendue */
} ParametersTestCalculDistancePosition;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Tableau contenant les donnees de test pour la conversion des Position.
 *
 */
ParametersTestCalculDistancePosition parametersTestCalculDistancePosition[] = {
    //                                                                     | <---------X---------> | <---------Y---------> |
    {.positionTested = {{.X = 0, .Y = 0 },{.X = 1, .Y = 1 }},    .expectedResult = 1.414213562},
    {.positionTested = {{.X = 0, .Y = 0 },{.X = 0, .Y = 1 }},    .expectedResult = 1},
    {.positionTested = {{.X = 0, .Y = 0 },{.X = 1, .Y = 0 }},    .expectedResult = 1},
    {.positionTested = {{.X = 8, .Y = 4 },{.X = 5, .Y = 4 }},    .expectedResult = 3},
    {.positionTested = {{.X = 30, .Y = 45 },{.X = 8, .Y = 18 }},    .expectedResult = 34.8281},
    {.positionTested = {{.X = 12, .Y = 24 },{.X = -38, .Y = -42 }},    .expectedResult = 82.80096},
    {.positionTested = {{.X = 72, .Y = 37 },{.X = -8, .Y = 0 }},    .expectedResult = 88.141931},
    {.positionTested = {{.X = 238, .Y = 427 },{.X = 837, .Y = -306 }},    .expectedResult = 946.620304},
    {.positionTested = {{.X = 840, .Y = -838 },{.X = 0, .Y = 0 }},    .expectedResult = 1186.526022},
    {.positionTested = {{.X = -47, .Y = 82 },{.X = -6, .Y = 838 }},    .expectedResult = 757.1109562},
    {.positionTested = {{.X = 1048, .Y = 82 },{.X = -85, .Y = 48}},    .expectedResult = 1133.510035},
    {.positionTested = {{.X = 72, .Y = 87 },{.X = 1001, .Y = -1001 }},    .expectedResult = 1430.658939},
    {.positionTested = {{.X = 0, .Y = 0 },{.X = 0, .Y = 0 }},    .expectedResult = 0}
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
static void test_distanceCalculWithPosition(void** state);


/**
 * @brief Suite de test de la conversion des structures de donnees en tableau d'octet.
 *
 */
static const struct CMUnitTest tests[] =
{
    // Calucl Distance with Position
    cmocka_unit_test_prestate(test_distanceCalculWithPosition, &(parametersTestCalculDistancePosition[0])),
    cmocka_unit_test_prestate(test_distanceCalculWithPosition, &(parametersTestCalculDistancePosition[1])),
    cmocka_unit_test_prestate(test_distanceCalculWithPosition, &(parametersTestCalculDistancePosition[2])),
    cmocka_unit_test_prestate(test_distanceCalculWithPosition, &(parametersTestCalculDistancePosition[3])),
    cmocka_unit_test_prestate(test_distanceCalculWithPosition, &(parametersTestCalculDistancePosition[4])),
    cmocka_unit_test_prestate(test_distanceCalculWithPosition, &(parametersTestCalculDistancePosition[5])),
    cmocka_unit_test_prestate(test_distanceCalculWithPosition, &(parametersTestCalculDistancePosition[6])),
    cmocka_unit_test_prestate(test_distanceCalculWithPosition, &(parametersTestCalculDistancePosition[7])),
    cmocka_unit_test_prestate(test_distanceCalculWithPosition, &(parametersTestCalculDistancePosition[8])),
    cmocka_unit_test_prestate(test_distanceCalculWithPosition, &(parametersTestCalculDistancePosition[9])),
    cmocka_unit_test_prestate(test_distanceCalculWithPosition, &(parametersTestCalculDistancePosition[10])),
    cmocka_unit_test_prestate(test_distanceCalculWithPosition, &(parametersTestCalculDistancePosition[11])),
    cmocka_unit_test_prestate(test_distanceCalculWithPosition, &(parametersTestCalculDistancePosition[12])),


    // Calucl Distance with Power

};

/**
 * @brief Lance la suite de test du module Mathematician.
 *
 * @return 0 en cas de succees ou le numero du test qui a echoue.
 */
int mathematician_run_tests() {
    return cmocka_run_group_tests_name("Test of the module Mathematician", tests, NULL, NULL);
}

static void test_distanceCalculWithPosition(void** state) {
    ParametersTestCalculDistancePosition* param = (ParametersTestCalculDistancePosition*) *state;

    float result;
    result = distanceCalculWithPosition(&param->positionTested[0], &param->positionTested[1]);
    assert_float_equal(result, param->expectedResult, EPSILON);
    
}


