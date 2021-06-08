/**
 * @file proxyLoggerMOB.h
 *
 * @brief Toutes les requetes qui doivent etre envoyees à LoggerMOB passent par cet objet.
 *
 * ProxyLoggerMOB utilise la classe TranslatorLOG pour traduire les messages avant de les envoyer, afin qu'ils
 * correspondent au protocole de communication.
 *
 * @version 2.0
 * @date 03-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#ifndef PROXY_LOGGER_MOB_
#define PROXY_LOGGER_MOB_

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../com_common.h"
#include "../../common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialise ProxyLoggerMOB.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t ProxyLoggerMOB_new(void);

/**
 * @brief Demarre ProxyLoggerMOB.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t ProxyLoggerMOB_start(void);

/**
 * @brief Stoppe ProxyLoggerMOB.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t ProxyLoggerMOB_stop(void);

/**
 * @brief Libere le ProxyLoggerMOB.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t ProxyLoggerMOB_free(void);

/**
 * @brief Envoie les trajet d'experimentation a LoggerMOB.
 *
 * @param experimentalTrajects Les trajets experimentaux a envoyer.
 * @param nbExperimentalTraject Le nombre de trajet experimentaux a envoyer.
 * @return int8_t 0 en cas de succes, une autre valeur sinon.
 */
extern int8_t ProxyLoggerMOB_setExperimentalTrajects(const ExperimentalTraject* experimentalTrajects, unsigned short nbExperimentalTraject);

/**
 * @brief Envoie les position d'experimentation a LoggerMOB
 *
 * @param experimentalPositions Les position experimentale a envoyer
 * @param nbExperimentalPosition Le nombre de position experimentale a envoyer
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t ProxyLoggerMOB_setExperimentalPositions(const ExperimentalPosition* experimentalPositions, unsigned short nbExperimentalPosition);

/**
 * @brief Envoie les donnees de calibration a LoggerMOB.
 *
 * @param calibrationData Les donnees de calibration a envoyer.
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t ProxyLoggerMOB_setCalibrationData(const CalibrationData* calibrationData);

/**
 * @brief Envoie les donnees des balises a LoggerMOB.
 *
 * @param beaconsData Les donnees des balises a envoyer.
 * @param nbBeacons Le nombre de donnees balise a envoyer
 * @param currentDate La date a laquelle les donnees on ete relevee.
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t ProxyLoggerMOB_setAllBeaconsData(const BeaconData* beaconsData, unsigned short nbBeacons, Date currentDate);

/**
 * @brief Envoie la position actuelle a LoggerMOB.
 *
 * @param currentPosition La position a envoyer.
 * @param currentDate La date a laquelle la position a ete relevee.
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t ProxyLoggerMOB_setCurrentPosition(const Position* currentPosition, Date currentDate);

/**
 * @brief Envoie la charge memoire et processeur a LoggerMOB.
 *
 * @param processorAndMemoryLoad
 * @param currentDate La date a laquelle la charge memoire et processuer ont ete relevees.
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t ProxyLoggerMOB_setProcessorAndMemoryLoad(const ProcessorAndMemoryLoad* processorAndMemoryLoad, Date currentDate);

#endif // PROXY_LOGGER_MOB_
