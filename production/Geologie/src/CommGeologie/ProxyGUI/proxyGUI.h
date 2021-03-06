/**
 * @file proxyGUI.h
 *
 * @brief  Proxy pour utilser les methodes de GUI cote GEOLOGIE
 *
 * @version 2.0
 * @date 03-06-2021
 * @author Nathan BRIENT
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#ifndef PROXYGUI_H
#define PROXYGUI_H


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../PostmanLOG/postmanLOG.h"
#include "../TranslatorLOG/translatorLOG.h"
#include "../com_common.h"
#include "../../common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * @fn extern int ProxyGUI_new()
 * @brief Instancie et initialise l'objet ProxyGUI
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la methode
 *
*/
extern int8_t ProxyGUI_new();

/**
 * @fn extern int ProxyGUI_free()
 * @brief Libere et supprime l'objet ProxyGUI
 * *
 * @return retourne 1 s'il y a une erreur dans l'execution de la methode
 *
*/
extern int8_t ProxyGUI_free();

/**
 * @fn extern int askSignalStartProxyGUI()
 * @brief Demande le demarrage de proxyGUI
 *
 * Demande a proxyGUI de demarrer, demarre le thread
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la methode
 *
*/
extern int8_t ProxyGUI_start();

/**
 * @fn extern int askSignalSStopProxyGUI()
 * @brief Demande l'arret de proxyGUI
 *
 * Demande a proxyGUI de s'arreter, arrete le thread ferme la boite aux lettres
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la methode
 *
*/
extern int8_t ProxyGUI_stop();

/**
 * @fn extern int setCalibrationPositions(CalibrationPosition calibrationPositions[]);
 * @brief Envoie les position de calibration
 *
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la methode
 *
*/
extern int8_t ProxyGUI_setCalibrationPositions(const CalibrationPosition * calibrationPositions, uint8_t nbCalibrationPosition);

/**
 * @fn extern int ProxyGUI_signalEndCalibrationPosition();
 * @brief Signale la fin de la calibration pour la position
 *
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la methode
 *
*/
extern int8_t ProxyGUI_signalEndCalibrationPosition();

/**
 * @fn extern int ProxyGUI_signalEndCalibration();
 * @brief Signale la fin de la calibration
 *
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la methode
 *
*/
extern int8_t ProxyGUI_signalEndCalibration();

#endif // PROXYGUI_H
