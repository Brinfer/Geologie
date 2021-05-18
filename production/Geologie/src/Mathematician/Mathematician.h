
/**
 * @file Mathematician.h
 *
 * @brief Module permettant d'effectuer des calculs de position et de coefficients
 *
 * On va pouvoir calculer : la position actuelle du robot, le coefficient d'attebuation d'une position, la moyenne des coefficients d'attenuations d'une balise
 *
 * @version 1.0.1
 * @date 09/05/2021
 * @author Nathan BRIENT
 * @copyright BSD 2-clauses
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef MATHEMATICIAN_H
#define MATHEMATICIAN_H

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../config.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *
 * @brief cration d'un tableau constant public .
 *
 */
extern float tab[];

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn extern Mathematician_getAttenuationCoefficient(Power power, Position position, Position calibrationPosition)
 * @brief calcul du coefficient d'attenuation
 *
 * Alloue et initialise un objet de la classe Example
 *
 * @param [in] power prend la puissance recue de la balise
 * @param [in] beaconPosition position connue de la balise a calibrer
 * @param [in] calibrationPosition position a calibrer
 * @return le coefficient d'attenuation calcule
 *
 */
extern AttenuationCoefficient Mathematician_getAttenuationCoefficient(const Power * power, const Position * beaconPosition, const Position * calibrationPosition);

/**
* @fn extern AttenuationCoefficient Mathematician_getAverageCalcul(BeaconCoefficients beaconCoefficients)
* @brief calcul la moyenne des coefficients d'attenuations
*
* @param [in] beaconCoefficients tableau contenant les coefficients d'attenuations pour une balise
* @return la moyenne des coefficients d'attenuations
*/
extern AttenuationCoefficient Mathematician_getAverageCalcul(const BeaconCoefficients beaconCoefficients);


/**
* @fn extern AttenuationCoefficient Mathematician_getAverageCalcul(BeaconCoefficients beaconCoefficients)
* @brief calcul la moyenne des coefficients d'attenuations
*
* @param [in] beaconCoefficients tableau contenant les coefficients d'attenuations pour une balise
* @return la moyenne des coefficients d'attenuations
*/
extern AttenuationCoefficient Mathematician_getAverageCalcul(const BeaconCoefficients beaconCoefficients);

/**
* @fn extern AttenuationCoefficient Mathematician_getCurrentPosition(BeaconCoefficients beaconCoefficients)
* @brief calcul la moyenne des coefficients d'attenuations
*
* @param [in] beaconsData tableau contenant les informations des balises
* @return la position actuelle de la carte
*/
extern Position Mathematician_getCurrentPosition(const BeaconData beaconsData[]);

#endif
