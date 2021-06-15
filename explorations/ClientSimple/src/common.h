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

/**
 * @brief Le nombre d'octet composant le header d'une trame.
 *
 * @addtogroup sizeTrame
 */
#define SIZE_HEADER (3)

/**
 * @brief Tableau de uint8_t composant un message envoye entre GEOMOBILE et GEOLOGIE.
 */
typedef uint8_t Trame;

/**
 * @brief Composition du header des message envoye entre GEOMOBILE et GEOLOGIE.
 *
 */
typedef struct {
    uint8_t commande;   /**< La commande a envoyer/reçus. */
    uint16_t size;      /**< La taille des donnees envoyee. */
} Header;

/**
 * @typedef Commande
 *
 * @brief Enumeration contenant l'identifiant des differentes commandes.
 */
typedef enum {
    ASK_CALIBRATION_POSITIONS = 0x01,       /**< GEOMOBILE demande a GEOLOGIE la liste des positions de calibrage. */

    SEND_EXPERIMENTAL_POSITIONS = 0x02,     /**< GEOLOGIE envoie a GEOMOBILE la liste des positions experimentales. */
    SEND_EXPERIMENTAL_TRAJECTS = 0x03,      /**< GEOLOGIE envoie a GEOMOBILE la liste des trajets experimentaux. */
    SEND_MEMORY_PROCESSOR_LOAD = 0x04,      /**< GEOLOGIE envoie a GEOMOBILE la charge memoire et processeur de GEOLOGIE. */
    SEND_ALL_BEACONS_DATA = 0x05,           /**< GEOLOGIE envoie a GEOMOBILE les informations sur les balises. */
    SEND_CURRENT_POSITION = 0x06,           /**< GEOLOGIE envoie a GEOMOBILE la position actuelle de GEOLOGIE. */
    SEND_CALIBRATION_DATA = 0x07,           /**< GEOLOGIE envoie a GEOMOBILE les donnees de calibration */

    REP_CALIBRATION_POSITIONS = 0x08,       /**< GEOLOGIE envoie a GEOMOBILE les positions de calibrage. */

    SIGNAL_CALIBRATION_POSITION = 0x09,     /**< GEOMOBILE signale a GEOLOGIE qu’il est bien place a l’emplacement de calibrage. */
    SIGNAL_CALIRATION_END = 0x0A,           /**< GEOLOGIE signale a GEOMOBILE la fin du calibrage. */
    SIGNAL_CALIBRATION_END_POSITION = 0x0B, /**< GEOLOGIE signale a GEOMOBILE la fin du calibrage a la position actuelle */

    NB_COMMANDE = 12,                       /**< Le nombre de commande */
} Commande;

/**
 * @typedef enum ConnexionState
 *
 * @brief Enumeration contenant l'etat de la connexion entre GEOLOGIE et GEOMOBILE.
 */
typedef enum {
    DISCONNECTED = 0,   /**< La connexion n'est pas etablie */
    CONNECTED           /**< La connexion est etablie */
} ConnectionState;

#endif // COMMON_
