/**
 * @file DispatcherLOG.h
 * 
 * @brief  Classe permettant de recevoir les demandes cote Geologie et effectuer des actions en fonction de ces demandes
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
#include "../../Geographer/geographer.h"
#include "../PostmanLOG/postmanLOG.h"
#include "../TranslatorLOG/translatorLOG.h"

#include "../../common.h"
#include "../com_common.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn extern int DispatcherLOG_new()
 * @brief Instancie et initialise l'objet DispatcherLOG
 * *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
 * 
*/
extern int DispatcherLOG_new();

/**
 * @fn extern int DispatcherLOG_free()
 * @brief Libere et supprime l'objet DispatcherLOG
 * *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
 * 
*/
extern int DispatcherLOG_free();

/**
 * @fn extern int ask4SignalStartDispatcherLOG()
 * @brief Demande le démarrage de dispatcherLOG
 *
 * Demande à dispatcherLOG de démarrer, démarre le thread
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
 * 
*/
extern int DispatcherLOG_start();

/**
 * @fn extern int ask4SignalSStopDispatcherLOG()
 * @brief Demande l'arret de dispatcherLOG
 *
 * Demande à dispatcherLOG de s'arreter, arrete le thread ferme la boite aux lettres
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
 * 
*/
extern int DispatcherLOG_stop();


#endif /* DISCPATCHERLOG_H */