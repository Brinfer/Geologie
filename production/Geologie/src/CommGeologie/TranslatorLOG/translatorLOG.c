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
#include "../../tools.h"

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
 * @brief La taille en octet de l'identifiant d'une position de calibration
 */
#define SIZE_CALIBRATION_POSITION_ID (1)

/**
 * @brief La taille en octet de l'identifiant d'une position d'experimentation
 */
#define SIZE_EXPERIMENTAL_POSITION_ID (1)

/**
 * @brief La taille en octet de l'identifiant d'un trajet d'experimentation
 */
#define SIZE_EXPERIMENTAL_TRAJECT_ID (1)

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

/**
 * @brief La taille en octet des informations d'une donnee de calibration a une position de calibration.
 */
#define SIZE_CALIBRATION_POSITION_DATA (SIZE_CALIBRATION_POSITION_ID + SIZE_ATTENUATION_COEFFICIENT)

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
static void composeHeader(Commande cmd, uint8_t nbElements, Trame* dest);

/**
 * @brief Compose le header pour le commande #SEND_EXPERIMENTAL_TRAJETS en fonction du
 * nombre du nombre de trajet experimentaux et du nombre de position dans le trajet experimental.
 *
 * Compose le header et le met dans @a dest.
 *
 * @param experimentalTrajects Le tableaux de trajet experimentaux
 * @param nbExperimentalTraject Le nombre de trajet experimentaux
 * @param dest Le tableau d'octet ou mettre la composition.
 *
 * @warning @dest doit une taille superieur ou egale a #SIZE_HEADER
 */
static void composeHeaderExperimentalTraject(const ExperimentalTraject* experimentalTrajects, uint8_t nbExperimentalTraject, Trame* dest);

/**
 * @brief Compose le header pour le commande #SEND_CALIBRATION_DATA en fonction du
 * nombre de donnee de calibration et du nombre de position dans le trajet experimental.
 *
 * Compose le header et le met dans @a dest.
 *
 * @param calibrationData Le tableaux de donnees de calibration.
 * @param nbCalibrationData Le nombre de donnees de calibration.
 * @param dest Le tableau d'octet ou mettre la composition.
 *
 * @warning @dest doit une taille superieur ou egale a #SIZE_HEADER
 */
static void composeHeaderCalibrationData(const CalibrationData* calibrationData, uint8_t nbCalibrationData, Trame* dest);

/**
 * @brief Convertie le tableau d'octet en un uint16_t.
 *
 * @param bytes Le tableau d'octet a convertir.
 * @return uint16_t La valeur de la conversion.
 *
 * @warning @a bytes doit avoir une taille superieur ou egale a deux.
 */
static uint16_t convertBytesToUint16_t(const Trame* bytes);

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
static void convertUint16_tToBytes(uint16_t value, Trame* dest);

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
static void convertUint32_tToBytes(uint32_t value, Trame* dest);

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
static void convertFloatToByte(float value, Trame* dest);

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
static void convertPositionToByte(const Position* position, Trame* dest);

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
static void convertBeaconDataToByte(const BeaconData* beaconData, Trame* dest);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern uint16_t TranslatorLOG_getTrameSize(Commande cmd, uint8_t nbElements) {
    uint16_t returnValue = 0;

    switch (cmd) {
        default:
        case ASK_CALIBRATION_POSITIONS:
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
            returnValue = SIZE_HEADER + SIZE_POSITION + SIZE_TIMESTAMP;
            break;
        case REP_CALIBRATION_POSITIONS:
            returnValue = SIZE_HEADER + 1 + nbElements * SIZE_CALIBRATION_POSITION;
            break;
        case SIGNAL_CALIBRATION_POSITION:
            returnValue = SIZE_HEADER + SIZE_CALIBRATION_POSITION_ID;
            break;
        case SEND_CALIBRATION_DATA:
            // should use TranslatorLOG_getTrameSizeCalibrationData
            break;
        case SEND_EXPERIMENTAL_TRAJECTS:
            // should use TranslatorLOG_getTrameSizeExperimentalTraject
            break;
    }

    return returnValue;
}

extern uint16_t TranslatorLOG_getTrameSizeExperimentalTraject(const ExperimentalTraject* experimentalTrajects, uint8_t nbExperimentalTrajects) {
    uint16_t returnValue = SIZE_HEADER + 1;

    for (uint8_t i = 0; i < nbExperimentalTrajects; i++) {
        returnValue += SIZE_EXPERIMENTAL_TRAJECT_ID + 1 + (experimentalTrajects[i].nbPosition * SIZE_POSITION);
    }

    return returnValue;
}

extern uint16_t TranslatorLOG_getTrameSizeCalibrationData(const CalibrationData* calibrationsData, uint8_t nbCalibrationsData) {
    uint16_t returnValue = SIZE_HEADER + 1;

    for (uint8_t i = 0; i < nbCalibrationsData; i++) {
        returnValue += SIZE_BEACON_ID + SIZE_ATTENUATION_COEFFICIENT + 1 + (calibrationsData[i].nbCoefficient * SIZE_CALIBRATION_POSITION_DATA);
    }

    return returnValue;
}

extern void TranslatorLOG_translateTrameToHeader(const Trame* trame, Header* dest) {
    /* CMD */
    dest->commande = trame[0];

    /* SIZE */
    dest->size = convertBytesToUint16_t(trame + 1);
}

extern void TranslatorLOG_translateForSendExperimentalPositions(uint8_t nbExperimentalPositions, const ExperimentalPosition* experimentalPositions, Trame* dest) {
    /* Header */
    composeHeader(SEND_EXPERIMENTAL_POSITIONS, nbExperimentalPositions, dest);

    /* Number of experimental position */
    dest[SIZE_HEADER] = nbExperimentalPositions;

    /* Experimental position */
    for (uint8_t i = 0; i < nbExperimentalPositions; i++) {
        dest[SIZE_HEADER + 1 + (SIZE_EXPERIMENTAL_POSITION * i)] = experimentalPositions[i].id;
        convertPositionToByte(&(experimentalPositions[i].position), &(dest[SIZE_HEADER + 2 + (i * SIZE_EXPERIMENTAL_POSITION)]));
    }
}

extern void TranslatorLOG_translateForSendAllBeaconsData(uint8_t nbBeacons, const BeaconData* beaconsData, Date currentDate, Trame* dest) {
     /* Header */
    composeHeader(SEND_ALL_BEACONS_DATA, nbBeacons, dest);

    /* Number of beacon */
    dest[SIZE_HEADER] = nbBeacons;

    /* TimeStamp */
    convertUint32_tToBytes(currentDate, dest + SIZE_HEADER + 1);

    /* Current position */
    for (uint8_t i = 0; i < nbBeacons; i++) {
        convertBeaconDataToByte(&(beaconsData[i]), dest + SIZE_HEADER + 1 + SIZE_TIMESTAMP + (i * SIZE_BEACON_DATA));
    }
}

extern void TranslatorLOG_translateForSendCurrentPosition(const Position* currentPosition, Date currentDate, Trame* dest) {
     /* Header */
    composeHeader(SEND_CURRENT_POSITION, 0, dest);

    /* TimeStamp */
    convertUint32_tToBytes(currentDate, dest + SIZE_HEADER);

    /* Current position */
    convertPositionToByte(currentPosition, dest + SIZE_HEADER + SIZE_TIMESTAMP);
}

extern void TranslatorLOG_translateForRepCalibrationPosition(uint8_t nbCalibrationPositions, const CalibrationPosition* calibrationPositions, Trame* dest) {
     /* Header */
    composeHeader(REP_CALIBRATION_POSITIONS, nbCalibrationPositions, dest);

    /* Number of calibration position */
    dest[SIZE_HEADER] = nbCalibrationPositions;

    /* calibration position */
    for (uint8_t i = 0; i < nbCalibrationPositions; i++) {
        dest[SIZE_HEADER + 1 + (SIZE_CALIBRATION_POSITION * i)] = calibrationPositions[i].id;
        convertPositionToByte(&(calibrationPositions[i].position), &(dest[SIZE_HEADER + 2 + (i * SIZE_CALIBRATION_POSITION)]));
    }
}

extern void TranslatorLOG_translateForSendMemoryAndProcessorLoad(const ProcessorAndMemoryLoad* processorAndMemoryLoad, Date currentDate, Trame* dest) {
     /* Header */
    composeHeader(SEND_MEMORY_PROCESSOR_LOAD, 0, dest);

    /* TimeStamp */
    convertUint32_tToBytes(currentDate, dest + SIZE_HEADER);

    /* Memory Load */
    convertFloatToByte(processorAndMemoryLoad->memoryLoad, dest + SIZE_HEADER + SIZE_TIMESTAMP);

    /* Processort Load */
    convertFloatToByte(processorAndMemoryLoad->processorLoad, dest + SIZE_HEADER + SIZE_TIMESTAMP + SIZE_MEMORY_LOAD);
}

extern CalibrationPositionId TranslatorLOG_translateForSignalCalibrationPosition(const Trame* trame) {
    return trame[SIZE_HEADER];
}

extern void TranslatorLOG_translateForSendExperimentalTrajects(const ExperimentalTraject* experimentalTrajects, uint8_t nbTraject, Trame* dest) {
    composeHeaderExperimentalTraject(experimentalTrajects, nbTraject, dest);
    uint16_t previousSize = SIZE_HEADER;

    dest[previousSize] = nbTraject;
    previousSize++;


    for (uint8_t i = 0; i < nbTraject; i++) {
        dest[previousSize] = experimentalTrajects[i].id;
        previousSize += SIZE_EXPERIMENTAL_TRAJECT_ID;

        dest[previousSize] = experimentalTrajects[i].nbPosition;
        previousSize++;

        for (uint8_t j = 0; j < experimentalTrajects[i].nbPosition; j++) {
            convertPositionToByte(&(experimentalTrajects[i].traject[j]), &(dest[previousSize]));
            previousSize += SIZE_POSITION;
        }
    }
}

extern void TranslatorLOG_translateForSignalCalibrationEnd(Trame* dest) {
    composeHeader(SIGNAL_CALIRATION_END, 0, dest);
}

extern void TranslatorLOG_translateForSignalCalibrationEndPosition(Trame* dest) {
    composeHeader(SIGNAL_CALIBRATION_END_POSITION, 0, dest);
}

extern void TranslatorLOG_translateForSendCalibrationData(const CalibrationData* calibrationsData, uint8_t nbCalibrationData, Trame* dest) {
    composeHeaderCalibrationData(calibrationsData, nbCalibrationData, dest);
    uint8_t sizeBeaconCalibrationDataPrevious = SIZE_HEADER;

    dest[sizeBeaconCalibrationDataPrevious] = nbCalibrationData;
    sizeBeaconCalibrationDataPrevious += 1;

    for (uint8_t i = 0; i < nbCalibrationData; i++) {
        uint8_t sizeBeaconCalibrationDataCurrent = 0;

        memcpy(&(dest[sizeBeaconCalibrationDataPrevious]), &(calibrationsData[i].beaconId), SIZE_BEACON_ID);
        sizeBeaconCalibrationDataCurrent += SIZE_BEACON_ID;

        convertFloatToByte(calibrationsData[i].coefficientAverage, &(dest[sizeBeaconCalibrationDataPrevious + sizeBeaconCalibrationDataCurrent]));
        sizeBeaconCalibrationDataCurrent += 4;

        dest[sizeBeaconCalibrationDataPrevious + sizeBeaconCalibrationDataCurrent] = calibrationsData[i].nbCoefficient;
        sizeBeaconCalibrationDataCurrent += 1;

        for (uint8_t j = 0; j < calibrationsData[i].nbCoefficient; j++) {
            dest[sizeBeaconCalibrationDataPrevious + sizeBeaconCalibrationDataCurrent] = calibrationsData[i].beaconCoefficient[j].positionId;
            sizeBeaconCalibrationDataCurrent += SIZE_CALIBRATION_POSITION_ID;

            convertFloatToByte(calibrationsData[i].beaconCoefficient[j].attenuationCoefficient, &(dest[sizeBeaconCalibrationDataPrevious + sizeBeaconCalibrationDataCurrent]));
            sizeBeaconCalibrationDataCurrent += SIZE_ATTENUATION_COEFFICIENT;
        }

        sizeBeaconCalibrationDataPrevious += sizeBeaconCalibrationDataCurrent;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void composeHeader(Commande cmd, uint8_t nbElements, Trame* dest) {
    uint16_t size = TranslatorLOG_getTrameSize(cmd, nbElements) - SIZE_HEADER;

    /* CMD */
    dest[0] = cmd;

    /* SIZE */
    convertUint16_tToBytes(size, dest + 1);
}

static void composeHeaderExperimentalTraject(const ExperimentalTraject* experimentalTrajects, uint8_t nbExperimentalTraject, Trame* dest) {
    uint16_t size = TranslatorLOG_getTrameSizeExperimentalTraject(experimentalTrajects, nbExperimentalTraject) - SIZE_HEADER;

    /* CMD */
    dest[0] = SEND_EXPERIMENTAL_TRAJECTS;

    /* SIZE */
    convertUint16_tToBytes(size, dest + 1);
}

static void composeHeaderCalibrationData(const CalibrationData* calibrationData, uint8_t nbCalibrationData, Trame* dest) {
    uint16_t size = TranslatorLOG_getTrameSizeCalibrationData(calibrationData, nbCalibrationData) - SIZE_HEADER;

    /* CMD */
    dest[0] = SEND_CALIBRATION_DATA;

    /* SIZE */
    convertUint16_tToBytes(size, dest + 1);
}

static uint16_t convertBytesToUint16_t(const Trame* bytes) {
    return ntohs(*(uint16_t*) bytes);
}

static void convertUint16_tToBytes(uint16_t value, Trame* dest) {
    uint16_t bigEndian = htons(value);

    for (int8_t i = 1; i >= 0; --i) {
        dest[i] = (bigEndian >> (8 * i));
    }
}

static void convertUint32_tToBytes(uint32_t value, Trame* dest) {
    uint32_t bigEndian = htonl(value);

    for (int8_t i = 3; i >= 0; --i) {
        dest[i] = (bigEndian >> (8 * i));
    }
}

static void convertFloatToByte(float value, Trame* dest) {
    convertUint32_tToBytes(*((uint32_t*) &value), dest); // Is cast in uint32_t
}

static void convertPositionToByte(const Position* position, Trame* dest) {
    convertUint32_tToBytes(position->X, dest);
    convertUint32_tToBytes(position->Y, dest + (SIZE_POSITION / 2));
}

static void convertBeaconDataToByte(const BeaconData* beaconData, Trame* dest) {
    /* ID */
    memcpy(dest, beaconData->ID, SIZE_BEACON_ID);

    /* Position */
    convertPositionToByte(&(beaconData->position), dest + SIZE_BEACON_ID);

    /* Power */
    convertFloatToByte(beaconData->power, dest + SIZE_BEACON_ID + SIZE_POSITION);

    /* Attenuation Coefficient */
    convertFloatToByte(beaconData->coefficientAverage, dest + SIZE_BEACON_ID + SIZE_POSITION + SIZE_POWER);
}
