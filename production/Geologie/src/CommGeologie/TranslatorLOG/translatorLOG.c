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

#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>

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
 * @brief La taille en octet de l'identifiant d'une balise.
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
 * @brief Compose le header en fonction de la commande et du
 * nombre d'element qui la compose si celle-ci est de taille variable.
 *
 * Dans le cas ou la commande n'est pas de taille variable, l'argument @a nbElements est ignore.
 *
 * @a nbElements peut correspondre au pare exemple nombre de #BeaconData a envoyer pour le message #SEND_ALL_BEACONS_DATA, ou
 * au nombre de position de calibration du message #REP_CALIBRATION_POSITIONS.
 *
 * Compose le header et le met dans @a dest.
 *
 * @param cmd La commande dont on veut connaitre la taille de la trame.
 * @param nbElements Le nombre d'element composant le message.
 * @param dest Le tableau d'octet ou mettre la composition.
 *
 * @warning @dest doit une taille superieur ou egale a #SIZE_HEADER
 */
static void composeHeader(Commande cmd, uint8_t nbElements, Trame dest);

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
 * @brief Convertie un uint16_t en un tableau d'octet.
 *
 * Convertie @a value et met la conversion dans @a dest.
 *
 * @param value La valeur a convertire.
 * @param dest Le tableau d'octet ou mettre la conversion
 *
 * @warning @a dest doit avoir une taille superieur ou egale a deux.
 */
static void convertUint16_tToBytes(uint16_t value, Trame dest);

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
 * @brief Convertie un float en un tableau d'octet.
 *
 * Convertie @a value et met la conversion dans @a dest.
 *
 * @param value La valeur a convertire.
 * @param dest Le tableau d'octet ou mettre la conversion
 *
 * @warning @a dest doit avoir une taille superieur ou egale a quatre.
 */
static void convertFloatToByte(float value, Trame dest);

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

extern void TranslatorLog_translateTrameToHeader(const Trame * trame, Header* dest) {
    /* CMD */
    dest->commande = trame[0];

    /* SIZE */
    dest->size = convertBytesToUint16_t(trame + 1);
}

extern void TranslatorLog_translateForSendExperimentalPositions(uint8_t nbExperimentalPositions, const ExperimentalPosition* experimentalPositions, Trame dest) {
    /* Header */
    composeHeader(SEND_EXPERIMENTAL_POSITIONS, nbExperimentalPositions, dest);

    /* Number of experimental position */
    dest[SIZE_HEADER] = nbExperimentalPositions;

    /* Experimental position */
    for (uint8_t i = 0; i < nbExperimentalPositions; i++) {
        dest[1 + (SIZE_EXPERIMENTAL_POSITION * i)] = experimentalPositions[SIZE_HEADER + i].id;
        convertPositionToByte(&(experimentalPositions[i].position), &(dest[SIZE_HEADER + 1 + (i * SIZE_EXPERIMENTAL_POSITION)]));
    }
}

extern void TranslatorLog_translateForSendAllBeaconsData(uint8_t nbBeacons, const BeaconData* beaconsData, Date currentDate, Trame dest) {
     /* Header */
    composeHeader(SEND_ALL_BEACONS_DATA, nbBeacons, dest);

    /* Number of beacon */
    dest[SIZE_HEADER] = nbBeacons;

    /* TimeStamp */
    convertUint32_tToBytes(currentDate, dest + 1);

    /* Current position */
    for (uint8_t i = 0; i < nbBeacons; i++) {
        convertBeaconDataToByte(&(beaconsData[i]), dest + SIZE_HEADER + 1 + SIZE_TIMESTAMP + (i * SIZE_BEACON_DATA));
    }
}

extern void TranslatorLog_translateForSendCurrentPosition(const Position* currentPosition, Date currentDate, Trame dest) {
     /* Header */
    composeHeader(SEND_CURRENT_POSITION, 0, dest);

    /* TimeStamp */
    convertUint32_tToBytes(currentDate, dest + SIZE_HEADER);

    /* Current position */
    convertPositionToByte(currentPosition, dest + SIZE_HEADER + SIZE_TIMESTAMP);
}

extern void TranslatorLog_translateForRepCalibrationPosition(uint8_t nbCalibrationPositions, const CalibrationPosition* calibrationPositions, Trame dest) {
     /* Header */
    composeHeader(REP_CALIBRATION_POSITIONS, nbCalibrationPositions, dest);

    /* Number of calibration position */
    dest[SIZE_HEADER] = nbCalibrationPositions;

    /* calibration position */
    for (uint8_t i = 0; i < nbCalibrationPositions; i++) {
        dest[1 + (SIZE_CALIBRATION_POSITION * i)] = calibrationPositions[i].id;
        convertPositionToByte(&(calibrationPositions[i].position), &(dest[SIZE_HEADER + 2 + (i * SIZE_CALIBRATION_POSITION)]));
    }
}

extern void TranslatorLog_translateForSendMemoryAndProcessorLoad(const ProcessorAndMemoryLoad* processorAndMemoryLoad, Date currentDate, Trame dest) {
     /* Header */
    composeHeader(SEND_MEMORY_PROCESSOR_LOAD, 0, dest);

    /* TimeStamp */
    convertUint32_tToBytes(currentDate, dest + SIZE_HEADER);

    /* Memory Load */
    convertFloatToByte(processorAndMemoryLoad->memoryLoad, dest + SIZE_HEADER + SIZE_TIMESTAMP);

    /* Processort Load */
    convertFloatToByte(processorAndMemoryLoad->processorLoad, dest + SIZE_HEADER + SIZE_TIMESTAMP + SIZE_MEMORY_LOAD);
}

extern CalibrationPositionId TranslatorLog_translateForSignalCalibrationPosition(const Trame trame) {
    return trame[SIZE_HEADER];
}

extern void translateForExperimentalTrajects(uint8_t nbTraject, const ExperimentalTraject* experimentalTrajects, Trame dest) {
    // TODO
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void composeHeader(Commande cmd, uint8_t nbElements, Trame dest) {
    uint16_t size = TranslatorLog_getTrameSize(cmd, nbElements);

    /* CMD */
    dest[0] = cmd;

    /* SIZE */
    convertUint16_tToBytes(size, dest + 1);
}


static uint16_t convertBytesToUint16_t(const Trame bytes) {
    return ntohs(*(uint16_t*) bytes);
}

static void convertUint16_tToBytes(uint16_t value, Trame dest) {
    value = htons(value);

    for (uint8_t i = 0; i < 2; i++) {
        dest[i] = (value >> (8 - (8 * i))) & 0xFF;
    }
}

static void convertUint32_tToBytes(uint32_t value, Trame dest) {
    value = htonl(value);

    for (uint8_t i = 0; i < 4; i++) {
        dest[i] = (value >> (24 - (8 * i))) & 0xFF;
    }
}

static void convertFloatToByte(float value, Trame dest) {
    convertUint32_tToBytes(value, dest); // Is cast in uint32_t
}

static void convertPositionToByte(const Position* position, Trame dest) {
    convertUint32_tToBytes(position->X, dest);
    convertUint32_tToBytes(position->Y, dest + (SIZE_POSITION % 2));
}

static void convertBeaconDataToByte(const BeaconData* beaconData, Trame dest) {
    /* ID */
    memcpy(dest, beaconData->ID, SIZE_BEACON_ID);

    /* Position */
    convertPositionToByte(&(beaconData->position), dest + 1);

    /* Power */
    convertFloatToByte(beaconData->power, dest + 1 + SIZE_POSITION);

    /* Attenuation Coefficient */
    convertUint32_tToBytes(beaconData->attenuationCoefficient, dest + 1 + SIZE_POSITION + SIZE_POWER);
}
