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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn static float distanceCalculWithPosition(Position p1, Position p2)
 * @brief methode privee permettant de calculer la distance entre deux positions
 *
 * @param p1 position du premier point
 * @param p2 position du deuxieme point
 * @return la distance entre ces deux points
 *
 */
static float distanceCalculWithPosition(const Position * p1, const Position * p2) { ///const pour pas modifier adresse
    float distance = 0;
    distance = sqrtf((p1->X - p2->X) * (p1->X - p2->X) + (p1->Y - p2->Y) * (p1->Y - p2->Y));
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
static float distanceCalculWithPower(const Power * power, const AttenuationCoefficient * attenuationCoefficient) {
    float distance = 0;
    float A = 0; ///correspond a ce qui est dans la puissance 
    A = ((*power) - ATT_COEFF_1_METER) / (-10 * (*attenuationCoefficient));
    distance = pow(10, A);
    return distance;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


extern AttenuationCoefficient Mathematician_getAttenuationCoefficient(const Power * power, const Position * beaconPosition, const Position * calibrationPosition) {
    AttenuationCoefficient attenuationCoefficient;
    float distance = distanceCalculWithPosition(beaconPosition, calibrationPosition);


    attenuationCoefficient = ((*power) - ATT_COEFF_1_METER) / (-10 * log10f(distance)); ///TODO revoir le calcuul, pas sur
    return attenuationCoefficient;
}

extern AttenuationCoefficient Mathematician_getAverageCalcul(const BeaconCoefficients beaconCoefficients) {
    AttenuationCoefficient somme = 0;
    AttenuationCoefficient AttenuationCoefficient = 0;
    for (int i = 0; i < NB_CALIBRATION_POSITIONS; i++) {
        somme = somme + beaconCoefficients[i];
    }
    AttenuationCoefficient = somme / NB_CALIBRATION_POSITIONS;
    return AttenuationCoefficient;
}


extern Position Mathematician_getCurrentPosition(const BeaconData beaconsData[NB_BEACONS]) {
    Position b1Position = beaconsData[0].position;
    /*Position b2Position = beaconsData[1].position;
    Position b3Position = beaconsData[2].position;
*/
    return b1Position;
}