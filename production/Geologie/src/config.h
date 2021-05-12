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


#define SIZE_BEACON_DATA (19 * 8)
#define SIZE_POSITION_DATA (8 * 8)

#define COMMAND_POSITION_START (0)
#define COMMAND_SIZE (1)


#define NUMBER_OF_BEACONS
#define SIZE_BEACON_ID (3)
#define NB_CALIBRATION_POSITION (3)

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
 * @brief cration d'une structure Position qui va prendre les coordonnees d'un point
 */
typedef struct {
    int X;
    int Y;
}Position;

typedef float Power;

typedef float AttenuationCoefficient;

typedef float BeaconCoefficients[NUMBER_OF_BEACONS];

/**
 * @struct type
 * @brief cration d'une structure qui va prendre les donnes d'une balise
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
