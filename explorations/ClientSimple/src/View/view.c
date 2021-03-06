/**
 * @file view.c
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

#include "view.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "../common.h"
#include "../tools.h"
#include "../Comm/client.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ASK_CALIBRATION_POSITION '1'

typedef enum {
    WAITING_CONNECTION = 0,
    MAIN,
    CALIBRATION,
    WAIT_CALIBRATION_POSITION,
    WAIT_CALIBRATION_SIGNAL
} IdScreen;

static bool keepGoing = false;

static pthread_mutex_t clientMutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_t viewThread;

static IdScreen currentScreen;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void setKeepGoing(bool newValue);

static bool getKeepGoing(void);

static void setScreen(IdScreen newScreen);

static IdScreen getScreen(void);

static void executeCommand(char command);

static void performCalibrationPosition(void);

static void perform(uint8_t indexCalibrationPosition);

static void* runView(void* _);

static void displayCalibrationScreen(void);

static void displayConnectionScreen(void);

static void displayWaitCalibrationSignalScreen(void);

static void displayWaitCalibrationDataScreen(void);

static void displayMainScreen(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int8_t View_new(void) {
    int8_t returnError;

    returnError = pthread_mutex_init(&clientMutex, NULL);

    if (returnError != EXIT_SUCCESS) {
        ERROR(true, "[View] Error when initialising the mutex");
    } else {
        setKeepGoing(false);
    }

    ERROR(returnError != EXIT_SUCCESS, "[View] Error when creating the view");

    return returnError != EXIT_SUCCESS ? EXIT_FAILURE : EXIT_SUCCESS;   // If the destruction of the mutex fail, returnError is equal to -1
}

extern int8_t View_free(void) {
    int8_t returnError;

    returnError = pthread_mutex_destroy(&clientMutex);
    if (returnError != EXIT_SUCCESS) {
        ERROR(true, "[View] Error when destroying the mutex");
    }

    ERROR(returnError != EXIT_SUCCESS, "[View] Error when destroying the client");

    return returnError != EXIT_SUCCESS ? EXIT_FAILURE : EXIT_SUCCESS;   // If the destruction of the mutex fail, returnError is equal to -1

}

extern int8_t View_start(void) {
    int8_t returnError;

    setKeepGoing(true);
    setScreen(WAITING_CONNECTION);

    returnError = pthread_create(&viewThread, NULL, &runView, NULL);
    ERROR(returnError < 0, "[View] Error when creating the processus");

    return returnError != EXIT_SUCCESS ? EXIT_FAILURE : EXIT_SUCCESS;   // If the start of the process fail, returnError is equal to -1
}

extern int8_t View_stop(void) {
    setKeepGoing(false);

    pthread_join(viewThread, NULL);

    return 0;
}

extern int8_t View_displayMessage(const char* message) {
    return 0;
}

extern void View_setConnectionStatus(ConnectionState state) {
    if (state != CONNECTED) {
        setScreen(WAITING_CONNECTION);
    } else {
        setScreen(MAIN);
    }
}

extern void View_signalGetCalibrationData(void) {
    setScreen(CALIBRATION);
}

extern void View_signalEndCalibrationPosition(void) {
    setScreen(CALIBRATION);
}

extern void View_signalEndCalibration(void) {
    setScreen(MAIN);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void setKeepGoing(bool newValue) {
    pthread_mutex_lock(&clientMutex);
    keepGoing = newValue;
    pthread_mutex_unlock(&clientMutex);
}

static bool getKeepGoing(void) {
    bool returnValue;

    pthread_mutex_lock(&clientMutex);
    returnValue = keepGoing;
    pthread_mutex_unlock(&clientMutex);

    return returnValue;
}

static void setScreen(IdScreen newScreen) {
    pthread_mutex_lock(&clientMutex);
    currentScreen = newScreen;
    pthread_mutex_unlock(&clientMutex);
}

static IdScreen getScreen(void) {
    IdScreen returnValue;

    pthread_mutex_lock(&clientMutex);
    returnValue = currentScreen;
    pthread_mutex_unlock(&clientMutex);

    return returnValue;
}

static void executeCommand(char command) {
    switch (getScreen()) {
        case CALIBRATION:
            perform(command - 48);  // char '0' == int 48
            break;

        case WAIT_CALIBRATION_SIGNAL:
        case WAIT_CALIBRATION_POSITION:
        case WAITING_CONNECTION:
        default:
            break;

        case MAIN:
            switch (command) {
                case ASK_CALIBRATION_POSITION:
                    performCalibrationPosition();
                    break;

                default:
                    break;
            }
    }
}

static void performCalibrationPosition(void) {
    Client_askCalibrationPosition();

    setScreen(WAIT_CALIBRATION_POSITION);
}

static void perform(uint8_t indexCalibrationPosition) {
    Client_signalCalibrationPosition(indexCalibrationPosition);
    setScreen(WAIT_CALIBRATION_SIGNAL);
}

static void displayScreen(void) {
    switch (getScreen()) {
        default:
        case MAIN:
            displayMainScreen();
            break;

        case WAIT_CALIBRATION_SIGNAL:
            displayWaitCalibrationSignalScreen();
            break;

        case WAIT_CALIBRATION_POSITION:
            displayWaitCalibrationDataScreen();
            break;

        case CALIBRATION:
            displayCalibrationScreen();
            break;

        case WAITING_CONNECTION:
            displayConnectionScreen();
            break;
    }
}

static void displayMainScreen(void) {
    LOG("%s", "\033[2J\033[;H");
    LOG("%c) Ask calibration data\n", ASK_CALIBRATION_POSITION);
    LOG("\n\n Please select the command: (1, 2, ...)%s", "\n\n");
}

static void displayWaitCalibrationDataScreen(void) {
    LOG("%s", "\033[2J\033[;H");
    LOG("Waiting for the calibration data...%s", "\n");
}

static void displayWaitCalibrationSignalScreen(void) {
    LOG("%s", "\033[2J\033[;H");
    LOG("Waiting for the end of the calibration...%s", "\n");
}

static void displayCalibrationScreen(void) {
    LOG("%s", "\033[2J\033[;H");

    uint8_t nbCalibrationPosition = Client_getNbCalibrationPosition();

    for (uint8_t i = 0; i < nbCalibrationPosition; i++) {
        CalibrationPosition calibrationPosition;
        Client_getCalibrationPosition(i, &calibrationPosition);
        LOG("%d)\tCalibrationID: %d\tPosition: X=%d ; Y=%d\n", i, calibrationPosition.id, calibrationPosition.position.X, calibrationPosition.position.Y);
    }

    LOG("\n\nPlease select a calibration position: (1, 2, ...)%s", "\n\n");
}

static void displayConnectionScreen(void) {
    LOG("%s", "\033[2J\033[;H");

    LOG("Waiting for to be connected ...%s", "\n");
}

static void* runView(void* _) {
    fd_set env;
    struct timeval l_timeout = { .tv_sec = 5, .tv_usec = 0 }; // wake uup select every 5 second

    while (getKeepGoing()) {

        displayScreen();

        FD_ZERO(&env);
        FD_SET(STDIN_FILENO, &env);

        if (select(FD_SETSIZE, &env, NULL, NULL, &l_timeout) == -1) {
            ERROR(true, "[View] Error with select() ... Exit");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(STDIN_FILENO, &env)) {
            char charInput[2]; // Size 2 for the null character

            if (read(STDIN_FILENO, charInput, sizeof(charInput)) < 0) {
                ERROR(true, "[View] Error when readind the input");
            } else {
                executeCommand(charInput[0]);
            }
        }
    }

    return NULL;
}
