/**
 * @file view.h
 *
 * @brief Affiche l'interface de l'utilisateur
 *
 * @version 2.0
 * @date 13-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "../common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int8_t View_new(void);

extern int8_t View_free(void);

extern int8_t View_start(void);

extern int8_t View_stop(void);

extern int8_t View_displayMessage(const char* message);

extern void View_setConnectionStatus(ConnectionState state);

extern void View_signalGetCalibrationData(void);

extern void View_signalEndCalibrationPosition(void);

extern void View_signalEndCalibration(void);
