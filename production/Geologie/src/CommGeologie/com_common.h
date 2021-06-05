/**
 * @file com_common.h
 *
 * @brief Fichier contenant les types en commun dans le module dedier a la communication de GEOLOGIE
 *
 * @version 2.0
 * @date 03-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#ifndef COM_COMMON_
#define COM_COMMON_

#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Le nombre d'octet composant le header d'une trame.
 */
#define SIZE_HEADER (3)

/**
 * @brief Tableau de uint8_t composant un message envoye entre GEOMOBILE et GEOLOGIE.
 */
typedef uint8_t Trame[];

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
    ASK_CALIBRATION_POSITIONS = 0x01,   /**< GEOMOBILE demande à GEOLOGIE la liste des positions de calibrage. */

    SEND_EXPERIMENTAL_POSITIONS = 0x55, /**< GEOLOGIE envoie à GEOMOBILE la liste des positions experimentales. */
    SEND_EXPERIMENTAL_TRAJECTS = 0x56,  /**< GEOLOGIE envoie à GEOMOBILE la liste des trajets experimentaux. */
    SEND_MEMORY_PROCESSOR_LOAD = 0x57,  /**< GEOLOGIE envoie à GEOMOBILE la charge memoire et processeur de GEOLOGIE. */
    SEND_ALL_BEACONS_DATA = 0x58,       /**< GEOLOGIE envoie à GEOMOBILE les informations sur les balises. */
    SEND_CURRENT_POSITION = 0x59,       /**< GEOLOGIE envoie à GEOMOBILE la position actuelle de GEOLOGIE. */

    REP_CALIBRATION_POSITIONS = 0x60,   /**< GEOLOGIE envoie à GEOMOBILE les positions de calibrage. */

    SIGNAL_CALIBRATION_START = 0xAA,    /**< GEOMOBILE signale à GEOLOGIE une nouvelle session de calibrage. */
    SIGNAL_CALIBRATION_POSITION = 0xAB, /**< GEOMOBILE signale à GEOLOGIE qu’il est bien place à l’emplacement de calibrage. */
    SIGNAL_CALIRATION_END = 0xAC,       /**< GEOLOGIE signale à GEOMOBILE la fin du calibrage. */

    NB_COMMANDE = 10,                   /**< Le nombre de commande */
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

#endif // COM_COMM_
