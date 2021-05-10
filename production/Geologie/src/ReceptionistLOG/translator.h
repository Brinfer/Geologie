/**
 * @file translator.c
 *
 * @brief Contient differrente fonction permettant de convertir les structures de donnees en
 * un tableau d'octect.
 *
 * @version 1.0
 * @date 5 mai 2021
 * @author GAUTIER Pierre-Louis
 * @copyright BSD 2-clauses
 *
 */

#ifndef TRANSLATOR_
#define TRANSLATOR_

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "tools.h"
#include "config.h"
#include "Mathematician/Mathematician.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @typedef Commande
 *
 * @brief Enumeration contenant l'identifiant des differentes commandes.
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
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Convertie une structure BeaconData en un tableau d'octect.
 *
 * La conversion est ecrite dans le tableau @p dest.
 *
 * @param beaconData Un pointeur vers la stucture a convertir.
 * @param dest Le pointeur vers tableau dans lequel ecrire la conversion.
 *
 * @warning Le tableau @p dest doit etre un tableau dont la taille fait au minimum 32
 * Aucune verification de la taille de celui-ci ne sera faite.
 */
extern void TRANSLATOR_convertBeaconData(const BeaconData* beaconData, unsigned char* dest);

/**
 * @brief Convertie une structure Position en un tableau d'octect.
 *
 * La conversion est ecrite dans le tableau @p dest.
 *
 * @param position Un pointeur vers la stucture a convertir.
 * @param dest Le pointeur vesr tableau dans lequel ecrire la conversion.
 *
 * @warning Le tableau @p dest doit etre un tableau dont la taille fait au minimum 8
 * Aucune verification de la taille de celui-ci ne sera faite.
 */
extern void TRANSLATOR_convertPosition(const Position* position, unsigned char* dest);

#endif /* TRANSLATOR_ */
