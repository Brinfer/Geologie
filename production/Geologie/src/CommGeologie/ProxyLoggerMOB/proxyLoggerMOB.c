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
#include "../../tools.h"
#include "../PostmanLOG/postmanLOG.h"
#include "../TranslatorLOG/translatorLOG.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Envoie le message aupres de PostmanLOG.
 *
 * @param trame La trame a envoyer.
 * @param size La taille de la trame a envoyer.
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t sendMsg(Trame* trame, uint16_t size);

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
    LOG("[ProxyLoggerMOB] Send the experimental traject.%s", "\n");

    int8_t returnError;
    uint16_t size = TranslatorLOG_getTrameSizeExperimentalTraject(experimentalTrajects, nbExperimentalTraject);
    Trame* trame = calloc(1, size);

    TranslatorLOG_translateForSendExperimentalTrajects(experimentalTrajects, nbExperimentalTraject, trame);

    returnError = sendMsg(trame, size);

    return returnError;
}

extern int8_t ProxyLoggerMOB_setExperimentalPositions(const ExperimentalPosition* experimentalPositions, unsigned short nbExperimentalPosition) {
    LOG("[ProxyLoggerMOB] Send the experimental positions.%s", "\n");

    int8_t returnError;
    uint16_t size = TranslatorLOG_getTrameSize(SEND_EXPERIMENTAL_POSITIONS, nbExperimentalPosition);
    Trame* trame = calloc(1, size);

    TranslatorLOG_translateForSendExperimentalPositions(nbExperimentalPosition, experimentalPositions, trame);

    returnError = sendMsg(trame, size);

    return returnError;
}

extern int8_t ProxyLoggerMOB_setCalibrationData(const CalibrationData* calibrationData, uint8_t nbCalibrationData) {
    LOG("[ProxyLoggerMOB] Send the calibration data.%s", "\n");

    int8_t returnError;
    uint16_t size = TranslatorLOG_getTrameSizeCalibrationData(calibrationData, nbCalibrationData);
    Trame* trame = calloc(1, size);

    TranslatorLOG_translateForSendCalibrationData(calibrationData, nbCalibrationData, trame);

    returnError = sendMsg(trame, size);

    return returnError;
}

extern int8_t ProxyLoggerMOB_setAllBeaconsData(const BeaconData* beaconsData, uint8_t nbBeacons, Date currentDate) {
    LOG("[ProxyLoggerMOB] Send the beacons data.%s", "\n");

    int8_t returnError;
    uint16_t size = TranslatorLOG_getTrameSize(SEND_ALL_BEACONS_DATA, nbBeacons);
    Trame* trame = calloc(1, size);

    TranslatorLOG_translateForSendAllBeaconsData(nbBeacons, beaconsData, currentDate, trame);

    returnError = sendMsg(trame, size);

    return returnError;
}

extern int8_t ProxyLoggerMOB_setCurrentPosition(const Position* currentPosition, Date currentDate) {
    LOG("[ProxyLoggerMOB] Send the current position.%s", "\n");

    int8_t returnError;
    uint16_t size = TranslatorLOG_getTrameSize(SEND_CURRENT_POSITION, 0);
    Trame* trame = calloc(1, size);

    TranslatorLOG_translateForSendCurrentPosition(currentPosition, currentDate, trame);

    returnError = sendMsg(trame, size);

    return returnError;
}

extern int8_t ProxyLoggerMOB_setProcessorAndMemoryLoad(const ProcessorAndMemoryLoad* processorAndMemoryLoad, Date currentDate) {
    LOG("[ProxyLoggerMOB] Send the processor and memory load%s", "\n");


    int8_t returnError;
    uint16_t size = TranslatorLOG_getTrameSize(SEND_MEMORY_PROCESSOR_LOAD, 0);
    Trame* trame = calloc(1, size);

    TranslatorLOG_translateForSendMemoryAndProcessorLoad(processorAndMemoryLoad, currentDate, trame);

    returnError = sendMsg(trame, size);

    return returnError;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int8_t sendMsg(Trame* trame, uint16_t size) {
    int8_t returnError = PostmanLOG_sendMsg(trame, size);
    ERROR(returnError < 0, "[ProxyLoggerMOB] Can't send the message ... Abondement");

    return returnError;
}
