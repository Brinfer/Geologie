/**
 * @file Mathematician.c
 *
 * @version 1.0.1
 * @date 9/05/2021
 * @author Nathan BRIENT
 * @copyright BSD 2-clauses
 *
 */

#include "Mathematician.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h> /// pour les racines carr, carr...
/**
 * @fn static float distanceCalcul(Position p1, Position p2)
 * @brief methode privee permettant de calculer la distance entre deux positions
 *
 * @param p1 position du premier point
 * @param p2 position du deuxieme point
 * @return la distance entre ces deux points
 *
 */
static float distanceCalcul(Position p1, Position p2){
    float distance;
    distance=sqrtf((p1.X-p2.X)*(p1.X-p2.X)+(p1.Y-p2.Y)*(p1.Y-p2.Y));
    return distance;
}

/**
 * @fn extern getAttenuationCoefficient(Power power, Position position, Position calibrationPosition)
 * @brief calcul du coefficient d'attenuation
 *
 * Alloue et initialise un objet de la classe Example
 *
 * @param power prend la puissance recue de la balise
 * @param beaconPosition position connue de la balise a calibrer
 * @param calibrationPosition position a calibrer
 * @return le coefficient d'attenuation calcule
 *
 */
extern AttenuationCoefficient getAttenuationCoefficient(Power power, Position beaconPosition, Position calibrationPosition){
    AttenuationCoefficient attenuationCoefficient;
    float distance = distanceCalcul(beaconPosition,calibrationPosition);

    return attenuationCoefficient;
}

/**
* @fn extern AttenuationCoefficient getAverageCalcul(BeaconCoefficients beaconCoefficients)
* @brief calcul la moyenne des coefficients d'attenuations
*
* @param beaconCoefficients tableau contenant les coefficients d'attenuations pour une balise
* @return la moyenne des coefficients d'attenuations
*/
extern AttenuationCoefficient getAverageCalcul(BeaconCoefficients beaconCoefficients);


/**
* @fn extern AttenuationCoefficient getAverageCalcul(BeaconCoefficients beaconCoefficients)
* @brief calcul la moyenne des coefficients d'attenuations
*
* @param beaconCoefficients tableau contenant les coefficients d'attenuations pour une balise
* @return la moyenne des coefficients d'attenuations
*/
extern AttenuationCoefficient getAverageCalcul(BeaconCoefficients beaconCoefficients);

/**
* @fn extern AttenuationCoefficient getAverageCalcul(BeaconCoefficients beaconCoefficients)
* @brief calcul la moyenne des coefficients d'attenuations
*
* @param [in] beaconsData tableau contenant les informations des balises
* @return la position actuelle de la carte
*/
extern Position getCurrentPosition(BeaconData beaconsData[]);
