/**
 * @file config.h
 *
 * @brief Fichier de configuration du projet.
 *
 * @version 1.0
 * @date 5 mai 2021
 * @author GAUTIER Pierre-Louis
 * @copyright BSD 2-clauses
 *
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


#define NUMBER_OF_BEACONS
#define SIZE_BEACON_ID (3)
#define NB_CALIBRATION_POSITION (10)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @typedef Commande
 *
 * @brief Enumeration contenant l'identifiant des differentes commandes.
 */
typedef enum {
    ERROR = -1,

    ASK_CONNECTION = 0,
    ASK_EXPERIMENTAL_POSITION,
    ASK_ALL_BEACONS_DATA,
    ASK_CURRENT_POSITION,
    ASK_CALIBRATION_POSITION,
    ASK_MEMORY_PROCESSOR_LOAD,

    SIGNAL_CALIBRATION_START,
    SIGNAL_CALIBRATION_POSITION,
    SIGNAL_CALIBRATION_END,

    MIN_VALUE = 0,
    MAX_VALUE = SIGNAL_CALIBRATION_END
} Commande;

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

/**
 * @typedef type BeaconCoefficients
 * @brief type representant un tableau de coefficient d'attenuation pour une balise
 */
typedef float BeaconCoefficients[NUMBER_OF_BEACONS];

/**
 * @struct type
 * @brief cration d'une structure qui va prendre les diffeerentes donnees d'une balise
 */
typedef struct {
    char ID[SIZE_BEACON_ID];
    Position position;
    Power power;
    AttenuationCoefficient attenuationCoefficient;
    unsigned char nbCoefficientAttenuations;
    AttenuationCoefficient attenuationCoefficientsArray[NB_CALIBRATION_POSITION];
} BeaconData;

#endif // CONFIG_
