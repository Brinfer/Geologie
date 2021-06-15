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

/**
 * @brief La taille en octet de l'identifiant d'une balise.
 *
 * L'identifiant est de 2 caractere (2 octets), le dernier octet est
 * reserve au caractere de fin de chaine.
 */
#define SIZE_BEACON_ID (3)
#define NB_CALIBRATION_POSITIONS (10)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Le type pour les identifiant de positions experimentales.
 *
 */
typedef uint8_t ExperimentalPositionId;

/**
 * @brief Le type pour les identifiant de trajets experimentalaux.
 *
 */
typedef uint8_t ExperimentalTrajectId;

/**
 * @brief Id d'une balise.
 */
typedef uint8_t CalibrationPositionId;

/**
 * @brief Type representant une puissance de reception d'un signal bluetooth.
 */
typedef float Power;

/**
 * @brief type representant un coefficient d'attenuation
 */
typedef float AttenuationCoefficient;

/**
 * @brief Un type permettant de sauvegarder le temps
 */
typedef uint64_t Date;

/**
 * @brief Une position caracterisee par des coordonnees X et Y.
 *
 * Les coordonnees sont des valeurs entieres superieur a 0.
 */
typedef struct {
    uint32_t X; /**< La coordonnees X. */
    uint32_t Y; /**< La coordonnees Y. */
} Position;

/**
 * @brief Un coefficient d'attenuation lie a une balise et a une position de calibration.
 *
 */
typedef struct {
    uint8_t beaconId[SIZE_BEACON_ID];               /**< L'identifiant de la balise auquel le coefficient d'attenuation est lie. */
    CalibrationPositionId positionId;               /**< L'identifiant de la position de calibration auquel le coefficient d'attenuation est lie. */
    AttenuationCoefficient attenuationCoefficient;  /**< Le coefficient d'attenuation. */
} BeaconCoefficients;

/**
 * @brief Creation d'une structure qui va prendre les differentes donnees d'une balise
 */
typedef struct {
    uint8_t ID[SIZE_BEACON_ID];
    Position position;
    Power power;
    AttenuationCoefficient coefficientAverage;
} BeaconData;

/**
 * @brief Stucture contenant les donnee de calibration d'une balise.
 */
typedef struct {
    uint8_t beaconId[SIZE_BEACON_ID];           /**< L'identifiant de la balise. */
    BeaconCoefficients* beaconCoefficient;      /**< Le tableau de #BeaconCoefficient lie a la balise. */
    uint8_t nbCoefficient;                      /**< Le nombre de #BeaconCoefficient lie a la balise. */
    AttenuationCoefficient coefficientAverage;  /**< La moyenne du tableau de #BeaconCoefficient lie a la balise. */
} CalibrationData;

/**
 * @brief Une position de calibration.
 *
 */
typedef struct {
    CalibrationPositionId id;   /**< L'identifiant de la position de calibration. */
    Position position;          /**< La #Position de la position de calibration. */
}CalibrationPosition;

/**
 * @brief Structure contenant un trajet experimental.
 */
typedef struct {
    ExperimentalTrajectId id;   /**< L'identifiant du trajet experimental. */
    uint8_t nbPosition;         /**< Le nombre de position composant le trajet experimental. */
    Position* traject;          /**< Le tableau de #Position composnant le trajet experimental */
} ExperimentalTraject;

/**
 * @brief Structure composant une position experimentale.
 */
typedef struct {
    ExperimentalPositionId id;  /**< L'identifiant de la position experimentale. */
    Position position;          /**< La #Position experimentale. */
} ExperimentalPosition;

/**
 * @brief Structure contenant les informations extraite d'un signal bluetoot.
 */
typedef struct {
    uint8_t name[SIZE_BEACON_ID];   /**< L'identifiant de la balise emettrice du signal */
    uint32_t uuid[2];               /**< Le mode du signal bluetooth. */
    int8_t rssi;                    /**< La puissance du signal bluetooth. */
    Position position;              /**< La #Position de la balise extraite de son signal. */
} BeaconSignal;

/**
 * @brief Structure contenant les informations les charges memoire et processeur.
 */
typedef struct {
    float memoryLoad;       /**< La charge memoire. */
    float processorLoad;    /**< La charge processeur. */
} ProcessorAndMemoryLoad;

#endif // COMMON_
