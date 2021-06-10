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
 * @brief Donne la taille de la trame necessaire pour envoyer le message de la commande en fonction du
 * nombre d'element qui la compose si celle-ci est de taille variable, la taille du header est comprise.
 *
 * Dans le cas ou la commande n'est pas de taille variable, l'argument @a nbElements est ignore.
 *
 * @a nbElements peut correspondre au pare exemple nombre de #BeaconData a envoyer pour le message #SEND_ALL_BEACONS_DATA, ou
 * au nombre de position de calibration du message #REP_CALIBRATION_POSITIONS.
 *
 * @param cmd La commande dont on veut connaitre la taille de la trame.
 * @param nbElements Le nombre d'element composant le message.
 * @return uint16_t La taille que la trame doit avoir pour pouvoir envoyer le message.
 *
 * @warning Pour la commande #SEND_EXPERIMENTAL_TRAJECTS, il faut utiliser #TranslatorLOG_getTrameSizeExperimentalTraject.
 */
extern uint16_t TranslatorLOG_getTrameSize(Commande cmd, uint8_t nbElements);

/**
 * @brief Donne la taille de la trame necessaire pour envoyer le message de la commande #SEND_EXPERIMENTAL_TRAJECTS.
 *
 * @param experimentalTrajects Le trajet de trajet experimentaux a envoyer.
 * @param nbExperimentalTrajects Le nombre de trajet experimentaux.
 * @return uint16_t La taille que la trame doit avoir pour pouvoir envoyer le message.
 */
extern uint16_t TranslatorLOG_getTrameSizeExperimentalTraject(const ExperimentalTraject* experimentalTrajects, uint8_t nbExperimentalTrajects);

/**
 * @brief Traduit la trame en un #Header.
 *
 * Traduit le header de @a trame et place cette traduction dans @a dest.
 *
 * @param trame La trame qui contient le header.
 * @param dest La structure #Header qui contiendra la traduction
 *
 * @warning @a dest doit etre de la bonne taille.
 * @see #SIZE_HEADER
 */
extern void TranslatorLOG_translateTrameToHeader(const Trame* trame, Header* dest);

/**
 * @brief Traduit les positions experimentales a envoyer en une trame. Compose aussi le header.
 *
 * Traduit le tableau @a experimentalPosition en une #Trame et place la traduction dans @a dest.
 *
 * @param nbExperimentalPositions Le nombre de position experimentale a envoyer.
 * @param experimentalPosition Les positions experimentales a envoyer.
 * @param dest La trame de destination de la traduction.
 *
 * @warning @a dest doit etre de la bonne taille.
 * @see #TranslatorLOG_getTrameSize
 */
extern void TranslatorLOG_translateForSendExperimentalPositions(uint8_t nbPosinbExperimentalPositionstions, const ExperimentalPosition* experimentalPositions, Trame* dest);

/**
 * @brief Traduit les trajets d'experimentation en une trame. Compose aussi le header.
 *
 * Traduit @a experimentalTrajects en une #Trame et place la traduction dans @a dest.
 *
 * @param ExperimentalTraject Les trajet experimentaux a traduire.
 * @param nbTraject Le nombre de trajet experimentaux.
 * @param dest La trame de destination de la traduction.
 *
 * @warning @a dest doit etre de la bonne taille.
 * @see #TranslatorLOG_getTrameSize
 */
extern void TranslatorLOG_translateForSendExperimentalTrajects(const ExperimentalTraject* experimentalTrajects, uint8_t nbTraject, Trame* dest);

/**
 * @brief Traduit les donnees balise en une trame. Compose aussi le header.
 *
 * Traduit le tableau @a beaconsData en une #Trame et place la traduction dans @a dest.
 *
 * @param nbBeacons Le nombre de donnee balise a traduire.
 * @param beaconsData Le donnees balise a traduire.
 * @param currentDate La date a laquelle les donnees on ete relevee.
 * @param dest La trame de destination de la traduction.
 *
 * @warning @a dest doit etre de la bonne taille.
 * @see #TranslatorLOG_getTrameSize
 */
extern void TranslatorLOG_translateForSendAllBeaconsData(uint8_t nbBeacons, const BeaconData* beaconsData, Date currentDate, Trame* dest);

/**
 * @brief Traduit la position courante en une trame. Compose aussi le header.
 *
 * Traduit @a currentPosition en une #Trame et place la traduction dans @a dest.
 *
 * @param currentPosition La position courante a traduire.
 * @param currentDate La date a laquelle les donnees on ete relevee.
 * @param dest La trame de destination de la traduction.
 *
 * @warning @a dest doit etre de la bonne taille.
 * @see #TranslatorLOG_getTrameSize
 */
extern void TranslatorLOG_translateForSendCurrentPosition(const Position* currentPosition, Date currentDate, Trame* dest);

/**
 * @brief Traduit les donnees de calibration en une trame. Compose aussi le header.
 *
 * Traduit @a calibrationPosition en une #Trame et place la traduction dans @a dest.
 *
 * @param nbCalibrationPositions Le nombre de position de calibration a traduire.
 * @param calibrationPosition Les position de calibration a traduire.
 * @param dest La trame de destination de la traduction.
 *
 * @warning @a dest doit etre de la bonne taille.
 * @see #TranslatorLOG_getTrameSize
 */
extern void TranslatorLOG_translateForRepCalibrationPosition(uint8_t nbCalibrationPositions, const CalibrationPosition* calibrationPositions, Trame* dest);

/**
 * @brief Traduit la charge processeur et memoire en une trame. Compose aussi le header.
 *
 * Traduit @a processorAndMemoryLoad en une #Trame et place la traduction dans @a dest.
 *
 * @param processorAndMemoryLoad La charge processeur et memoire a traduire.
 * @param currentDate La date a laquelle les donnees on ete relevee.
 * @param dest La trame de destination de la traduction.
 *
 * @warning @a dest doit etre de la bonne taille.
 * @see #TranslatorLOG_getTrameSize
 */
extern void TranslatorLOG_translateForSendMemoryAndProcessorLoad(const ProcessorAndMemoryLoad* processorAndMemoryLoad, Date currentDate, Trame* dest);

/**
 * @brief Traduit une trame en un identifiant de position de calibration.
 *
 * Traduit @a trame en un #CalibrationPositionId
 *
 * @param trame La trame a traduire.
 * @return CalibrationPositionId -1 en cas d'erreur, l'identifiant de la position de calibration sinon
 *
 * @warning @a trame doit contenir le #Header.
 * @see #TranslatorLOG_getTrameSize
 */
extern CalibrationPositionId TranslatorLOG_translateForSignalCalibrationPosition(const Trame* trame);

/**
 * @brief Compose la trame pour la commande #SIGNAL_CALIRATION_END. Compose aussi le header.
 *
 * Compose une une #Trame et la place dans @a dest.
 *
 * @param dest La trame de destination du message.
 *
 * @warning @a dest doit etre de la bonne taille.
 * @see #TranslatorLOG_getTrameSize
 */
extern void TranslatorLOG_translateForSignalCalibrationEnd(Trame* dest);

/**
 * @brief Compose la trame pour la commande #SIGNAL_CALIBRATION_END_POSITION. Compose aussi le header.
 *
 * Compose une une #Trame et la place dans @a dest.
 *
 * @param dest La trame de destination du message.
 *
 * @warning @a dest doit etre de la bonne taille.
 * @see #TranslatorLOG_getTrameSize
 */
extern void TranslatorLOG_translateForSignalCalibrationEndPosition(Trame* dest);

/**
 * @brief Compose la trame pour la commande #SEND_CALIBRATION_DATA. Compose aussi le header.
 *
 * Compose une une #Trame et la place dans @a dest.
 *
 * @param calibrationsData Les donnees de calibration a envoyer.
 * @param nbCalibrationData Le nombre de donnee de calibration a envoyer.
 * @param dest La trame de destination du message.
 *
 * @warning @a dest doit etre de la bonne taille.
 * @see #TranslatorLOG_getTrameSize
 */
extern void TranslatorLOG_translateForSendCalibrationData(const CalibrationData* calibrationsData, uint8_t nbCalibrationData, Trame* dest);

#endif // TRANSLATOR_LOG_
