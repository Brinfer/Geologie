/**
 * @file geographer.h
 *
 * @brief Ce module est en quelques sorte le chef d'orchestre de Geologie,
 *
 * Vous pouvez ici mettre un descriptif un peu plus long de ce que propose
 * le module (fichiers .h + .c). C'est là que vous expliquez les modalités
 * générales d'utilisation (préconditions, contexte d'utilisation, avertissements,
 * ...).
 *
 * @version 2.0
 * @date 03-06-2021
 * @author Nathan BRIENT
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @def GEOGRAPHER_H
 * @def ROBOT_HEIGHT_MM
 */

#ifndef GEOGRAPHER_H
#define GEOGRAPHER_H


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "../Scanner/scanner.h"
#include "../CommGeologie/ProxyLoggerMOB/proxyLoggerMOB.h"
#include "../CommGeologie/ProxyGUI/proxyGUI.h"
#include "../CommGeologie/com_common.h"
#include "../common.h"
#include "../tools.h"

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
 * @fn extern uint8_t Geographer_new()
 * @brief Instancie et initialise l'objet Geographer
 * *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
 *
*/
extern uint8_t Geographer_new();

/**
 * @fn extern uint8_t Geographer_free()
 * @brief Libere et supprime l'objet Geographer
 * *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
 *
*/
extern uint8_t Geographer_free();

/**
 * @fn extern uint8_t askSignalStartGeographer()
 * @brief Demande le démarrage de geographer
 *
 * Demande à geographer de démarrer, démarre le thread
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
 *
*/
extern uint8_t Geographer_askSignalStartGeographer();

/**
 * @fn extern uint8_t askSignalSStopGeographer()
 * @brief Demande l'arret de geographer
 *
 * Demande à geographer de s'arreter, arrete le thread ferme la boite aux lettres
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
 *
*/
extern uint8_t Geographer_askSignalStopGeographer();

/**
 * @fn extern uint8_t askCalibrationPositions()
 * @brief Demande les positions de calibrage
 *
 * Demande à geographer les positions de calibrage qui les enverra a GUI
 *
 * Cette methode sera appellee par GUI
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
 *
*/
extern uint8_t Geographer_askCalibrationPositions();

/**
 * @fn extern uint8_t Geographer_validatePosition(CalibrationPositionId calibrationPositionId)
 *
 * @brief Valide la demande de calibration à la position ayant calibrationPositionId comme Id
 *
 *
 * Cette methode sera appellee par GUI
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
 *
*/
extern uint8_t Geographer_validatePosition(CalibrationPositionId calibrationPositionId);

/**
 * @fn extern uint8_t Geographer_signalEndUpdateAttenuation()
 *
 * @brief Signale la fin du calcul d'attenuation pour une balise
 *
 *
 * Cette methode sera appellee par Scanner
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
 *
*/
extern uint8_t Geographer_signalEndUpdateAttenuation();

/**
 * @fn extern uint8_t Geographer_signalEndAverageCalcul(CalibrationData calibrationData[])
 *
 * @brief Signale la fin du calcul d'attenuation moyen pour chaque balise
 *
 * On mettra ensuite a jour les donnees des balises avec les donnees de calibrage
 *
 * Cette methode sera appellee par Scanner
 *
 * @param calibrationData tableau contenat les donnees de calibrage pour chaque balise
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
 *
*/
extern uint8_t Geographer_signalEndAverageCalcul(CalibrationData calibrationData[], uint8_t nbCalibration);

/**
 * @fn extern uint8_t Geographer_signalConnectionEstablished()
 *
 * @brief Signale la connexion entre Geologie et Geomobile
 *
 * On mettra ensuite a jour la variable connectionState
 *
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
 *
*/
extern uint8_t Geographer_signalConnectionEstablished();

/**
 * @fn extern uint8_t Geographer_signalConnectionDown()
 *
 * @brief Signale la fin de la connexion entre Geologie et Geomobile
 *
 * On mettra ensuite a jour la variable connectionState
 *
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
 *
*/
extern uint8_t Geographer_signalConnectionDown();

/**
 * @fn extern uint8_t Geographer_dateAndSendData(BeaconData beaconsData[], Position currentPosition, ProcessorAndMemoryLoad currentProcessorAndMemoryLoad)
 *
 * @brief Reçoit les donnee actuelle, les dates et les renvoie
 *
 * Cette méthode intervient dans la mise à jour automatique des donnees
 *
 * @param beaconsData[] tableau contenant les donnees des balises
 * @param currentPosition position actuelle de la carte mere
 * @param currentProcessorAndMemoryLoad charge processeur et memoire actuelle
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
 *
*/
extern uint8_t Geographer_dateAndSendData(BeaconData * beaconsData, uint8_t beaconsDataSize, Position * currentPosition, ProcessorAndMemoryLoad currentProcessorAndMemoryLoad);

#endif /* GEOGRAPHER_H */
