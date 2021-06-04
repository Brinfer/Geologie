/**
 * @file com_common.h
 *
 * @brief Fichier contenant les types en commun dans le module dédier à la communication de Geologie
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

typedef uint8_t Trame[];

/**
 * @struct Header
 * @brief  TODO
 *
 */
typedef struct {
    /* TODO */
} Header;

/**
 * @typedef Commande
 *
 * @brief Enumeration contenant l'identifiant des differentes commandes.
 */
typedef enum {
    /* TODO */
    TODO = 0,
} Commande;

/**
 * @typedef enum ConnexionState
 *
 * @brief Enumeration contenant l'etat de la connexion entre Geologie et Geomobile.
 */
typedef enum {
    DISCONNECTED = 0,
    CONNECTED
} ConnectionState;

/**
 * @brief  TODO
 *
 */


#endif // COM_COMM_
