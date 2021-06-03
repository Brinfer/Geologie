/**
 * @file config.h
 *
 * @brief Fichier de configuration du projet.
 *
 * @version 1.0
 * @date 5 mai 2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#ifndef CONFIG_
#define CONFIG_


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define SIZE_BEACON_DATA (19 * 8)
#define SIZE_POSITION_DATA (8 * 8)

#define COMMAND_POSITION_START (0)
#define COMMAND_SIZE (1)


#define NB_BEACONS (3)
#define SIZE_BEACON_ID (3)
#define NB_CALIBRATION_POSITIONS (10)
#define ATT_COEFF_1_METER (6)

/**
 * @struct type
 * @brief structure qui va prendre les coordonnees d'un point (X et Y seront des entiers et seront en cm)
 */
typedef struct {
    int X;
    int Y;
}Position;

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

typedef float BeaconCoefficients[NB_BEACONS];

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

#endif // CONFIG_
