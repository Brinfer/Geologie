/**
 * @file translatorLOG.h
 *
 * @brief Traduit une trame reçus ou construit le message a partie de donnees.
 *
 * @version 2.0
 * @date 03-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#ifndef TRANSLATOR_LOG_
#define TRANSLATOR_LOG_

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "../com_common.h"
#include "../../common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Traduit la trame en un #Header.
 *
 * Traduit le header de @a trame et place cette traduction dans @a p.
 *
 * @param trame La trame qui contient le header.
 * @param dest La structure #Header qui contiendra la traduction
 * @return int16_t -1 en cas d'erreur, la taille de la trame sinon.
 *
 *  @warning dest doit etre un pointeur initialise.
 */
extern int16_t TranslatorLog_translateTrameToHeader(const Trame *trame, Header dest);

/**
 * @brief Traduit les positions experimentales a envoyer en une trame.
 *
 * Traduit le tableau @a experimentalPosition en une #Trame et place la traduction dans @a dest.
 *
 * @param nbPositions Le nombre de position experimentale a envoyer.
 * @param experimentalPosition Les positions experimentales a envoyer.
 * @param dest La trame de destination de la traduction.
 * @return int16_t -1 en cas d'erreur, la taille de la trame sinon.
 *
 * @warning dest doit etre un pointeur initialise.
 */
extern int16_t TranslatorLog_translate4SendExperimentalPosition(uint8_t nbPositions, const ExperimentalPosition* experimentalPosition, Trame dest);

/**
 * @brief Traduit les donnees balise en une trame.
 *
 * Traduit le tableau @a beaconsData en une #Trame et place la traduction dans @a dest.
 *
 * @param nbBeacons Le nombre de donnee balise a traduire.
 * @param beaconsData Le donnees balise a traduire.
 * @param currentDate La date a laquelle les donnees on ete relevee.
 * @param dest La trame de destination de la traduction.
 * @return int16_t -1 en cas d'erreur, la taille de la trame sinon.
 *
 * @warning dest doit etre un pointeur initialise.
 */
extern int16_t TranslatorLog_translate4SendAllBeaconsData(uint8_t nbBeacons, const BeaconData* beaconsData, Date currentDate, Trame dest);

/**
 * @brief Traduit la position courante en une trame.
 *
 * Traduit @a currentPosition en une #Trame et place la traduction dans @a dest.
 *
 * @param currentPosition La position courante a traduire.
 * @param currentDate La date a laquelle les donnees on ete relevee.
 * @param dest La trame de destination de la traduction.
 * @return int16_t -1 en cas d'erreur, la taille de trame sinon.
 *
 * @warning dest doit etre un pointeur initialise.
 */
extern int16_t TranslatorLog_translate4SendCurrentPosition(const Position* currentPosition, Date currentDate, Trame dest);

/**
 * @brief Traduit les donnees de calibration en une trame.
 *
 * Traduit @a calibrationPosition en une #Trame et place la traduction dans @a dest.
 *
 * @param nbCalibrationPosition Le nombre de position de calibration a traduire.
 * @param calibrationPosition Les position de calibration a traduire.
 * @param dest La trame de destination de la traduction.
 * @return int16_t -1 en cas d'erreur, la taille de trame sinon.
 *
 * @warning dest doit etre un pointeur initialise.
 */
extern int16_t TranslatorLog_translateRepCalibrationPosition(uint8_t nbCalibrationPosition, const CalibrationPosition* calibrationPosition, Trame dest);

/**
 * @brief Traduit la charge processeur et memoire en une trame.
 *
 * Traduit @a processorAndMemoryLoad en une #Trame et place la traduction dans @a dest.
 *
 * @param processorAndMemoryLoad La charge processeur et memoire a traduire.
 * @param currentDate La date a laquelle les donnees on ete relevee.
 * @param dest La trame de destination de la traduction.
 * @return int16_t -1 en cas d'erreur, la taille de trame sinon.
 *
 * @warning dest doit etre un pointeur initialise.
 */
extern int16_t TranslatorLog_translateSendProcessorAndMemoryLoad(ProcessorAndMemoryLoad processorAndMemoryLoad, Date currentDate, Trame dest);

/**
 * @brief Traduit une trame en un identifiant de position de calibration.
 *
 * Traduit @a trame en un #CalibrationPositionId
 *
 * @param trame La trame a traduire.
 * @return CalibrationPositionId -1 en cas d'erreur, l'identifiant de la position de calibration sinon
 *
 * @warning dest doit etre un pointeur initialise.
 */
extern CalibrationPositionId TranslatorLog_treanslate4SignalCalibrationPosition(const Trame trame);

/**
 * @brief Traduit les trajets d'experimentation en une trame.
 *
 * Traduit @a experimentalTrajects en une #Trame et place la traduction dans @a dest.
 *
 * @param nbTraject Le nombre de trajet experimentaux.
 * @param ExperimentalTraject Les trajet experimentaux a traduire.
 * @param dest La trame de destination de la traduction.
 * @return int16_t -1 en cas d'erreur, la taille de trame sinon.
 *
 * @warning dest doit etre un pointeur initialise.
 */
extern int16_t translate4ExperimentalTrajects(uint8_t nbTraject, const ExperimentalTraject* experimentalTrajects, Trame dest);

#endif // TRANSLATOR_LOG_
