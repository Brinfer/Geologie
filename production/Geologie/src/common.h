/**
 * @file common.h
 *
 * @brief Fichier commun du projet.
 *
 * @version 1.0
 * @date 5 mai 2021
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#ifndef COMMON_
#define COMMON_

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NB_BEACONS (3) // TODO move

/**
 * @brief  TODO
 *
 */
#define SIZE_BEACON_ID (3)

#define NB_CALIBRATION_POSITIONS (10) // TODO move

/**
 * @brief  TODO
 *
 */
#define ATT_COEFF_1_METER (6) // TODO move


typedef uint8_t ExperimentalPositionId;

typedef uint8_t ExperimentalTrajectId;

/**
 * @typedef type Power
 * @brief type representant une puissance de reception d'un signal bluetooth
 */
typedef float Power;

/**
 * @typedef type AttenuationCoefficient
 * @brief type representant un coefficient d'attenuation
 */
typedef float AttenuationCoefficient;

/**
 * @brief  TODO
 *
 */
typedef float BeaconCoefficients[NB_BEACONS];

/**
 * @brief  TODO
 *
 */
typedef float ProcessorAndMemoryLoad[2];

/**
 * @brief  TODO
 *
 */
typedef short NbCoefficient;

/**
 * @brief  TODO
 *
 */
typedef int CalibrationPositionId;

/**
 * @brief creation d'une structure qui va prendre les differentes donnees d'une balise
 */
typedef unsigned long Date;

/**
 * @brief structure qui va prendre les coordonnees d'un point (X et Y seront des entiers et seront en cm)
 */
typedef struct {
    int X;
    int Y;
} Position;

/**
 * @brief creation d'une structure qui va prendre les differentes donnees d'une balise
 */
typedef struct {
    char ID[SIZE_BEACON_ID];
    Position position;
    Power power;
    AttenuationCoefficient attenuationCoefficient;
    unsigned char nbCoefficientAttenuations;
    AttenuationCoefficient attenuationCoefficientsArray[NB_CALIBRATION_POSITIONS];
} BeaconData;

typedef struct {
    uint8_t TODO;
} CalibrationData;

typedef struct {
    CalibrationPositionId id;
    Position position;
} CalibrationPosition;

/**
 * @brief  TODO
 *
 */
typedef struct {
    ExperimentalTrajectId id;
    Position* traject;
} ExperimentalTraject;

typedef struct {
    ExperimentalPositionId id;
    Position position;
} ExperimentalPosition;

#endif // COMMON_
