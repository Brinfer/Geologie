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
 * @fn static float distanceCalculWithPosition(Position p1, Position p2)
 * @brief methode privee permettant de calculer la distance entre deux positions
 *
 * @param p1 position du premier point
 * @param p2 position du deuxieme point
 * @return la distance entre ces deux points
 *
 */
static float distanceCalculWithPosition(Position p1, Position p2){
    float distance=0;
    distance=sqrtf((p1.X-p2.X)*(p1.X-p2.X)+(p1.Y-p2.Y)*(p1.Y-p2.Y));
    return distance;
}

/**
 * @fn static float distanceCalculWithPosition(Position p1, Position p2)
 * @brief methode privee permettant de calculer la distance entre deux positions
 *
 * @param p1 position du premier point
 * @param p2 position du deuxieme point
 * @return la distance entre ces deux points
 *
 */
static float distanceCalculWithPower(Power power, AttenuationCoefficientAverage attenuationCoefficientAverage){
    float distance=0;
    float A=0; ///correspond a ce qui est dans la puissance 
    A=(power-ATT_COEFF_1_METER)/(-10*attenuationCoefficientAverage);
    distance=pow(10,A);
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
    float distance = distanceCalculWithPosition(beaconPosition,calibrationPosition);
    
    
    attenuationCoefficient=(power-ATT_COEFF_1_METER)/(-10*log10f(distance)); ///TODO revoir le calcuul, pas sur
    return attenuationCoefficient;
}

/**
* @fn extern AttenuationCoefficientAverage getAverageCalcul(BeaconCoefficients beaconCoefficients)
* @brief calcul la moyenne des coefficients d'attenuations
*
* @param beaconCoefficients tableau contenant les coefficients d'attenuations pour une balise
* @return la moyenne des coefficients d'attenuations
*/
extern AttenuationCoefficientAverage getAverageCalcul(BeaconCoefficients beaconCoefficients){
    AttenuationCoefficientAverage somme=0;
    AttenuationCoefficientAverage attenuationCoefficientAverage=0;
    for(int i=0; i<NB_CALIBRATION_POSITIONS; i++){
        somme=somme+beaconCoefficients[i];
    }
    attenuationCoefficientAverage=somme/NB_CALIBRATION_POSITIONS;
    return attenuationCoefficientAverage;
}


/**
* @fn extern AttenuationCoefficientAverage getAverageCalcul(BeaconCoefficients beaconCoefficients)
* @brief calcul la moyenne des coefficients d'attenuations
*
* @param [in] beaconsData tableau contenant les informations des balises
* @return la position actuelle de la carte
*/
extern Position getCurrentPosition(BeaconData beaconsData[NB_BEACONS]){
    Position b1Position=beaconsData[0].position;
    Position b2Position=beaconsData[1].position;
    Position b3Position=beaconsData[2].position;


}