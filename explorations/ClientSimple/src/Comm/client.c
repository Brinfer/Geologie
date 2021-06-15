/**
 * @file client.c
 *
 * @brief Permet de faire un client simple.
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

#include "client.h"
#include "translator.h"

#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../tools.h"
#include "../common.h"
#include "../View/view.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TEMP_CONNECTION (1)

static bool keepGoing = false;

static int32_t clientSocket;

static struct sockaddr_in serverAdress;

static pthread_t clientThread;

static pthread_mutex_t clientMutex = PTHREAD_MUTEX_INITIALIZER;

static ConnectionState connectionState;

static CalibrationPosition* calibrationPosition;

static uint8_t nbCalibrationPosition;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int8_t setUpSocket(void);

static int8_t tearDownSocket(void);

static int8_t setUpServer(void);

static void setKeepGoing(bool newValue);

static bool getKeepGoing(void);

static void setCalibrationPosition(uint8_t nbCalibrationPosition);

static void getCalibrationPosition(uint8_t index, CalibrationPosition* dest);

static int8_t connection(void);

static int8_t socketReadMessage(Trame* destTrame, uint16_t nbToRead);

static int8_t socketSendMessage(Trame* trame, uint16_t nbToSend);

static void setConnectionState(ConnectionState newValue);

static void setNbCalibrationPosition(uint8_t nbPosition);

static uint8_t getNbCalibrationPosition(void);

static void* runClient(void* _);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int8_t Client_new(void) {
    int8_t returnError;

    returnError = setUpServer();

    if (returnError == EXIT_SUCCESS) {
        returnError = setUpSocket();
    } else {
        ERROR(true, "[Client] Error when initialising the server");
    }

    if (returnError == EXIT_SUCCESS) {
        returnError = pthread_mutex_init(&clientMutex, NULL);

        if (returnError != EXIT_SUCCESS) {
            ERROR(true, "[Client] Error when initialising the mutex");
        }

    } else {
        ERROR(true, "[Client] Error when initialising the client socket");
    }

    ERROR(returnError != EXIT_SUCCESS, "[Client] Error when creating the client");

    return returnError != EXIT_SUCCESS ? EXIT_FAILURE : EXIT_SUCCESS;   // If the destruction of the mutex fail, returnError is equal to -1
}

extern int8_t Client_free(void) {
    int8_t returnError;

    pthread_mutex_lock(&clientMutex);
    free(calibrationPosition);
    pthread_mutex_unlock(&clientMutex);

    returnError = tearDownSocket();

    if (returnError == EXIT_SUCCESS) {
        returnError = pthread_mutex_destroy(&clientMutex);
        ERROR(returnError != EXIT_SUCCESS, "[Client] Error when destroying the mutex");
    }

    ERROR(returnError != EXIT_SUCCESS, "[Client] Error when destroying the client");

    return returnError != EXIT_SUCCESS ? EXIT_FAILURE : EXIT_SUCCESS;   // If the destruction of the mutex fail, returnError is equal to -1
}

extern int8_t Client_start(void) {
    int8_t returnError;

    setConnectionState(DISCONNECTED);
    setKeepGoing(true);

    returnError = pthread_create(&clientThread, NULL, &runClient, NULL);
    ERROR(returnError < 0, "[Client] Error when creating the processus");

    return returnError < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

extern int8_t Client_stop(void) {
    int8_t returnError;

    setConnectionState(DISCONNECTED);
    setKeepGoing(false);

    returnError = shutdown(clientSocket, SHUT_RDWR); // wake up accept and recv, do not destroy the socket
    ERROR(returnError < 0, "[PostmanLOG] Error when shutdown the socket");

    returnError = pthread_join(clientThread, NULL);
    ERROR(returnError < 0, "[Client] Error when stopping the processus");

    return returnError < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

extern int8_t Client_signalCalibrationPosition(uint8_t calibrationPositionIndex) {

    CalibrationPositionId calibrationId = calibrationPosition[calibrationPositionIndex].id; // TODO Protect

    Trame trame[SIZE_HEADER + 1] = { SIGNAL_CALIBRATION_POSITION , 0x00, 0x01, calibrationId };
    return socketSendMessage(trame, SIZE_HEADER + 1);
}

extern int8_t Client_askCalibrationPosition(void) {
    Trame trame[SIZE_HEADER] = { ASK_CALIBRATION_POSITIONS , 0x00, 0x00 };
    return socketSendMessage(trame, SIZE_HEADER);
}

extern void Client_getCalibrationPosition(uint8_t index, CalibrationPosition* dest) {
    getCalibrationPosition(index, dest);
}

extern int8_t Client_getNbCalibrationPosition(void) {
    return getNbCalibrationPosition();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void setCalibrationPosition(uint8_t nbCalibrationPosition) {
    pthread_mutex_lock(&clientMutex);
    free(calibrationPosition);
    calibrationPosition = malloc(nbCalibrationPosition);
    pthread_mutex_unlock(&clientMutex);
}

static void getCalibrationPosition(uint8_t index, CalibrationPosition* dest) {
    pthread_mutex_lock(&clientMutex);
    dest->id = calibrationPosition[index].id;
    dest->position = calibrationPosition[index].position;
    pthread_mutex_unlock(&clientMutex);
}

static void setNbCalibrationPosition(uint8_t nbPosition) {
    pthread_mutex_lock(&clientMutex);
    nbCalibrationPosition = nbPosition;
    pthread_mutex_unlock(&clientMutex);
}

static uint8_t getNbCalibrationPosition(void) {
    uint8_t returnValue;

    pthread_mutex_lock(&clientMutex);
    returnValue = nbCalibrationPosition;
    pthread_mutex_unlock(&clientMutex);

    return returnValue;
}

static int8_t setUpSocket(void) {
    TRACE("%sSet up the client socket%s", "\033[44m\033[37m", "\033[0m\n");

    int8_t returnError;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0) {
        returnError = EXIT_FAILURE;
        ERROR(true, "[Client] Error when setting up the socket");
    }

    return returnError;
}

static int8_t setUpServer(void) {
    int8_t returnError;

    const struct hostent* l_hostInfo = gethostbyname(SERVER_IP);

    if (l_hostInfo == NULL) {
        ERROR(true, "[Client] Error the host is not reachable");
        returnError = EXIT_FAILURE;
    } else {
        serverAdress.sin_port = htons(SERVER_PORT);
        serverAdress.sin_family = AF_INET;
        serverAdress.sin_addr = *((struct in_addr*) l_hostInfo->h_addr_list[0]);

        returnError = EXIT_SUCCESS;
    }

    return returnError;

}

static int8_t tearDownSocket(void) {
    TRACE("%sTear down the client socket%s", "\033[44m\033[37m", "\033[0m\n");

    int8_t returnError = close(clientSocket);
    ERROR(returnError < 0, "[Client] Error when closing the socket");

    return returnError < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

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

static void setConnectionState(ConnectionState newValue) {
    pthread_mutex_lock(&clientMutex);
    connectionState = newValue;
    pthread_mutex_unlock(&clientMutex);

    View_setConnectionStatus(newValue);
}

static ConnectionState getConnectionState(void) {
    bool returnValue;

    pthread_mutex_lock(&clientMutex);
    returnValue = connectionState;
    pthread_mutex_unlock(&clientMutex);

    return returnValue;
}

static int8_t connection(void) {
    LOG("Connection tent to the server%s", "\n");

    int returnError = EXIT_FAILURE;

    while (getKeepGoing()) {
        if (connect(clientSocket, (struct sockaddr*) &serverAdress, sizeof(serverAdress)) < 0) {
            ERROR(true, "Fail to connect ... retry");
            sleep(TEMP_CONNECTION); // Sleep and retry after
        } else {
            returnError = EXIT_SUCCESS;
            setConnectionState(CONNECTED);

            break; // Connected
        }
    }

    return returnError;
}

static int8_t disconnection(void) {
    int8_t returnError;

    returnError = close(clientSocket);

    return returnError < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

static int8_t socketReadMessage(Trame* destTrame, uint16_t nbToRead) {
    errno = 0;

    int16_t quantityReaddean = 0;
    int16_t returnError;

    if (getConnectionState() == CONNECTED) {
        quantityReaddean = recv(clientSocket, destTrame, nbToRead, MSG_WAITALL);

        if (quantityReaddean < 0) {
            ERROR(errno != ECONNRESET, "[Client] Error when receiving the message in the socket");
            returnError = EXIT_FAILURE;

        } else if (quantityReaddean == 0) {
            LOG("[Client] Client is disconnect%s", "\n");
            setConnectionState(DISCONNECTED);
            disconnection();
            setUpSocket();

            returnError = EXIT_FAILURE;
        } else {
            returnError = EXIT_SUCCESS;
            TRACE("%sRead a message%s", "\033[36m", "\033[0m\n");
        }
    } else {
        returnError = EXIT_FAILURE;
    }

    return returnError;
}

static int8_t socketSendMessage(Trame* trame, uint16_t nbToSend) {
    TRACE("%sSend a message%s", "\033[36m", "\033[0m\n");

    int16_t quantityWritten = 0;
    int16_t quantityToWrite = nbToSend;
    int8_t returnError = EXIT_SUCCESS;

    while (quantityToWrite > 0) {
        quantityWritten = send(clientSocket, trame + quantityWritten, quantityToWrite, 0);

        if (quantityWritten < 0) {
            if (errno == ECONNRESET) {
                LOG("[Client] The socket is shutdown%s", "\n");
                returnError = 0;
            } else {
                ERROR(true, "[Client] Error when sending the message");
            }
            returnError = EXIT_FAILURE;
        } else {
            quantityToWrite -= quantityWritten;
        }
    }

    return returnError;
}

static void* runClient(void* _) {
    while (getKeepGoing()) {
        int8_t returnError;

        if (getConnectionState() == DISCONNECTED) {
            returnError = connection();

            if (returnError != EXIT_SUCCESS) {
                ERROR(true, "[Client] Fail to etablish the connection ... Stop the processus");
                break;
            }
        }

        Header header;
        Trame headerTrame[SIZE_HEADER];

        returnError = socketReadMessage(headerTrame, SIZE_HEADER);

        if (returnError == EXIT_SUCCESS) {
            Translator_translateTrameToHeader(headerTrame, &header);

            /* Read the Data */
            Trame dataTrame[header.size];

            if (header.size > 0) {
                returnError = socketReadMessage(dataTrame, header.size);
            }

            // TODO exploit the data

            if (header.commande == REP_CALIBRATION_POSITIONS) {
                TRACE("[Client] Get the calibration data%s", "\n");
                int8_t nbCalibrationData = dataTrame[0];

                setNbCalibrationPosition(nbCalibrationData);
                setCalibrationPosition(nbCalibrationData);

                pthread_mutex_lock(&clientMutex);
                Translator_translateForRepCalibrationPosition(dataTrame, calibrationPosition, nbCalibrationPosition);
                pthread_mutex_unlock(&clientMutex);

                View_signalGetCalibrationData();
            } else if (header.commande ==  SIGNAL_CALIBRATION_END_POSITION) {
                View_signalEndCalibrationPosition();
            } else if (header.commande == SIGNAL_CALIRATION_END) {
                View_signalEndCalibration();
            }
        }
    }

    return NULL;
}
