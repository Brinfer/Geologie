/**
 * @file mathematician.c
 *
 * @version 1.0.1
 * @date 9/05/2021
 * @author Nathan BRIENT
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#include "mathematician.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h> // pour les racines carr, carr...

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn static double distanceCalculWithPosition(Position p1, Position p2)
 * @brief methode privee permettant de calculer la distance entre deux positions
 *
 * @param p1 position du premier point
 * @param p2 position du deuxieme point
 * @return la distance entre ces deux points
 *
 */
static double distanceCalculWithPosition(const Position* p1, const Position* p2) { //const pour pas modifier adresse
    double distance = 0;
    distance = sqrtf((p1->X - p2->X) * (p1->X - p2->X) + (p1->Y - p2->Y) * (p1->Y - p2->Y));
    return distance;
}

/**
 * @fn static double distanceCalculWithPosition(Position p1, Position p2)
 * @brief methode privee permettant de calculer la distance entre deux positions
 *
 * @param p1 position du premier point
 * @param p2 position du deuxieme point
 * @return la distance entre ces deux points
 *
 */
static double distanceCalculWithPower(const Power* power, const AttenuationCoefficient* attenuationCoefficient) {
    double distance = 0;
    double A = 0; ///correspond a ce qui est dans la puissance
    A = ((*power) - ATT_COEFF_1_METER) / (-10 * (*attenuationCoefficient));
    distance = pow(10, A);
    return distance;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


extern AttenuationCoefficient Mathematician_getAttenuationCoefficient(const Power* power, const Position* beaconPosition, const CalibrationPosition* calibrationPosition) {
    AttenuationCoefficient attenuationCoefficient;
    double distance = distanceCalculWithPosition(beaconPosition, calibrationPosition);


    attenuationCoefficient = ((*power) - ATT_COEFF_1_METER) / (-10 * log10f(distance)); //TODO revoir le calcul, pas sur
    return attenuationCoefficient;
}

extern AttenuationCoefficient Mathematician_getAverageCalcul(const BeaconCoefficients *  beaconCoefficients,  uint8_t nbCoefficient) {
    AttenuationCoefficient somme = 0;
    AttenuationCoefficient attenuationCoefficient = 0;
    for (int i = 0; i < nbCoefficient; i++) {
        somme = somme + beaconCoefficients[i].attenuationCoefficient;
    }
    attenuationCoefficient = somme / nbCoefficient;
    return attenuationCoefficient;
}


extern Position Mathematician_getCurrentPosition(const BeaconData * beaconsData,  uint8_t nbBeacon) {
    Position M;
    M.X = 3;
    M.Y = 3;
    Position A = beaconsData[0].position;
    double Ax=(double)beaconsData[0].position.X;
    double Ay=(double)beaconsData[0].position.Y;
    double dA = distanceCalculWithPower(&beaconsData[0].power, &beaconsData[0].coefficientAverage);
    //double dA = distanceCalculWithPosition(&M, &A);
    printf("distance avec A :%f\n", dA);

    Position B = beaconsData[1].position;
    double dB = distanceCalculWithPower(&beaconsData[1].power, &beaconsData[1].coefficientAverage);
    //double dB = distanceCalculWithPosition(&M, &B);
    printf("distance avec B :%f\n", dB);
    double Bx=(double)beaconsData[1].position.X;
    double By=(double)beaconsData[1].position.Y;  

    Position C = beaconsData[2].position;
    double dC = distanceCalculWithPower(&beaconsData[2].power, &beaconsData[2].coefficientAverage);
    //double dC = distanceCalculWithPosition(&M, &C);
    printf("distance avec C :%f\n", dC);
    double Cx=(double)beaconsData[2].position.X;
    double Cy=(double)beaconsData[2].position.Y;
    
    Position currentPosition;
    double a=0;
    double b;
    double c;
    double d;

//considérons que uniquement deux balises max peuvent etre sur le meme Y, jamais 3
    if (A.Y == B.Y) { // si A et B meme ordonnee
        printf("dans le cas ou a=b\n");
        a = (Cx - Ax) / (Ay - Cy);
        printf("a=%f\n", a);
        b = (Ax * Ax + Ay * Ay - dA * dA - Cx * Cx - Cy * Cy + dC * dC) / (2 * (Ay - Cy));

        c = (Bx - Cx) / (Cy - By);
        d = (Cx * Cx + Cy * Cy - dC * dC - Bx * Bx - By * By + dB * dB) / (2 * (Cy - By));
        printf("b=%f\n", b);
        printf("c=%f\n", c);
        printf("d=%f\n", d);


    } else if (A.Y == C.Y) { // si A et C meme ordonnee
        printf("dans le cas ou a=c\n");

        a = (Bx - Ax) / (Ay - By);
        b = (Ax * Ax + Ay * Ay - dA * dA - Bx * Bx - By * By + dB * dB) / (2 * (Ay - By));

        c = (Bx - Cx) / (Cy - By);
        d = (Cx * Cx + Cy * Cy - dC * dC - Bx * Bx - By * By + dB * dB) / (2 * (Cy - By));
        printf("a=%f\n", a);
        printf("b=%f\n", b);
        printf("c=%f\n", c);
        printf("d=%f\n", d);

    } else if (B.Y == C.Y) { // si C et B meme ordonnee
        printf("dans le cas ou c=b\n");

        a = (Bx - Ax) / (Ay - By);
        b = (Ax * Ax + Ay * Ay - dA * dA - Bx * Bx - By * By + dB * dB) / (2 * (Ay - By));

        c = (Cx - Ax) / (Ay - Cy);
        d = (Ax * Ax + Ay * Ay - dA * dA - Cx * Cx - Cy * Cy + dC * dC) / (2 * (Ay - Cy));
        printf("a=%f\n", a);
        printf("b=%f\n", b);
        printf("c=%f\n", c);
        printf("d=%f\n", d);
    }
    double x = (d - b) / (a - c);
    double y = a*x+b;
    printf("x : %f\n",x);
    printf("y : %f\n",y);
    currentPosition.X = (uint32_t) x;
    currentPosition.Y = (uint32_t) y;
    return currentPosition;
}
