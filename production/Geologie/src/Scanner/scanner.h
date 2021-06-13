/**
 * @file scanner.h
 * @brief TODO

 * Vous pouvez ici mettre un descriptif un peu plus long de ce que propose

 * le module (fichiers .h + .c). C'est là que vous expliquez les modalités

 * générales d'utilisation (préconditions, contexte d'utilisation, avertissements,

 * ...). TODO

 * @version 2.0

 * @date 03-06-2021

 * @author Gabriel LECENNE

 * @copyright Geo-Boot

 * @license BSD 2-clauses

 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Define

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**

 * @def SCANNER_H

 */

#ifndef SCANNER_H

#define SCANNER_H

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Include

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Receiver/receiver.h"

#include "../Bookkeeper/bookkeeper.h"

#include "../MathematicianLOG/mathematicianLOG.h"

#include "../common.h"

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
 * @fn extern int Scanner_new()
 * @brief Instancie et initialise l'objet Scanner
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
*/

extern void Scanner_new();

/**
 * @fn extern int Scanner_free()
 * @brief Libere et supprime l'objet Scanner
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
*/

extern void Scanner_free();

/**
 * @fn extern int Scanner_ask4StartScanner()
 * @brief Demande le démarrage de Scanner
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
*/

extern void Scanner_ask4StartScanner();

/**
 * @fn extern int Scanner_ask4StopScanner()
 * @brief Demande l'arret de Scanner
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
*/

extern void Scanner_ask4StopScanner();

/**
 * @fn extern int Scanner_ask4UpdateAttenuationCoefficientFromPosition(Position calibrationPosition)
 * @brief Demande la mise à jour du coefficient d'atténuation pour la position passee en parametre
 * @param calibrationPosition : Position de calibrage
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
*/

extern void Scanner_ask4UpdateAttenuationCoefficientFromPosition(CalibrationPosition calibrationPosition);

/**
 * @fn extern int Scanner_ask4AverageCalcul()
 * @brief Demande la moyenne des coefficients d'attenuation pour chacune des balises
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
*/

extern void Scanner_ask4AverageCalcul();

/**
 * @fn extern int Scanner_setAllBeaconsSignal(BeaconsSignal beaconsSignal)
 * @brief Envoie les donnée d’émission de toutes les balises détectées.
 * @param beaconsSignal : Id, Position et RSSI d'une balise
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
*/

extern void Scanner_setAllBeaconsSignal(BeaconSignal * beaconsSignal, uint32_t NbBeaconsAvailable);

/**
 * @fn extern int setCurrentProcessorAndMemoryLoad(ProcessorAndMemoryLoad currentProcessorAndMemoryLoad)
 * @brief Envoie les charges processeur et memoire
 * @param currentProcessorAndMemoryLoad : charge memoire et processeur
 *
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
*/

extern void setCurrentProcessorAndMemoryLoad(ProcessorAndMemoryLoad currentProcessorAndMemoryLoad);

extern void Scanner_setCurrentProcessorAndMemoryLoad(ProcessorAndMemoryLoad currentPAndMLoad);

#endif /* SCANNER_H */
