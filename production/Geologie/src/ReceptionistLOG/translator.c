/**
 * @file translator.c
 *
 * @version 1.0
 * @date 5 mai 2021
 * @author GAUTIER Pierre-Louis
 * @copyright BSD 2-clauses
 *
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "translator.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SIZE_BEACON_DATA (19 * 8)
#define SIZE_POSITION_DATA (8 * 8)

#define COMMAND_POSITION_START (0)
#define COMMAND_SIZE (1)

/**
 * @brief Inverse l'ordre du tableau d'octet.
 *
 * Intervertie les elements du tableau @p dest.
 *
 * @param array Le pointeur vers le tableau a inverser.
 * @param size La taille du tableau.
 */
static void reverseArray(unsigned char* array, unsigned int size);

/**
 * @brief Convertie un entier en un tableau d'octet.
 *
 * Copie dans le tableau @p dest la valeur des octects de l'entier.
 *
 * @param i L'entier a convertire.
 * @param dest Le pointeur vers le tableau ou ecrire les octects.
 *
 * @warning Le tableau @p dest doit etre un tableau dont la taille fait au minimum 4
 * Aucune verification de la taille de celui-ci ne sera faite.
 */
static void convertIntToBytes(int i, unsigned char* dest);

/**
 * @brief Convertie un floattant en un tableau d'octet.
 *
 * Copie dans le tableau @p dest la valeur des octects du floattant.
 *
 * @param f Le floattant a convertire.
 * @param dest Le pointeur vers le tableau ou ecrire les octects.
 *
 * @warning Le tableau @p dest doit etre un tableau dont la taille fait au minimum 4
 * Aucune verification de la taille de celui-ci ne sera faite.
 */
static void convertFloatToBytes(float f, unsigned char* dest);

/**
 * @brief Convertie un tableau d'octet en un entier.
 *
 * @param src Le pointeur vers le tableau d'octet.
 * @return int Le nombre entier en base 10 des octets.
 */
static int convertBytesToInt(const unsigned char* src);

/**
 * @brief Convertie un tableau d'octet en un entier.
 *
 * @param src Le pointeur vers le tableau d'octet.
 * @return float Le nombre reel en base 10 des octets.
 */
static float convertBytesToFloat(const unsigned char* src);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void Translator_convertBeaconDataToByte(const BeaconData* beaconData, unsigned char* dest) {
    /* ID */
    memcpy(dest, beaconData->ID, 3);

    /* Position */
    Translator_convertPositionToByte(&(beaconData->position), dest + 3);

    /* Power */
    convertIntToBytes(beaconData->power, dest + 11);

    /* Attenuation coefficient */
    convertFloatToBytes(beaconData->attenuationCoefficient, dest + 15);

    /* NB Attenuation coefficient */
    memcpy(dest + 19, &(beaconData->nbCoefficientAttenuations), 1);

    /* Attenuation coefficients */
    for (unsigned int i = 0; i < beaconData->nbCoefficientAttenuations; i++) {
        convertFloatToBytes(beaconData->attenuationCoefficientsArray[i], dest + 20 + (i * 4));
    }
}

extern void Translator_convertPositionToByte(const Position* position, unsigned char* dest) {
    convertIntToBytes(position->X, dest);
    convertIntToBytes(position->Y, dest + 4);
}

extern void Translator_convertByteToPosition(const unsigned char* src, Position* dest) {
    dest->X = convertBytesToInt(src);
    dest->Y = convertBytesToInt(src + 4);
}

extern void Translator_convertByteToBeaconData(const unsigned char* src, BeaconData* dest) {
    /* ID */
    memcpy(dest->ID, src, 3);

    /* Position */
    Translator_convertByteToPosition(src + 3, &(dest->position));

    /* Power */
    dest->power = convertBytesToInt(src + 11);

    /* Attenuation coefficient */
    dest->attenuationCoefficient = convertBytesToFloat(src + 15);

    /* NB Attenuation coefficient */
    memcpy(&(dest->nbCoefficientAttenuations), src + 19, 1);

    /* Attenuation coefficients */
    for (unsigned int i = 0; i < dest->nbCoefficientAttenuations; i++) {
        dest->attenuationCoefficientsArray[i] = convertBytesToFloat(src + 20 + (i * 4));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void reverseArray(unsigned char* array, unsigned int size) {
    for (int i = 0; i < size / 2; i++) {
        char temp = array[i];
        array[i] = array[size - 1 - i];
        array[size - 1 - i] = temp;
    }
}

static void convertIntToBytes(int i, unsigned char* dest) {
    unsigned char tempArray[sizeof(int)];
    memcpy(tempArray, (unsigned char*) (&i), sizeof(int));
    reverseArray(tempArray, sizeof(int));
    memcpy(dest, tempArray, sizeof(int));
}

static void convertFloatToBytes(float f, unsigned char* dest) {
    unsigned char tempArray[sizeof(float)];
    memcpy(tempArray, (unsigned char*) (&f), sizeof(float));
    reverseArray(tempArray, sizeof(float));
    memcpy(dest, tempArray, sizeof(float));
}

static int convertBytesToInt(const unsigned char* src) {
    int returnVal;
    unsigned char tempArray[sizeof(int)];

    memcpy(tempArray, src, sizeof(int));
    reverseArray(tempArray, sizeof(int));

    returnVal = *(int*) tempArray;
    return returnVal;
}

static float convertBytesToFloat(const unsigned char* src) {
    float returnVal;
    unsigned char tempArray[sizeof(float)];

    memcpy(tempArray, src, sizeof(float));
    reverseArray(tempArray, sizeof(float));

    returnVal = *(float*) tempArray;
    return returnVal;
}
