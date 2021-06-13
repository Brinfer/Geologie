/**
 * @file LED.c
 *
 * @brief Pilote la LED en allumant ou en l'eteignant.
 *
 * @version 2.0
 * @date 04-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "led.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/gpio.h>
#include <stdint.h>
#include <stdbool.h>

#include "../tools.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Indique que l'etat de la LED est: allumee.
 */
#define ON (0)

/**
 * @brief Indique que l'etat de la LED est: eteinte.
 */
#define OFF (1)

/**
 * @brief Contient les informations des demandes de modification sur la LED
 */
static struct gpiohandle_request request;

/**
 * @brief Contient les informations des manipulations sur la LED.
 */
static struct gpiohandle_data data;

#ifdef NLED
/**
 * @brief Indique si la LED a pus etre correctement initialisee.
 *
 * Dans le cas ou celle =-ci n'a pus l'etre, le programme continue, la LED
 * n'etant pas indispansable a son fonctionnement.
 *
 */
static bool isFunctional = false;
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int8_t Led_new(void) {
    int returnError = EXIT_SUCCESS;
    int deviceFile;

#ifdef NLED
    isFunctional = true;
#endif

    /*  Open device */
    deviceFile = open(LED_NAME, 0);
    if (deviceFile < 0) {
#ifdef NLED
        LOG("[LED] Failed to open %s, the program is still running\n", LED_NAME);
        isFunctional = false;
#else
        LOG("[LED] Failed to open %s\n", LED_NAME);
        returnError = -1;
#endif

    } else {
        /* request GPIO line */
        request.lineoffsets[0] = LED_GPIO_LINE;
        request.flags = GPIOHANDLE_REQUEST_OUTPUT;
        memcpy(request.default_values, &data, sizeof(request.default_values));
        strcpy(request.consumer_label, LED_LABEL);
        request.lines = 1; // 1 : number of lines modified in the request

        returnError = ioctl(deviceFile, GPIO_GET_LINEHANDLE_IOCTL, &request);
        if (returnError < 0) {
#ifdef NLED
            LOG("[LED] Failed to issue GET LINEHANDLE IOCTL, the program is still running%s", "\n");
            isFunctional = false;
#else
            LOG("[LED] Failed to issue GET LINEHANDLE IOCTL%s", "\n");
#endif
        }

        returnError = close(deviceFile);
        if (returnError < 0) {
#ifdef NLED
            LOG("[LED] Failed to close GPIO character device file, the program is still running%s", "\n");
            isFunctional = false;
#else
            LOG("[LED] Failed to close GPIO character device file%s", "\n");
#endif
            Led_free();
        }
    }

    return returnError;
}

extern int8_t Led_ledOn(void) {
    int returnError;

#ifdef NLED
    if (isFunctional) {
#endif
        data.values[0] = ON;
        returnError = ioctl(request.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
        ERROR(returnError < 0, "[LED] Error when turning on the LED");

#ifdef NLED
    } else {
        LOG("[LED] Can't use the LED%s", "\n");
        returnError = EXIT_SUCCESS;
    }
#endif

    return returnError;
}

extern int8_t Led_ledOff(void) {
    int returnError;

#ifdef NLED
    if (isFunctional) {
#endif
        data.values[0] = OFF;
        returnError = ioctl(request.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
        ERROR(returnError < 0, "[LED] Error when turning on the LED");

#ifdef NLED
    } else {
        LOG("[LED] Can't use the LED%s", "\n");
        returnError = EXIT_SUCCESS;
    }
#endif

    return returnError;
}

extern int8_t Led_free(void) {
    int returnError;

#ifdef NLED
    if (isFunctional) {
#endif
        returnError = close(request.fd);
        ERROR(returnError < 0, "[LED] Error when closing the LED");

#ifdef NLED
    } else {
        LOG("[LED] Can't use the LED%s", "\n");
        returnError = EXIT_SUCCESS;
    }
#endif

    return returnError;
}
