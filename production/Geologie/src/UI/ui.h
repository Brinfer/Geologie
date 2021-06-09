/**
 * @file ui.h
 *
 * @brief Permet de faire le lien avec l’utilisateur  en lui indiquant l’etat de GEOLOGIE (demarre ou eteint).
 *
 * @version 2.0
 * @date 03-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#ifndef UI_
#define UI_
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialise l'interface.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t UI_new(void);

/**
 * @brief Detruit l'interfece
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t UI_free(void);

/**
 * @brief Reçoit l'information que GEOLOGIE est bien en train de demarrer.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t UI_askSignalBeginningGEOLOGIE(void);

/**
 * @brief Reçoit l'information que GEOLOGIE est bien en train de s'eteindre.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t UI_askSignalEndingGEOLOGIE(void);

#endif // UI_
