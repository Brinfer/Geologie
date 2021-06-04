/**
 * @file managerLOG.h
 *
 * @brief Permet le démarrage et l’extinction du logiciel. A aussi pour role de creer tous les objets
 *  et de demarrer les differentes machines à etats.
 * Cette objet permet aussi de stopper les machines a etats et de detruire tous les objets
 * instancies à l’extinction.
 *
 * @version 2.0
 * @date 03-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#ifndef MANAGER_LOG_
#define MANAGER_LOG_

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Creer tout les objets necessaire au logiciel.
 *
 */
extern void ManagerLOG_new(void);

/**
 * @brief Detruit tout les objets du logiciel
 *
 */
extern void ManagerLOG_free(void);

/**
 * @brief Demande le demarrage le logiciel.
 *
 */
extern void ManagerLOG_startGEOLOGIE(void);

/**
 * @brief Demande l'arret le logiciel.
 *
 */
extern void ManagerLOG_stopGEOLOGIE(void);

#endif /* MANAGER_LOG_ */
