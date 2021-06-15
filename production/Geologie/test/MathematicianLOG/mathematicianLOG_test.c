/**
 * @file mathematician_test.c
 *
 * @brief Ensemble de fonction permettant de tester le module mathematicianLOG.
 *
 * @version 1.0
 * @date 11/05/2021
 * @author BRIENT Nathan
 * @author MOLARD Simon
 * @copyright ESD 2-clauses
 *
 * @see ReceptionistLOG/mathematicianLOG.h
 * @see ReceptionistLOG/mathematicianLOG.c
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <limits.h>
#include "cmocka.h"
#include "MathematicianLOG/mathematicianLOG.c"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define EPSILON (0.0001)
#define EPSILONPOSITION (1)
/**
 * @struct ParametersTestCalculDistancePosition
 *
 * @brief Structure des donnees passees en parametre des fonctions de test pour la conversion des Position
 * en tableau d'octet.
 */
typedef struct {
    Position positionTested[2];            /**< La Position a convertir */
    float expectedResult;    /**< Le resultat de la conversion attendue */
} ParametersTestCalculDistancePosition;

/**
 * @struct ParametersTestCalculDistancePower
 *
 * @brief Structure des donnees passees en parametre des fonctions de test pour la conversion des Puissances
 * en tableau d'octet.
 */
typedef struct {
    AttenuationCoefficient attenuationCoefficient;  /**< Le coefficient d'attenuation */
    Power power;            /**< La Puissance a convertir */
    float expectedDistance;    /**< Le resultat de la conversion attendue */
} ParametersTestCalculDistancePower;

/**
 * @struct ParametersTestGetAverageCalcul
 *
 * @brief Structure des donnees passees en parametre des fonctions de test pour le calcul des moyennes des coefficients d'atténuation.
 */


typedef struct {
    uint8_t nbCoefficient;           /**< Le nombre de coefficient */
    uint8_t nbBeaconInput;      /**< Le nombre de balises donnee en entree. */
    BeaconCoefficients * beaconCoefficientInput; /**< Les donnees balises donnees en entree. */
    float expectedAverage;    /**< Le resultat de la conversion attendue */
} ParametersTestGetAverageCalcul;

/**
 * @struct ParametersTestGetAttenuation
 *
 * @brief Structure des donnees passees en parametre des fonctions de test pour le calcul d'un coefficient d'attenuation.
 */


typedef struct {
    Power power;            /**< La Puissance a convertir */
    Position beaconPosition; /**< la position de la balise . */
    CalibrationPosition calibrationPosition;/**< la position de calibration . */
    float expectedAttenuationCoefficient;    /**< Le resultat de la conversion attendue */
} ParametersTestGetAttenuationCoefficient;

/**
 * @struct ParametersTestGetCurrentPosition
 *
 * @brief Structure des donnees passees en parametre des fonctions de test pour le calcul de la position actuelle.
 */


typedef struct {
    BeaconData * beaconsData;            /**< Les données balises données en entrée */
    uint8_t nbBeacon;               /**< nombre de balise. */
    Position currentPosition; /**< la position actuelle . */
    Position expectedCurrentPosition;    /**< Le resultat de la conversion attendue */
} ParametersTestGetCurrentPosition;

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

/**
 * @brief Tableau contenant les donnees de test pour la conversion des Position.
 *
 */
static ParametersTestCalculDistancePosition parametersTestCalculDistancePosition[] = {
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
static ParametersTestCalculDistancePower parametersTestCalculDistancePower[] = {
    {.attenuationCoefficient = 1,.power = -40,   .expectedDistance = 10.0000},
    {.attenuationCoefficient = 1,.power = -50,   .expectedDistance = 100.00},
    {.attenuationCoefficient = 2,.power = -50,   .expectedDistance = 100.00},
    {.attenuationCoefficient = 2,.power = -80,   .expectedDistance = 3162.277588},
    {.attenuationCoefficient = 2,.power = -100,   .expectedDistance = 31622.7766},
    {.attenuationCoefficient = 3,.power = -50,   .expectedDistance = 100},
    {.attenuationCoefficient = 3,.power = -70,   .expectedDistance = 464.158905},
    {.attenuationCoefficient = 3,.power = -90,   .expectedDistance = 2154.434814},
    {.attenuationCoefficient = 4,.power = -40,   .expectedDistance = 56.2341},
    {.attenuationCoefficient = 4,.power = -60,   .expectedDistance = 177.8279},
    {.attenuationCoefficient = 4,.power = -90,   .expectedDistance = 1000.000},
    {.attenuationCoefficient = 5,.power = -70,   .expectedDistance = 251.1886},
    {.attenuationCoefficient = 5,.power = -100,   .expectedDistance = 1000.0},
    {.attenuationCoefficient = 6,.power = -60,   .expectedDistance = 146.7799},
    {.attenuationCoefficient = 6,.power = -80,   .expectedDistance = 316.2277}
};

/**
 * @brief Teste le calcul d'une distance avec deux positions.
 *
 * @param state
 */
static void test_distanceCalculWithPosition(void** state);

/**
 * @brief Teste le calcul du coefficient d'atténuation pour une balise
 *
 * @param state
 */
static void test_getAttenuationCoefficient(void** state);

/**
 * @brief les données coefficients des données balises du test A
 *
 */
static BeaconCoefficients beaconsDataTest_A[3] = {
    {
        .positionId = 01,
        .attenuationCoefficient = 2
    },
    {
        .positionId = 02,
        .attenuationCoefficient = 3
    },
    {
        .positionId = 03,
        .attenuationCoefficient = 4
    }
};

/**
 * @brief les données coefficients des données balises du test B
 *
**/

static BeaconCoefficients beaconsDataTest_B[5] = {
    {
        .positionId = 4,
        .attenuationCoefficient = 2
    },
    {
        .positionId = 5,
        .attenuationCoefficient = 3
    },
    {
        .positionId = 6,
        .attenuationCoefficient = 4
    },
    {
        .positionId = 7,
        .attenuationCoefficient = 4
    },
    {
        .positionId = 8,
        .attenuationCoefficient = 3
    }
};


/**
 * @brief Teste le calcul d'une distance avec une puissance
 *
 * @param state
 */
static void test_distanceCalculWithPower(void** state);

/**
 * @brief Teste le calcul de la position actuelle
 *
 * @param state
 */

static void test_getCurrentPosition(void** state);

/**
 * @brief Ensemble des donnees de tests pour le calcul des moyennes des coefficient d'attenuation.
 */
static ParametersTestGetAverageCalcul parameterTest[] = {

    {
        .nbBeaconInput = 3,
        .beaconCoefficientInput = beaconsDataTest_A,
        .expectedAverage = 3,
    },
    {
        .nbBeaconInput = 5,
        .beaconCoefficientInput = beaconsDataTest_B,
        .expectedAverage = 3.2,
    }
};

/**
 * @brief Ensemble des donnees de tests pour le calcul des moyennes d'un' coefficient d'attenuation.
*/
static ParametersTestGetAttenuationCoefficient parametersTestGetAttenuationCoefficientA[1] = {
    {
        .power = -60,
        .beaconPosition = { .X = 4700, .Y= 8200},
        .calibrationPosition.position = { .X = 600, .Y= 8380},
        .expectedAttenuationCoefficient = 0.619885
    }
};

/**
 * @brief Ensemble des donnees de tests pour le calcul des moyennes d'un' coefficient d'attenuation.
*/
static ParametersTestGetAttenuationCoefficient parametersTestGetAttenuationCoefficientB[1] = {

    {
        .power = -60,
        .beaconPosition = { .X = 800, .Y= 400},
        .calibrationPosition.position = { .X = 500, .Y= 400},
        .expectedAttenuationCoefficient = 2.095903
    }
};

/**
 * @brief Ensemble des donnees de tests pour le calcul de la position actuelle.
*/
static BeaconData parametersTestGetCurrentPositionA[3] = {

    {
        .position = { .X = 400, .Y= 700},
        .power = -69.51544993,
        .coefficientAverage = 3,
    },
    {
        .position = { .X = 900, .Y= 100},
        .power = -68.45673382,
        .coefficientAverage = 3,
    },
    {
        .position = { .X = 1300, .Y= 800},
        .power = -72.97218376,
        .coefficientAverage = 3,
    }
};

/**
 * @brief Ensemble des donnees de tests pour le calcul de la position actuelle.
*/
static BeaconData parametersTestGetCurrentPositionB[4] = {

    {
        .position = { .X = 300, .Y= 1100},
        .power = -83.060425027,
        .coefficientAverage = 4,
    },
    {
        .position = { .X = 1500, .Y= 1100},
        .power = -91.36371724,
        .coefficientAverage = 4,
    },
    {
        .position = { .X = 1400, .Y= 100},
        .power = -88.06179975,
        .coefficientAverage = 4,
    },
    {
        .position = { .X = 1600, .Y= 1700},
        .power = -98,
        .coefficientAverage = 4,
    }
};

/**
 * @brief Ensemble des donnees de tests pour le calcul de la position actuelle.
*/
static BeaconData parametersTestGetCurrentPositionC[4] = {

    {
        .position = { .X = 300, .Y= 1100},
        .power = -83.060425027,
        .coefficientAverage = 4,
    },
    {
        .position = { .X = 1400, .Y= 100},
        .power = -88.06179975,
        .coefficientAverage = 4,
    },
    {
        .position = { .X = 1500, .Y= 1100},
        .power = -91.36371724,
        .coefficientAverage = 4,
    },
};

/**
 * @brief Ensemble des donnees de tests pour le calcul de la position actuelle.
*/
static BeaconData parametersTestGetCurrentPositionD[4] = {

    {
        .position = { .X = 1400, .Y= 100},
        .power = -88.06179975,
        .coefficientAverage = 4,
    },
    {
        .position = { .X = 1500, .Y= 1100},
        .power = -91.36371724,
        .coefficientAverage = 4,
    },
    {
        .position = { .X = 300, .Y= 1100},
        .power = -83.060425027,
        .coefficientAverage = 4,
    },
};

/**
 * @brief Ensemble des donnees de tests pour le calcul de la position actuelle.
 */
static ParametersTestGetCurrentPosition parameterTestCurrentPosition[] = {

    {
        .nbBeacon = 3,
        .beaconsData = parametersTestGetCurrentPositionA,
        .currentPosition = { .X = 0, .Y= 0},
        .expectedCurrentPosition = { .X = 800, .Y= 500},
    },
    {
        .nbBeacon = 4,
        .beaconsData = parametersTestGetCurrentPositionB,
        .currentPosition = { .X = 0, .Y= 0},
        .expectedCurrentPosition = { .X = 600, .Y= 500},
    },
    {
        .nbBeacon = 3,
        .beaconsData = parametersTestGetCurrentPositionC,
        .currentPosition = { .X = 0, .Y= 0},
        .expectedCurrentPosition = { .X = 600, .Y= 500},
    },
    {
        .nbBeacon = 3,
        .beaconsData = parametersTestGetCurrentPositionD,
        .currentPosition = { .X = 0, .Y= 0},
        .expectedCurrentPosition = { .X = 600, .Y= 500},
    }
};

/**
 * @brief Teste la moyenne des coefficient d'attenuation avec différentes balises
 *
 * @param state
 */
static void test_getAverageCalcul(void** state);

/**
 * @brief Suite de test de la conversion des structures de donnees en tableau d'octet.
 *
 */
static const struct CMUnitTest tests[] =
{
    // Calcul Distance avec la position
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


    // Calcul de la distance avec Puissance
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

    // Calcul du coefficient d'attenuation

    cmocka_unit_test_prestate(test_getAverageCalcul, &(parameterTest[0])),
    cmocka_unit_test_prestate(test_getAverageCalcul, &(parameterTest[1])),

    // Calcul du coefficient d'attenuation

    cmocka_unit_test_prestate(test_getAttenuationCoefficient, &(parametersTestGetAttenuationCoefficientA[0])),
    cmocka_unit_test_prestate(test_getAttenuationCoefficient, &(parametersTestGetAttenuationCoefficientB[0])),

    // Calcul la position actuelle

    cmocka_unit_test_prestate(test_getCurrentPosition, &(parameterTestCurrentPosition[0])),
    cmocka_unit_test_prestate(test_getCurrentPosition, &(parameterTestCurrentPosition[1])),
    cmocka_unit_test_prestate(test_getCurrentPosition, &(parameterTestCurrentPosition[2])),
    cmocka_unit_test_prestate(test_getCurrentPosition, &(parameterTestCurrentPosition[3])),
};

/**
 * @brief Lance la suite de test du module MathematicianLOG.
 *
 * @return 0 en cas de succees ou le numero du test qui a echoue.
 */
extern int mathematician_run_tests() {
    return cmocka_run_group_tests_name("Test of the module mathematicianLOG", tests, NULL, NULL);
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

static void test_getAverageCalcul(void** state) {
    ParametersTestGetAverageCalcul * param = (ParametersTestGetAverageCalcul*) *state;
    float result;
    result = Mathematician_getAverageCalcul(param->beaconCoefficientInput, param->nbBeaconInput);
    assert_float_equal(result, param->expectedAverage, EPSILON);
}

static void test_getAttenuationCoefficient(void** state) {
    ParametersTestGetAttenuationCoefficient * param = (ParametersTestGetAttenuationCoefficient*) *state;
    float result;
    result = Mathematician_getAttenuationCoefficient(&param->power, &param->beaconPosition,&param->calibrationPosition);
    assert_float_equal(result, param->expectedAttenuationCoefficient, EPSILON);
}

static void test_getCurrentPosition(void** state) {
    ParametersTestGetCurrentPosition  * param = (ParametersTestGetCurrentPosition *) *state;
    Mathematician_getCurrentPosition(param->beaconsData, param->nbBeacon,&param->currentPosition);
    assert_float_equal(param->currentPosition.X, param->expectedCurrentPosition.X,EPSILONPOSITION);
    assert_float_equal(param->currentPosition.Y, param->expectedCurrentPosition.Y,EPSILONPOSITION);
}
