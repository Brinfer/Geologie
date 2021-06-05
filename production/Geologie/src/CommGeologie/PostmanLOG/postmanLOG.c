/**
 * @file postmanLOG.c
 *
 * @brief Gére le socket de communication.
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

#include "../tools.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @def MAX_PENDING_CONNECTIONS
 * @brief Le nombre maximal de connexion possible
 *
 */
#define MAX_PENDING_CONNECTIONS (1)

/**
 * @def ROBOT_PORT
 * @brief Le numero de port que le serveur ecoute.
 *
 */
#define ROBOT_PORT (12345)

/**
 * @brief Socket serveur qui ecoute les connexions sur le port #ROBOT_PORT.
 *
 */
static int32_t myServerSocket;

/**
 * @brief Socket client utilisé par GEOMOBILE pour communiquer avec GEOLOGIE.
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
static int8_t setUpConnection(void);

/**
 * @brief Ferme le socket #myServerSocket.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t tearDownConnection(void);

/**
 * @brief Lie @a nbToRead d'octet et les places dans @a destTrame .
 *
 * @param destTrame La trame ou place le resultat de la lecture.
 * @param nbToRead Le nombre d'octet a lire.
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t readMessage(Trame destTrame, uint8_t nbToRead);

/**
 * @brief Envoie @a size d'octet et contenue dans @a trame .
 *
 * @param trame La trame a envoye.
 * @param size La taille de la la trame a envoye.
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t sendMessage(Trame trame, uint8_t size);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int8_t PostmanLOG_new(void) {
    int8_t returnError = EXIT_FAILURE;

    returnError = setUpConnection();
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}


extern int8_t PostmanLOG_start(void) {
    return 0;
}


extern int8_t PostmanLOG_sendMsg(Trame trame, uint8_t size) {
    int8_t returnError = EXIT_FAILURE;

    returnError = sendMessage(trame, size);
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}

extern int8_t PostmanLOG_readMsg(Trame destTrame, uint8_t nbToRead) {
    int8_t returnError = EXIT_FAILURE;

    returnError = readMessage(destTrame, nbToRead);
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}


extern int8_t PostmanLOG_stop(void) {
    return 0;
}

extern int8_t PostmanLOG_free(void) {
    int8_t returnError = EXIT_FAILURE;

    returnError = tearDownConnection();
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int8_t setUpConnection(void) {
    int8_t returnError = EXIT_FAILURE;
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

static int8_t tearDownConnection(void) {
    int8_t returnError = EXIT_FAILURE;

    returnError = close(myClientSocket);
    assert(returnError >= 0);

    returnError = close(myServerSocket);
    assert(returnError >= 0);

    return returnError;
}

static int8_t readMessage(Trame destTrame, uint8_t nbToRead) {
    return 0;
}

static int8_t sendMessage(Trame trame, uint8_t size) {
    return 0;
}
