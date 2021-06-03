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

/**
 * @brief Initialise la led.
 *
 * @return int 0 en cas de succes, une autre valeur sinon.
 */
extern int Led_new(void);

/**
 * @brief Libere la led.
 *
 * @return int 0 en cas de succes, une autre valeur sinon.
 */
extern int Led_free(void);

/**
 * @brief Allume la led.
 *
 * @return int 0 en cas de succes, une autre valeur sinon.
 */
extern int Led_ledOn(void);

/**
 * @brief Etiend la led.
 *
 * @return int 0 en cas de succes, une autre valeur sinon.
 */
extern int Led_ledOff(void);

#endif // LED_
