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
#include "Geographer/geographer.h"

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

static void convertToMSB(unsigned char array[], unsigned int size);
static void convertIntToBytes(int i, unsigned char dest[]);
static void convertFloatToBytes(float f, unsigned char dest[]);
static void convertDoubleToBytes(double d, unsigned char dest[]);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int TRANSLATOR_convertBeaconData(const BeaconData* beaconData, unsigned char dest[32]) {
    int returnValue = EXIT_FAILURE;
    /* ID */
    memcpy(dest, beaconData->ID, 3);

    /* Position */
    TRANSLATOR_convertPosition(&(beaconData->position), dest + 3);

    /* Puissance */
    convertIntToBytes(beaconData->power, dest + 11);

    /* Attenuation coefficient */
    convertFloatToBytes(beaconData->attenuationCoefficient, dest + 15);

    /* NB Attenuation coefficient */
    memcpy(dest + 19, &(beaconData->nbCoefficientAttenuations), 1);

    /* Attenuation coefficients */
    for (unsigned int i; i < beaconData->nbCoefficientAttenuations; i++) {
        convertFloatToBytes(beaconData->attenuationCoefficients[i], dest + 20 + (i * 4));
    }

    return returnValue;
}

extern int TRANSLATOR_convertPosition(const Position* position, unsigned char dest[8]) {
    int returnValue = EXIT_FAILURE;
    convertIntToBytes(position->X, dest);
    convertIntToBytes(position->Y, dest + 4);
    return returnValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void convertToMSB(unsigned char array[], unsigned int size) {
    for (int i = 0; i < size / 2; i++) {
        char temp = array[i];
        array[i] = array[size - 1 - i];
        array[size - 1 - i] = temp;
    }
}

static void convertIntToBytes(int i, unsigned char dest[]) {
    unsigned char tempArray[sizeof(int)];
    memcpy(tempArray, (unsigned char*) (&i), sizeof(int));
    convertToMSB(tempArray, sizeof(int));
    memcpy(dest, tempArray, sizeof(int));
}

static void convertFloatToBytes(float f, unsigned char dest[]) {
    unsigned char tempArray[sizeof(float)];
    memcpy(tempArray, (unsigned char*) (&f), sizeof(float));
    convertToMSB(tempArray, sizeof(float));
    memcpy(dest, tempArray, sizeof(float));
}

static void convertDoubleToBytes(double d, unsigned char dest[]) {
    unsigned char tempArray[sizeof(double)];
    memcpy(tempArray, (unsigned char*) (&d), sizeof(double));
    convertToMSB(tempArray, sizeof(double));
    memcpy(dest, tempArray, sizeof(double));
}
