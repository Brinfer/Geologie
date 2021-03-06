/**
 * @file managerLOG.h
 *
 * @brief Permet le demarrage et l’extinction de GEOLOGIE. A aussi pour role de creer tous les objets
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
 * @brief Demande le demarrage de GEOLOGIE.7
 *
 * Dans le cas ou un module de GEOLOGIE echoue a demarrer, alors une deuxieme tentative est faite.
 * Si la deuxieme tentative echoue elle aussi le programme s'arrete.
 */
extern void ManagerLOG_startGEOLOGIE(void);

/**
 * @brief Demande l'arret de GEOLOGIE.
 *
 * Dans le cas ou un module de GEOLOGIE echoue a s'arreter, alors une deuxieme tentative est faite.
 * Si la deuxieme tentative echoue elle aussi le programme continue.
 *
 */
extern void ManagerLOG_stopGEOLOGIE(void);

#endif /* MANAGER_LOG_ */
