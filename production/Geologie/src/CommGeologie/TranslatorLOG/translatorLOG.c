/**
 * @file translatorLOG.c
 *
 * @brief Traduit une trame reçus ou construit le message a partie de donnees.
 *
 * @version 2.0
 * @date 05-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "translatorLOG.h"

#include <stdint.h>
#include <arpa/inet.h>

#include "../com_common.h"
#include "../../common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief La taille en octet d'une position.
 */
#define SIZE_POSITION (8)

/**
 * @brief La taille en octet de la date.
 * */
#define SIZE_TIMESTAMP (4)

/**
 * @brief La taille en octet de la charge memoire.
 */
#define SIZE_MEMORY_LOAD (4)

/**
 * @brief La taille en octet de la charge processeur.
 */
#define SIZE_PROCESSOR_LOAD (4)

/**
 * @brief La taille en octet de la puissance d'un signal.
 */
#define SIZE_POWER (4)

/**
 * @brief La taille en octet d'un coefficient d'attenuation.
 */
#define SIZE_ATTENUATION_COEFFICIENT (4)

/**
 * @brief La taille en octet de l'indentifiant d'une balise.
 */
#define SIZE_BEACON_ID (3)

/**
 * @brief La taille en octet de l'identifiant d'une position de calibration
 */
#define SIZE_CALIBRATION_POSITION_ID (1)

/**
 * @brief La taille en octet de l'identifiant d'une position d'experimentation
 */
#define SIZE_EXPERIMENTAL_POSITION_ID (1)

/**
 * @brief La taille en octet des informations d'une position experimental.
 */
#define SIZE_EXPERIMENTAL_POSITION (SIZE_EXPERIMENTAL_POSITION_ID + SIZE_POSITION)

/**
 * @brief La taille en octet des informations d'une balise.
 */
#define SIZE_BEACON_DATA (SIZE_BEACON_ID + SIZE_POSITION + SIZE_POWER + SIZE_ATTENUATION_COEFFICIENT)

/**
 * @brief La taille en octet des informations sur la charge memoire et processeur.
 */
#define SIZE_MEMORY_AND_PROCESSOR_LOAD (SIZE_MEMORY_LOAD + SIZE_PROCESSOR_LOAD)

/**
 * @brief La taille en octet des informations d'une position de calibration.
 */
#define SIZE_CALIBRATION_POSITION (SIZE_CALIBRATION_POSITION_ID + SIZE_POSITION)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Convertie le tableau d'octet en un uint16_t.
 *
 * @param bytes Le tableau d'octet a convertir.
 * @return uint16_t La valeur de la conversion.
 *
 * @warning @a bytes doit avoir une taille superieur ou egale a deux.
 */
static uint16_t convertBytesToUint16_t(const Trame bytes);

/**
 * @brief Convertie un uint32_t en un tableau d'octet.
 *
 * Convertie @a value et met la conversion dans @a dest.
 *
 * @param value La valeur a convertire.
 * @param dest Le tableau d'octet ou mettre la conversion
 *
 * @warning @a dest doit avoir une taille superieur ou egale a quatre.
 */
static void convertUint32_tToBytes(uint32_t value, Trame dest);

/**
 * @brief Convertie une #Position en un tableau d'octet.
 *
 * Convertie @a position et met la conversion dans @a dest.
 *
 * @param position La #Position a convertire.
 * @param dest Le tableau d'octet ou mettre la conversion
 *
 * @warning @a dest doit avoir une taille superieur ou egale a #SIZE_POSITION.
 */
static void convertPositionToByte(const Position* position, Trame dest);

/**
 * @brief Convertie un #BeaconData en un tableau d'octet.
 *
 * Convertie @a beaconData et met la conversion dans @a dest.
 *
 * @param beaconData Le #BeaconData a convertire.
 * @param dest Le tableau d'octet ou mettre la conversion
 *
 * @warning @a dest doit avoir une taille superieur ou egale a #SIZE_BEACON_DATA.
 */
static void convertBeaconDataToByte(const BeaconData* beaconData, Trame dest);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern uint16_t TranslatorLog_getTrameSize(Commande cmd, uint8_t nbElements) {
    uint16_t returnValue = 0;

    switch (cmd) {
        default:
        case ASK_CALIBRATION_POSITIONS:
        case SIGNAL_CALIBRATION_START:
        case SIGNAL_CALIRATION_END:
            returnValue = SIZE_HEADER + 0;
            break;
        case SEND_EXPERIMENTAL_POSITIONS:
            returnValue = SIZE_HEADER + 1 + nbElements * SIZE_EXPERIMENTAL_POSITION;
            break;
        case SEND_MEMORY_PROCESSOR_LOAD:
            returnValue = SIZE_HEADER + SIZE_TIMESTAMP + SIZE_MEMORY_AND_PROCESSOR_LOAD;
            break;
        case SEND_ALL_BEACONS_DATA:
            returnValue = SIZE_HEADER + 1 + SIZE_TIMESTAMP + nbElements * SIZE_BEACON_DATA;
            break;
        case SEND_CURRENT_POSITION:
            returnValue = SIZE_HEADER + SIZE_POSITION;
            break;
        case REP_CALIBRATION_POSITIONS:
            returnValue = SIZE_HEADER + 1 + nbElements * SIZE_CALIBRATION_POSITION;
            break;
        case SIGNAL_CALIBRATION_POSITION:
            returnValue = SIZE_HEADER + SIZE_CALIBRATION_POSITION_ID;
            break;
        case SEND_EXPERIMENTAL_TRAJECTS:
            returnValue = -1; // SIZE_HEADER +  // TODO
            break;
    }

    return returnValue;
}

extern void TranslatorLog_translateTrameToHeader(const Trame trame, Header* dest) {
    /* CMD */
    dest->commande = trame[0];

    /* SIZE */
    dest->size = convertBytesToUint16_t(trame + 1);
}

extern void TranslatorLog_translateForSendExperimentalPosition(uint8_t nbExperimentalPositions, const ExperimentalPosition* experimentalPositions, Trame dest) {
    /* Number of experimental position */
    dest[0] = nbExperimentalPositions;

    /* Experimental position */
    for (uint8_t i = 0; i < nbExperimentalPositions; i++) {
        dest[1 + (SIZE_EXPERIMENTAL_POSITION * i)] = experimentalPositions[i].id;
        convertPositionToByte(&(experimentalPositions[i].position), &(dest[2 + (i * SIZE_EXPERIMENTAL_POSITION)]));
    }
}

extern void TranslatorLog_translateForSendAllBeaconsData(uint8_t nbBeacons, const BeaconData* beaconsData, Date currentDate, Trame dest) {
    /* Number of beacon */
    dest[0] = nbBeacons;

    /* TimeStamp */
    convertUint32_tToBytes(currentDate, dest + 1);

    /* Current position */
    for(uint8_t i = 0; i < nbBeacons; i++) {
        convertBeaconDataToByte(&(beaconsData[i]), dest + 1 + SIZE_TIMESTAMP + (i * SIZE_BEACON_DATA));
    }
}

extern void TranslatorLog_translateForSendCurrentPosition(const Position* currentPosition, Date currentDate, Trame dest) {
    /* TimeStamp */
    convertUint32_tToBytes(currentDate, dest);

    /* Current position */
    convertPositionToByte(currentPosition, dest + SIZE_TIMESTAMP);
}

extern void TranslatorLog_translateForRepCalibrationPosition(uint8_t nbCalibrationPositions, const CalibrationPosition* calibrationPositions, Trame dest) {
    /* Number of calibration position */
    dest[0] = nbCalibrationPositions;

    /* calibration position */
    for (uint8_t i = 0; i < nbCalibrationPositions; i++) {
        dest[1 + (SIZE_CALIBRATION_POSITION * i)] = calibrationPositions[i].id;
        convertPositionToByte(&(calibrationPositions[i].position), &(dest[2 + (i * SIZE_CALIBRATION_POSITION)]));
    }
}

extern void TranslatorLog_translateForSendMemoryAndProcessorLoad(const ProcessorAndMemoryLoad* processorAndMemoryLoad, Date currentDate, Trame dest) {
    /* TimeStamp */
    convertUint32_tToBytes(currentDate, dest);

    /* Memory Load */
    convertPositionToByte(&(processorAndMemoryLoad->memoryLoad), dest + SIZE_TIMESTAMP);

    /* Processort Load */
    convertPositionToByte(&(processorAndMemoryLoad->processorLoad), dest + SIZE_TIMESTAMP + SIZE_MEMORY_LOAD);
}

extern CalibrationPositionId TranslatorLog_translateForSignalCalibrationPosition(const Trame trame) {
    return trame[0];
}

extern void translateForExperimentalTrajects(uint8_t nbTraject, const ExperimentalTraject* experimentalTrajects, Trame dest) {
    // TODO
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint16_t convertBytesToUint16_t(const Trame bytes) {
    return ntohs(*(uint16_t*) bytes);
}

static float convertBytesToFloat(const Trame bytes) {
    int32_t temp;                       // ntohl accept uint32_t not the float
    temp = ntohl(*(int32_t*) bytes);
    return *(float*) &temp;
}

static void convertUint32_tToBytes(uint32_t value, Trame dest) {
    value = htonl(value);

    for (uint8_t i = 0; i < 4; i++) {
        bytes[i] = (value >> (24 - (8 * i))) & 0xFF;
    }
}

static void convertPositionToByte(const Position* position, Trame dest) {
    convertUint32_tToBytes(position->X, dest);
    convertUint32_tToBytes(position->Y, dest + (SIZE_POSITION % 2));
}

static void convertBeaconDataToByte(const BeaconData* beaconData, Trame dest) {
    /* ID */
    dest[0] = beaconData->ID;

    /* Position */
    convertPositionToByte(&(beaconData->position), dest + 1);

    /* Power */
    convertUint32_tToBytes(beaconData->power, dest + 1 + SIZE_POSITION);

    /* Attenuation Coefficient */
    convertUint32_tToBytes(beaconData->attenuationCoefficient, dest + 1 + SIZE_POSITION + SIZE_POWER);
}
