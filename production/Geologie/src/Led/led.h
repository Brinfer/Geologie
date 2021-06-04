/**
 * @file led.h
 *
 * @brief Pilote la led en allumant ou en l'eteignant.
 *
 * @version 2.0
 * @date 03-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#ifndef LED_
#define LED_

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @def LED_NAME
 *
 * @brief Correspond au chemin vers le fichier de la LED.
 *
 * @warning Depend de la carte.
 */
#define LED_NAME "/dev/gpiochip0"

/**
 * @def LED_GPIO_LINE
 *
 * @brief Correspond au numero de la ligne de la LED.
 *
 * @warning Depend de la carte.
 */
#define LED_GPIO_LINE (14)

/**
 * @def LED_LABEL
 *
 * @brief Correspond au label de la LED.
 */
#define LED_LABEL "LED_Geologie"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialise la led.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon
 */
extern int8_t Led_new(void);

/**
 * @brief Libere la led.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon
 */
extern int8_t Led_free(void);

/**
 * @brief Allume la led.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon
 */
extern int8_t Led_ledOn(void);

/**
 * @brief Etiend la led.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon
 */
extern int8_t Led_ledOff(void);

#endif // LED_
