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
#define ATT_COEFF_1_METER (-50) // TODO move

/**
 * @brief  TODO
 *
 */
typedef uint8_t ExperimentalPositionId;

/**
 * @brief  TODO
 *
 */
typedef uint8_t ExperimentalTrajectId;

/**
 * @brief Id d'une balise
 */
typedef uint8_t CalibrationPositionId;

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
  *  @brief  TODO
 *
 */
typedef struct {
    uint8_t positionId;
    AttenuationCoefficient attenuationCoefficient;
}BeaconCoefficients;

/**
 * @brief  TODO
 *
 */
typedef uint8_t NbCoefficient;

/**
 * @brief creation d'une structure qui va prendre les differentes donnees d'une balise
 */
typedef uint64_t Date;

/**
 * @brief structure qui va prendre les coordonnees d'un point (X et Y seront des entiers et seront en cm)
 */
typedef struct {
    uint32_t X;
    uint32_t Y;
} Position;

/**
 * @brief creation d'une structure qui va prendre les differentes donnees d'une balise
 */
typedef struct {
    uint8_t ID[SIZE_BEACON_ID];
    Position position;
    Power power;
    AttenuationCoefficient coefficientAverage;
} BeaconData;

typedef struct {
    uint8_t beaconId;
    BeaconCoefficients* beaconCoefficient;
    uint8_t nbCoefficient;
    AttenuationCoefficient coefficientAverage;
} CalibrationData;

typedef struct {
    CalibrationPositionId id;
    Position position;
}CalibrationPosition;


/** * @brief  TODO
 *
 */
typedef struct {
    ExperimentalTrajectId id;
    uint8_t nbPosition;
    Position* traject;
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
    /* data */
} BeaconSignal;

/**
 * @brief  TODO
 *
 */
typedef struct {
    float memoryLoad;
    float processorLoad;
} ProcessorAndMemoryLoad;

#endif // COMMON_
