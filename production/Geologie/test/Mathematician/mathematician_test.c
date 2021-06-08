/**
 * @file mathematician_test.c
 *
 * @brief Ensemble de fonction permettant de tester le module mathematician.
 *
 * @version 1.0
 * @date 11/05/2021
 * @author BRIENT Nathan
 * @copyright ESD 2-clauses
 *
 * @see ReceptionistLOG/mathematician.h
 * @see ReceptionistLOG/mathematician.c
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
#include "Mathematician/mathematician.c"

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

/**
 * @struct ParametersTestCalculDistancePower
 *
 * @brief Structure des donnees passees en parametre des fonction de test pour la conversion des Puissances
 * en tableau d'octet.
 */
typedef struct {
    AttenuationCoefficient attenuationCoefficient;  /**< Le coefficient d'attenuation */
    Power power;            /**< La Puissance a convertir */
    float expectedDistance;    /**< Le resultat de la conversion attendue */
} ParametersTestCalculDistancePower;

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


/**
 * @brief Tableau contenant les donnees de test pour la conversion des Puissances.
 *
 */
ParametersTestCalculDistancePower parametersTestCalculDistancePower[] = {
    {.attenuationCoefficient = 1,.power = -40,   .expectedDistance = 1},
    {.attenuationCoefficient = 1,.power = -50,   .expectedDistance = 1.0000},
    {.attenuationCoefficient = 2,.power = -50,   .expectedDistance = 1.0000},
    {.attenuationCoefficient = 2,.power = -80,   .expectedDistance = 57.665039},
    {.attenuationCoefficient = 2,.power = -100,   .expectedDistance = 9536.743164},
    {.attenuationCoefficient = 3,.power = -50,   .expectedDistance = 0},
    {.attenuationCoefficient = 3,.power = -70,   .expectedDistance = 0.017341},
    {.attenuationCoefficient = 3,.power = -90,   .expectedDistance = 17.757726},
    {.attenuationCoefficient = 4,.power = -40,   .expectedDistance = 0},
    {.attenuationCoefficient = 4,.power = -60,   .expectedDistance = 0},
    {.attenuationCoefficient = 4,.power = -90,   .expectedDistance = 1},
    {.attenuationCoefficient = 5,.power = -70,   .expectedDistance = 0.000104},
    {.attenuationCoefficient = 5,.power = -100,   .expectedDistance = 1},
    {.attenuationCoefficient = 6,.power = -60,   .expectedDistance = 0.000016},
    {.attenuationCoefficient = 6,.power = -80,   .expectedDistance = 0.00604},
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Teste le calcul d'une distance avec deux position
 *
 * @param state
 */
static void test_distanceCalculWithPosition(void** state);

/**
 * @brief Teste le calcul d'une distance avec une puissance
 *
 * @param state
 */
static void test_distanceCalculWithPower(void** state);


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


    // Calcul Distance with Power
    cmocka_unit_test_prestate(test_distanceCalculWithPower, &(parametersTestCalculDistancePower[0])),
    cmocka_unit_test_prestate(test_distanceCalculWithPower, &(parametersTestCalculDistancePower[1])),
    cmocka_unit_test_prestate(test_distanceCalculWithPower, &(parametersTestCalculDistancePower[2])),
    cmocka_unit_test_prestate(test_distanceCalculWithPower, &(parametersTestCalculDistancePower[3])),
    cmocka_unit_test_prestate(test_distanceCalculWithPower, &(parametersTestCalculDistancePower[4])),
    cmocka_unit_test_prestate(test_distanceCalculWithPower, &(parametersTestCalculDistancePower[5])),
    cmocka_unit_test_prestate(test_distanceCalculWithPower, &(parametersTestCalculDistancePower[6])),
    cmocka_unit_test_prestate(test_distanceCalculWithPower, &(parametersTestCalculDistancePower[7])),
    cmocka_unit_test_prestate(test_distanceCalculWithPower, &(parametersTestCalculDistancePower[8])),
    cmocka_unit_test_prestate(test_distanceCalculWithPower, &(parametersTestCalculDistancePower[9])),
    cmocka_unit_test_prestate(test_distanceCalculWithPower, &(parametersTestCalculDistancePower[10])),
    cmocka_unit_test_prestate(test_distanceCalculWithPower, &(parametersTestCalculDistancePower[11])),
    cmocka_unit_test_prestate(test_distanceCalculWithPower, &(parametersTestCalculDistancePower[12])),
    cmocka_unit_test_prestate(test_distanceCalculWithPower, &(parametersTestCalculDistancePower[13])),
    cmocka_unit_test_prestate(test_distanceCalculWithPower, &(parametersTestCalculDistancePower[14])),
};

/**
 * @brief Lance la suite de test du module Mathematician.
 *
 * @return 0 en cas de succees ou le numero du test qui a echoue.
 */
int mathematician_run_tests() {
    return cmocka_run_group_tests_name("Test of the module mathematician", tests, NULL, NULL);
}

static void test_distanceCalculWithPosition(void** state) {
    ParametersTestCalculDistancePosition* param = (ParametersTestCalculDistancePosition*) *state;

    float result;
    result = distanceCalculWithPosition(&param->positionTested[0], &param->positionTested[1]);
    assert_float_equal(result, param->expectedResult, EPSILON);

}

static void test_distanceCalculWithPower(void** state) {
    ParametersTestCalculDistancePower* param = (ParametersTestCalculDistancePower*) *state;
    float result;
    result = distanceCalculWithPower(&param->power, &param->attenuationCoefficient);
    assert_float_equal(result, param->expectedDistance, EPSILON);
}
