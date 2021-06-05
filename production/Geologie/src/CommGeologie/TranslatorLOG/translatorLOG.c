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

#include "../com_common.h"
#include "../../common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SIZE_POSITION (8)
#define SIZE_TIMESTAMP (4)
#define SIZE_MEMORYLOAD (4)
#define SIZE_PROCESSOR_LOAD (4)
#define SIZE_BEACON_ID (3)
#define SIZE_POWER (4)
#define SIZE_ATTENUATION_COEFFICIENT (4)
#define SIZE_BEACON_DATA (SIZE_BEACON_ID + SIZE_POSITION + SIZE_POWER + SIZE_ATTENUATION_COEFFICIENT)
#define SIZE_CALIBRATION_POSITION_ID (1)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Convertie le tableau d'octet en un uint6_t.
 *
 * @param bytes Le tableau d'octet a convertir.
 * @return uint16_t La valeur de la conversion.
 *
 * @warning @a bytes doit avoir une taille superieur ou egale a deux.
 */
static uint16_t convertBytesToUint16_t(Trame bytes);

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
            returnValue = SIZE_HEADER + 1 + nbElements * SIZE_POSITION;
            break;
        case SEND_MEMORY_PROCESSOR_LOAD:
            returnValue = SIZE_HEADER + SIZE_TIMESTAMP + SIZE_MEMORYLOAD + SIZE_PROCESSOR_LOAD;
            break;
        case SEND_ALL_BEACONS_DATA:
            returnValue = SIZE_HEADER + 1 + SIZE_TIMESTAMP + nbElements * SIZE_BEACON_DATA;
            break;
        case SEND_CURRENT_POSITION:
            returnValue = SIZE_HEADER + SIZE_POSITION;
            break;
        case REP_CALIBRATION_POSITIONS:
            returnValue = SIZE_HEADER + 1 + nbElements * SIZE_POSITION;
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

extern void TranslatorLog_translate4SendExperimentalPosition(uint8_t nbPositions, const ExperimentalPosition* experimentalPosition, Trame dest) {
}


extern void TranslatorLog_translate4SendAllBeaconsData(uint8_t nbBeacons, const BeaconData* beaconsData, Date currentDate, Trame dest) {
}

extern void TranslatorLog_translate4SendCurrentPosition(const Position* currentPosition, Date currentDate, Trame dest) {
}

extern void TranslatorLog_translateRepCalibrationPosition(uint8_t nbCalibrationPosition, const CalibrationPosition* calibrationPosition, Trame dest) {
}

extern int16_t TranslatorLog_translateSendProcessorAndMemoryLoad(ProcessorAndMemoryLoad processorAndMemoryLoad, Date currentDate, Trame dest) {
}

extern CalibrationPositionId TranslatorLog_treanslate4SignalCalibrationPosition(const Trame trame) {
    return 0;
}

extern void translate4ExperimentalTrajects(uint8_t nbTraject, const ExperimentalTraject* experimentalTrajects, Trame dest) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint16_t convertBytesToUint16_t(Trame bytes) {
    uint16_t returnValue = 0;

    for(uint8_t i = 0; i < 2; i++) {
        returnValue += bytes[i] << (8 * (2 * i));
    }
    returnValue = ntohl(returnValue);
    return returnValue;
}
