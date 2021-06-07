/**
 * @file DispatcherLOG.h
 *
 * @brief  Classe permettant de recevoir les demandes cote GEOLOGIE et effectuer des actions en fonction de ces demandes
 *
 * @version 2.0
 * @date 03-06-2021
 * @author Nathan BRIENT
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

/**
 * @def DISCPATCHERLOG_H
 * @def ROBOT_HEIGHT_MM
 */

#ifndef DISCPATCHERLOG_H
#define DISCPATCHERLOG_H


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "../../Geographer/geographer.h"
#include "../PostmanLOG/postmanLOG.h"
#include "../TranslatorLOG/translatorLOG.h"

#include "../../common.h"
#include "../com_common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Instancie et initialise l'objet DispatcherLOG
 * *
 * @return retourne 1 s'il y a une erreur dans l'execution de la methode
 *
*/
extern int8_t DispatcherLOG_new();

/**
 * @brief Libere et supprime l'objet DispatcherLOG
 * *
 * @return retourne 1 s'il y a une erreur dans l'execution de la methode
 *
*/
extern int8_t DispatcherLOG_free();

/**
 * @brief Demande le demarrage de dispatcherLOG
 *
 * Demande a dispatcherLOG de demarrer, demarre le thread
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la methode
 *
*/
extern int8_t DispatcherLOG_start();

/**
 * @brief Demande l'arret de dispatcherLOG
 *
 * Demande a dispatcherLOG de s'arreter, arrete le thread ferme la boite aux lettres
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la methode
 *
*/
extern int8_t DispatcherLOG_stop();

/**
 * @brief  TODO
 * 
 * @param connectionState 
 * @return int8_t 
 */
extern int8_t DispatcherLOG_setConnectionState(ConnectionState connectionState);

#endif /* DISCPATCHERLOG_H */
