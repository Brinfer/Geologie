/**
 * @file postmanLOG.c
 *
 * @brief Gere le socket de communication.
 *
 * Joue le role de serveur dans cette communication et ecoute le socket pour etablir une
 * connexion ou recevoir une communication.
 * PostmanLOG ne sait pas decrypter les messages qu’il envoie ou qu’il reçoit.
 * Il s’occupe uniquement de donner les messages qu’il reçoit a DispatcherLOG ainsi que
 * d’envoyer les messages fournis par ProxyGUI et ProxyLoggerMOB.
 *
 * @version 2.0
 * @date 05-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "postmanLOG.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../../tools.h"
#include "DispatcherLOG/dispatcherLOG.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Le nombre maximal de connexion possible
 *
 */
#define MAX_PENDING_CONNECTIONS (1)

/**
 * @brief Le numero de port que le serveur ecoute.
 *
 */
#define ROBOT_PORT (12345)

/**
 * @brief Le type de transmission des messages
 *
 * @see <a href="https://www.man7.org/linux/man-pages/man2/send.2.html">send()</a>
 * @see #sendMessage
 */
#define SEND_FLAGS (0) // No flags

/**
 * @brief Le type de transmission des messages
 *
 * @see <a href="https://www.man7.org/linux/man-pages/man2/recv.2.html">recv()</a>
 * @see readMessage()
 */
#define RECV_FLAGS (MSG_WAITALL)

/**
 * @brief Socket serveur qui ecoute les connexions sur le port #ROBOT_PORT.
 *
 */
static int32_t myServerSocket;

/**
 * @brief Socket client utilise par GEOMOBILE pour communiquer avec GEOLOGIE.
 *
 */
static int32_t myClientSocket;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialise le socket #myServerSocket.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t setUpSocket(void);

/**
 * @brief Ferme le socket #myServerSocket.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t tearDownSocket(void);

/**
 * @brief Lie @a nbToRead d'octet et les places dans @a destTrame .
 *
 * @param destTrame La trame ou place le resultat de la lecture.
 * @param nbToRead Le nombre d'octet a lire.
 * @return int8_t Le nombre d'octet n'ayant pas ete lue (o en cas de succes une valeur negative sinon).
 */
static int8_t readMessage(Trame destTrame, uint8_t nbToRead);

/**
 * @brief Envoie @a size d'octet et contenue dans @a trame .
 *
 * @param trame La trame a envoye.
 * @param size La taille de la la trame a envoye.
 * @return int8_t Le nombre d'octet n'ayant pas ete envoye (o en cas de succes une valeur negative sinon).
 */
static int8_t sendMessage(Trame trame, uint8_t size);

/**
 * @brief Accepte la connexion avec le client
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t connectClient(void);

/**
 * @brief Ferme le socket du client.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t disconnectClient(void);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int8_t PostmanLOG_new(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = setUpSocket();
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}

extern int8_t PostmanLOG_start(void) {
    return 0;
}

extern int8_t PostmanLOG_sendMsg(Trame trame, uint8_t size) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = sendMessage(trame, size);
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}

extern int8_t PostmanLOG_readMsg(Trame destTrame, uint8_t nbToRead) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = readMessage(destTrame, nbToRead);
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}

extern int8_t PostmanLOG_stop(void) {
    return 0;
}

extern int8_t PostmanLOG_free(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = tearDownSocket();
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int8_t setUpSocket(void) {
    TRACE("%sSet up the server%s", "\033[44m\033[37m", "\033[0m\n");

    int8_t returnError = EXIT_SUCCESS;
    struct sockaddr_in serverAddress;

    myServerSocket = socket(AF_INET, SOCK_STREAM, 0);                                               // Socket creation : AF_INET = IP, SOCK_STREAM = TCP
    assert(myServerSocket >= 0);

    serverAddress.sin_family = AF_INET;                                                             // Address type = IP (source internet family)
    serverAddress.sin_port = htons(ROBOT_PORT);                                                     // TCP port where the service is accessible (port)
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);                                              // We focus on all interfaces

    returnError = bind(myServerSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));   // We attach the socket to the indicated address
    assert(returnError >= 0);

    return returnError;
}

static int8_t tearDownSocket(void) {
    TRACE("%sTear down the server%s", "\033[44m\033[37m", "\033[0m\n");

    int8_t returnError = EXIT_SUCCESS;

    returnError = close(myClientSocket);
    assert(returnError >= 0);

    returnError = close(myServerSocket);
    assert(returnError >= 0);

    return returnError;
}

static int8_t readMessage(Trame destTrame, uint8_t nbToRead) {
    TRACE("%sRead a message%s", "\033[36m", "\033[0m\n");

    uint8_t quantityReaddean = 0;

    quantityReaddean = recv(myClientSocket, &destTrame + quantityReaddean, nbToRead, RECV_FLAGS);

    if (quantityReaddean < 0) {
        LOG("Error when receiving the message%s", "\n");
    } else if (quantityReaddean == 0) {
        TRACE("%s Client is disconnect%s", "\033[43m\033[37m", "\033[0m\n");
        // DISCONECT
        // TODO
    }

    return (quantityReaddean - nbToRead);
}

static int8_t sendMessage(Trame trame, uint8_t size) {
    TRACE("%sSend a message%s", "\033[36m", "\033[0m\n");

    uint8_t quantityWritten = 0;
    uint8_t quantityToWrite = size;

    while (quantityToWrite > 0) {
        quantityWritten = send(myClientSocket, trame + quantityWritten, quantityToWrite, SEND_FLAGS);

        if (quantityWritten < 0) {
            LOG("Error when sending the message%s", "\n");
            break; // quantityToWrite
        } else {
            quantityToWrite -= quantityWritten;
        }
    }
    return (quantityWritten - size);
}

static int8_t connectClient(void) {
    int8_t returnError = EXIT_SUCCESS;
    myClientSocket = accept(myServerSocket, NULL, 0);

    if (myClientSocket < 0) {
        LOG("Error when connecting the client%s", "\n");
        returnError = -1;
    } else {
        LOG("Establish a connection with the client%s", "\n");
    }

    return returnError;
}

static int8_t disconnectClient(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = close(myClientSocket);

    if (returnError < 0) {
        LOG("Error when disconnecting the client%s", "\n");
    } else {
        LOG("Client is dosconnected%s", "\n");
    }

    return returnError;
}
