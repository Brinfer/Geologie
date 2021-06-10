/**
 * @file proxyLoggerMOB.h
 *
 * @brief Toutes les requetes qui doivent etre envoyees à LoggerMOB passent par cet objet.
 *
 * ProxyLoggerMOB utilise la classe TranslatorLOG pour traduire les messages avant de les envoyer, afin qu'ils
 * correspondent au protocole de communication.
 *
 * @version 2.0
 * @date 07-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "proxyLoggerMOB.h"

#include <stdint.h>
#include <stdlib.h>

#include "../com_common.h"
#include "../../common.h"
#include "../PostmanLOG/postmanLOG.h"
#include "../TranslatorLOG/translatorLOG.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int8_t ProxyLoggerMOB_new(void) {
    /* Nothing to do */
    return EXIT_SUCCESS;
}

extern int8_t ProxyLoggerMOB_start(void) {
    /* Nothing to do */
    return EXIT_SUCCESS;
}

extern int8_t ProxyLoggerMOB_stop(void) {
    /* Nothing to do */
    return EXIT_SUCCESS;
}

extern int8_t ProxyLoggerMOB_free(void) {
    /* Nothing to do */
    return EXIT_SUCCESS;
}

extern int8_t ProxyLoggerMOB_setExperimentalTrajects(const ExperimentalTraject* experimentalTrajects, unsigned short nbExperimentalTraject) {
    int8_t returnError;
    uint16_t size = TranslatorLOG_getTrameSizeExperimentalTraject(experimentalTrajects, nbExperimentalTraject);
    Trame* trame = calloc(1, size);

    TranslatorLOG_translateForSendExperimentalTrajects(experimentalTrajects, nbExperimentalTraject, trame);
    returnError = PostmanLOG_sendMsg(trame, size);

    return returnError;
}

extern int8_t ProxyLoggerMOB_setExperimentalPositions(const ExperimentalPosition* experimentalPositions, unsigned short nbExperimentalPosition) {
    int8_t returnError;
    uint16_t size = TranslatorLOG_getTrameSize(SEND_EXPERIMENTAL_POSITIONS, nbExperimentalPosition);
    Trame* trame = calloc(1, size);

    TranslatorLOG_translateForSendExperimentalPositions(nbExperimentalPosition, experimentalPositions, trame);
    returnError = PostmanLOG_sendMsg(trame, size);

    return returnError;
}

extern int8_t ProxyLoggerMOB_setCalibrationData(const CalibrationData* calibrationData, uint8_t nbCalibrationData) {
    int8_t returnError;
    uint16_t size = TranslatorLOG_getTrameSize(SEND_CALIBRATION_DATA, nbCalibrationData);
    Trame* trame = calloc(1, size);

    TranslatorLOG_translateForSendCalibrationData(calibrationData, nbCalibrationData, trame);
    returnError = PostmanLOG_sendMsg(trame, size);

    return returnError;
}

extern int8_t ProxyLoggerMOB_setAllBeaconsData(const BeaconData* beaconsData, unsigned short nbBeacons, Date currentDate) {
    int8_t returnError;
    uint16_t size = TranslatorLOG_getTrameSize(SEND_ALL_BEACONS_DATA, nbBeacons);
    Trame* trame = calloc(1, size);

    TranslatorLOG_translateForSendAllBeaconsData(nbBeacons, beaconsData, currentDate, trame);
    returnError = PostmanLOG_sendMsg(trame, size);

    return returnError;
}

extern int8_t ProxyLoggerMOB_setCurrentPosition(const Position* currentPosition, Date currentDate) {
    int8_t returnError;
    uint16_t size = TranslatorLOG_getTrameSize(SEND_CURRENT_POSITION, 0);
    Trame* trame = calloc(1, size);

    TranslatorLOG_translateForSendCurrentPosition(currentPosition, currentDate, trame);
    returnError = PostmanLOG_sendMsg(trame, size);

    return returnError;
}

extern int8_t ProxyLoggerMOB_setProcessorAndMemoryLoad(const ProcessorAndMemoryLoad* processorAndMemoryLoad, Date currentDate) {
    int8_t returnError;
    uint16_t size = TranslatorLOG_getTrameSize(SEND_MEMORY_PROCESSOR_LOAD, 0);
    Trame* trame = calloc(1, size);

    TranslatorLOG_translateForSendMemoryAndProcessorLoad(processorAndMemoryLoad, currentDate, trame);
    returnError = PostmanLOG_sendMsg(trame, size);

    return returnError;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////