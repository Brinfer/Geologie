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

#define NUMBER_OF_BEACONS
#define SIZE_BEACON_ID (3)
#define NB_CALIBRATION_POSITION (3)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *
 * @brief création d'un tableau constant public .
 *
 */
extern float tab[];

/**
 * @struct type
 * @brief création d'une structure Position qui va prendre les coordonnees d'un point
 */
typedef struct {
    int X;
    int Y;
}Position;

typedef float Power;

typedef float AttenuationCoefficient;

typedef float AttenuationCoefficientAverage;

typedef float BeaconCoefficients[NUMBER_OF_BEACONS];

/**
 * @struct type
 * @brief création d'une structure qui va prendre les données d'une balise
 */
typedef struct {
    char ID[SIZE_BEACON_ID];
    Position position;
    Power power;
    AttenuationCoefficientAverage attenuationCoefficient;
    unsigned char nbCoefficientAttenuations;
    AttenuationCoefficient attenuationCoefficientsArray[NB_CALIBRATION_POSITION];
} BeaconData;


/**
 * @struct Example
 * @brief objet chaine de caractère
 * Example est un objet de gestion de chaine de caractère et on connait
 * la taille de cette chaine de caractère
 */
typedef struct {
  char exampleChar;
  unsigned long len;
} Example;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn extern getAttenuationCoefficient(Power power, Position position, Position calibrationPosition)
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
extern AttenuationCoefficient getAttenuationCoefficient(Power power, Position beaconPosition, Position calibrationPosition);

/**
* @fn extern AttenuationCoefficientAverage getAverageCalcul(BeaconCoefficients beaconCoefficients)
* @brief calcul la moyenne des coefficients d'attenuations
*
* @param [in] beaconCoefficients tableau contenant les coefficients d'attenuations pour une balise
* @return la moyenne des coefficients d'attenuations
*/
extern AttenuationCoefficientAverage getAverageCalcul(BeaconCoefficients beaconCoefficients);


/**
* @fn extern AttenuationCoefficientAverage getAverageCalcul(BeaconCoefficients beaconCoefficients)
* @brief calcul la moyenne des coefficients d'attenuations
*
* @param [in] beaconCoefficients tableau contenant les coefficients d'attenuations pour une balise
* @return la moyenne des coefficients d'attenuations
*/
extern AttenuationCoefficientAverage getAverageCalcul(BeaconCoefficients beaconCoefficients);

/**
* @fn extern AttenuationCoefficientAverage getAverageCalcul(BeaconCoefficients beaconCoefficients)
* @brief calcul la moyenne des coefficients d'attenuations
*
* @param [in] beaconsData tableau contenant les informations des balises
* @return la position actuelle de la carte
*/
extern Position getCurrentPosition(BeaconData beaconsData[]);

#endif /* EXAMPLE_H */
