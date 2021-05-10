/**
 * @file translator.c
 *
 * @brief TODO
 *
 * @version 1.0
 * @date 5 mai 2021
 * @author GAUTIER Pierre-Louis
 * @copyright BSD 2-clauses
 *
 */

#include "tools.h"
#include "config.h"
#include "Geographer/geographer.h"

#ifndef TRANSLATOR_
#define TRANSLATOR_

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @typedef Commande
 *
 * @brief
 *
 */
typedef enum {
    ERROR = -1,

    ASK_CONNECTION = 0,
    ASK_EXPERIMENTAL_POSITION,
    ASK_ALL_BEACONS_DATA,
    ASK_CURRENT_POSITION,
    ASK_CALIBRATION_POSITION,
    ASK_MEMORY_PROCESSOR_LOAD,

    SIGNAL_CALIBRATION_START,
    SIGNAL_CALIBRATION_POSITION,
    SIGNAL_CALIBRATION_END,

    MIN_VALUE = 0,
    MAX_VALUE = SIGNAL_CALIBRATION_END
} Commande;


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

extern int TRANSLATOR_convertBeaconData(const BeaconData* beaconData, unsigned char dest[32]);

extern int TRANSLATOR_convertPosition(const Position* position, unsigned char dest[8]);


#endif /* TRANSLATOR_ */
