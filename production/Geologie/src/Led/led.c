/**
 * @file led.c
 *
 * @brief Pilote la led en allumant ou en l'eteignant.
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

#include "../tools.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @def ON
 *
 * @brief Indique que l'etat de la LED est: allumee.
 */
#define ON (0)

/**
 * @def OFF
 *
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int8_t Led_new(void) {
    int returnError = EXIT_FAILURE;
    int deviceFile;

    /*  Open device */
    deviceFile = open(LED_NAME, 0);
    if (deviceFile < 0) {
        LOG("Failed to open %s\n", LED_NAME);

    } else {
        /* request GPIO line */
        request.lineoffsets[0] = LED_GPIO_LINE;
        request.flags = GPIOHANDLE_REQUEST_OUTPUT;
        memcpy(request.default_values, &data, sizeof(request.default_values));
        strcpy(request.consumer_label, LED_LABEL);
        request.lines = 1; // 1 : nombre de ligne modifie dans la requete

        returnError = ioctl(deviceFile, GPIO_GET_LINEHANDLE_IOCTL, &request);
        if (returnError < 0) {
            LOG("Failed to issue GET LINEHANDLE IOCTL%s", "\n");
        }

        returnError = close(deviceFile);
        if (returnError < 0) {
            LOG("Failed to close GPIO character device file%s", "\n");
        }
    }
    return returnError;
}


extern int8_t Led_ledOn(void) {
    int returnError = EXIT_FAILURE;

    data.values[0] = ON;
    returnError = ioctl(request.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
    if (returnError < 0) {
        LOG("Failed to turn ON the led%s", "\n");
    }
    return returnError;
}

extern int8_t Led_ledOff(void) {
    int returnError;

    data.values[0] = OFF;
    returnError = ioctl(request.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
    if (returnError < 0) {
        LOG("Failed to turn OFF the led%s", "\n");
    }
    return returnError;
}

extern int8_t Led_free(void) {
    int returnError;

    returnError = close(request.fd);
    if (returnError < 0) {
        LOG("Failed to close GPIO LINEHANDLE device file%s", "\n");
    }
    return returnError;
}
