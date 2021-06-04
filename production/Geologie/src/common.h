/**
 * @file common.h
 *
 * @brief Fichier de commonuration du projet.
 *
 * @version 1.0
 * @date 5 mai 2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#ifndef COMMON_
#define COMMON_


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
typedef unsigned long Date;


/**
 * @struct type
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

typedef struct
{
    /* data */
} CalibrationData;

typedef struct
{
    /* data */
} CalibrationPosition;





/**
 * @struct CalibrationPositionId
 * @brief Id d'une balise
 */
typedef int CalibrationPositionId;

/** * @brief  TODO
 *
 */

typedef struct
{
    /* data */
} ExperimentalTraject;

typedef struct
{
    /* data */
} ExperimentalPosition;

typedef struct
{
    /* data */
} BeaconSignal;

#endif // COMMON_
