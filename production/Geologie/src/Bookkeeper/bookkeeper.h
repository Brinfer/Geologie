/**
 * @file bookkeeper.h
 *
 * @brief Permet de calculer les charges memoire et processeur de maniere periodique.
 *
 * @version 2.0
 * @date 12-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#ifndef BOOKKEEPER_
#define BOOKKEEPER_

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

/**
 * @brief Initialise Bookkeeper.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t Bookkeeper_new(void);

/**
 * @brief Detruit Bookkeeper.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t Bookkeeper_free(void);

/**
 * @brief Demande le demarrage de bookkeeper.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t Bookkeeper_askStartBookkeeper(void);

/**
 * @brief Demande l'arret de Bookkeeper.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t Bookkeeper_askStopBookkeeper(void);

/**
 * @brief Demande aupres de Bookkeeper la charge processeur et memoire actuelle.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t Bookkeeper_ask4CurrentProcessorAndMemoryLoad();

#endif /* BOOKKEEPER_ */
