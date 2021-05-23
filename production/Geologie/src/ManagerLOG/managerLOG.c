/**
 * @file managerLOG.c
 *
 * @version 1.0
 * @date 5 mai 2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 *
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "managerLOG.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/gpio.h>

#include "../tools.h"
#include "../config.h"

// TODO suppress
#include "ReceptionistLOG/receptionistLOG.h"
#include "Receivers/receivers.h"


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

/**
 * @fn static int ledNew(void)
 * @brief Configure la LED indiquant le lancement de Geologie.
 *
 * @return int 0 en cas de succes, -1 sinon.
 */
static int ledNew(void);

/**
 * @fn static int ledFree(void)
 * @brief Retire tout controle sur la LED.
 *
 * @return int 0 en cas de succes, -1 sinon.
 */
static int ledFree(void);

/**
 * @fn static int ledON(void)
 * @brief Allume la LED.
 *
 * @return int 0 en cas de succes, -1 sinon.
 */
static int ledON(void);

/**
 * @fn static int ledOFF(void)
 * @brief Eteins la LED.
 *
 * @return int 0 en cas de succes, -1 sinon.
 */
static int ledOFF(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void MangerLOG_startGeologie(void) {
    TRACE("%sManagerLOG launched%s\n", "\033[42m\033[37m", "\033[0m");

    ledNew();
    ledON();

    // TODO Use Geographer
    ReceptionistLOG_new();
    ReceptionistLOG_start();
    // Receivers_start(NULL);
}

extern void ManagerLOG_stopGeologie(void) {
    ledOFF();
    ledFree();

    TRACE("%sManagerLOG stopped%s\n", "\033[41m\033[37m", "\033[0m");
    // Receivers_stop(NULL);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int ledNew(void) {
    int returnValue = EXIT_FAILURE;
    int deviceFile;

    /*  Open device */
    deviceFile = open(LED_NAME, 0);
    if (deviceFile < 0) {
        LOG("%sFailed to open %s%s\n", "\033[41m\033[37m", LED_NAME, "\033[0m");

    } else {
        /* request GPIO line */
        request.lineoffsets[0] = LED_GPIO_LINE;
        request.flags = GPIOHANDLE_REQUEST_OUTPUT;
        memcpy(request.default_values, &data, sizeof(request.default_values));
        strcpy(request.consumer_label, LED_LABEL);
        request.lines = 1; // 1 : nombre de ligne modifie dans la requete

        returnValue = ioctl(deviceFile, GPIO_GET_LINEHANDLE_IOCTL, &request);
        if (returnValue < 0) {
            LOG("%sFailed to issue GET LINEHANDLE IOCTL%s\n", "\033[41m\033[37m", "\033[0m");
        }

        returnValue = close(deviceFile);
        if (returnValue < 0) {
            LOG("%sFailed to close GPIO character device file%s\n", "\033[41m\033[37m", "\033[0m");
        }
    }

    return returnValue;
}

static int ledFree(void) {
    int returnValue;

    returnValue = close(request.fd);
    if (returnValue < 0) {
        LOG("%sFailed to close GPIO LINEHANDLE device file%s\n", "\033[41m\033[37m", "\033[0m");
    }
    return returnValue;
}

static int ledON(void) {
    int returnValue = EXIT_FAILURE;

    data.values[0] = ON;
    returnValue = ioctl(request.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
    if (returnValue < 0) {
        LOG("%sFailed to turn ON the led%s\n", "\033[41m\033[37m", "\033[0m");
    }
    return returnValue;
}

static int ledOFF(void) {
    int returnValue;

    data.values[0] = OFF;
    returnValue = ioctl(request.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
    if (returnValue < 0) {
        LOG("%sFailed to turn Off the led %s\n", "\033[41m\033[37m", "\033[0m");
    }
    return returnValue;
}
