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
 * @struct type
 * @brief structure qui va prendre les coordonnees d'un point (X et Y seront des entiers et seront en cm)
 */
typedef struct {
    int X;
    int Y;
} Position;

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
 * @struct Header
 * @brief  TODO
 *
 */
 /**
  *  @brief  TODO
 *
 */
typedef float BeaconCoefficients[NB_BEACONS];


/**
 * @brief  TODO
 *
 */
typedef short NbCoefficient;


/**
 * @struct type
 * @brief creation d'une structure qui va prendre les differentes donnees d'une balise
 */
typedef unsigned long Date;

/**
 * @struct type
 * @brief creation d'une structure qui va prendre les differentes donnees d'une balise
 */
/**
 * @struct CalibrationPositionId
 * @brief Id d'une balise
 */
typedef int CalibrationPositionId;

typedef struct {
    char ID[SIZE_BEACON_ID];
    Position position;
    Power power;
    AttenuationCoefficient attenuationCoefficient;
    unsigned char nbCoefficientAttenuations;
    AttenuationCoefficient attenuationCoefficientsArray[NB_CALIBRATION_POSITIONS];
} BeaconData;

typedef struct {
    uint8_t baliseId;
    BeaconCoefficients * beaconCoefficient;
} CalibrationData;

typedef struct {
    CalibrationPositionId id;
    Position position;
} CalibrationPosition;






/** * @brief  TODO
 *
 */

typedef struct {
    ExperimentalTrajectId id;
    Position* traject;
    uint8_t nbPosition;
} ExperimentalTraject;

typedef struct {
    ExperimentalPositionId id;
    Position position;
} ExperimentalPosition;

/**
 * @brief  TODO
 *
 */
typedef struct {
    float memoryLoad;
    float processorLoad;
} ProcessorAndMemoryLoad;

typedef struct {
    /* data */
} BeaconSignal;

#endif // COMMON_
