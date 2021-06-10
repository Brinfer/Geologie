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
#include <mqueue.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
fd_set l_env;

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "../../tools.h"
#include "../DispatcherLOG/dispatcherLOG.h"

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
 * @see #socketSendMessage
 */
#define SEND_FLAGS (0) // No flags

/**
 * @brief Le type de transmission des messages
 *
 * @see <a href="https://www.man7.org/linux/man-pages/man2/recv.2.html">recv()</a>
 * @see socketReadMessage()
 */
#define RECV_FLAGS (MSG_WAITALL)

/**
 * @brief Le nom de la boite au lettre de PostmanLOG.
 */
#define MQ_LABEL "/MQ_POSTMAN_LOG"

/**
 * @brief Le nombre maximale de message dans la boite aux lettre de PostmanLOG.
 */
#define MQ_MAX_MESSAGES (10)

/**
 * @brief Les indicateurs de la boite au lettre de PostmanLOG.
 *
 */
#define MQ_FLAGS (O_CREAT | O_RDWR)

/**
 * @brief Les modes de la boite aux lettre de PostmanLOG.
 */
#define MQ_MODE (S_IRUSR | S_IWUSR)


typedef enum {
    SEND = 0,
    STOP = 1
} Flag;

/**
 * @brief La structure correspondant aux message passant par la boite aux letres de PostmanLOG.
 */
typedef struct {
    Trame* trame;   /**< Un pointeur vers la #Trame a faire passer. */
    uint16_t size;  /**< La taille de la #Trame a faire passer. */
    Flag flag;
} MqMsg;

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

/**
 * @brief La boite aux lettre de PostmanLOG.
 */
static mqd_t myMq;

/**
 * @brief Le thread de PostmanLOG
 */
static pthread_t myThread;

/**
 * @brief Le mutex de PostmanLOG.
 *
 */
static pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Le status de la connexion.
 */
static ConnectionState connectionState = DISCONNECTED;

static bool keepGoing = false;

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
static int8_t socketReadMessage(Trame* destTrame, uint8_t nbToRead);

/**
 * @brief Envoie @a size d'octet et contenue dans @a trame .
 *
 * @param trame La trame a envoye.
 * @param size La taille de la la trame a envoye.
 * @return int8_t Le nombre d'octet n'ayant pas ete envoye (o en cas de succes une valeur negative sinon).
 */
static int8_t socketSendMessage(Trame* trame, uint8_t size);

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

/**
 * @brief Arrete le processus de PostmanLOG.
 */
static void stopAll(void);

/**
 * @brief Ecrit la #MqMsg dans la boite aux lettres de PostmanLOG.
 *
 * @param message Le message a ecrire.
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 *
 * @warning La fonction appelante est en charge de la garantie de la validite du message (passage de pointeur).
 */
static int8_t mqSendMessage(MqMsg* message);

/**
 * @brief Lie une #MqMsg dans la boite au lettre de PostmanLOG.
 *
 * Le message lue est place dans @a dest.
 *
 * @param dest La #MqMsg ou place le message lue.
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t mqReadMessage(MqMsg* dest);

/**
 * @brief Retourne l'indication du thread de PostmanLOG sur s'il doit continuer ou non sa routine.
 *
 * @return true Le thread de PostmanLOG doit continuer sa routine.
 * @return false Le thread de PostmanLOG doit arreter sa routine.
 */
static bool getKeepGoing(void);

/**
 * @brief Modifie l'indication du thread de PostmanLOG sur s'il doit continuer ou non sa routine.
 *
 * @param newValue
 */
static void setKeepGoing(bool newValue);

/**
 * @brief Retourne l'etat de la connexion.
 *
 * @return ConnectionState L'etat de la connexion.
 */
static ConnectionState getConnectionState(void);

/**
 * @brief Modifie l'etat de la connexion.
 *
 * @param newValue Le nouvelle etat de la connexion.
 */
static void setConnectionState(ConnectionState newValue);

/**
 * @brief Routine du thread de PostmanLOG.
 *
 * @param _ Le parametre passe a la routine du thread, ignore ici
 * @return void* La valeur retourne par la routine du thread, NULL ici
 */
static void* run(void* _);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int8_t PostmanLOG_new(void) {
    int8_t returnError = EXIT_SUCCESS;

    /* Init Socket */
    returnError = setUpSocket();
    STOP_ON_ERROR(returnError < 0);

    /* Init Mq */
    mq_unlink(MQ_LABEL);
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof(MqMsg);
    attr.mq_curmsgs = 0;
    myMq = mq_open(MQ_LABEL, MQ_FLAGS, MQ_MODE, &attr);
    STOP_ON_ERROR(myMq < 0);

    /* Init mutex */
    returnError = pthread_mutex_init(&myMutex, NULL);
    STOP_ON_ERROR(returnError < 0);

    /* Set value */
    setKeepGoing(false);
    setConnectionState(DISCONNECTED);

    return returnError;
}

extern int8_t PostmanLOG_start(void) {
    TRACE("%sStart the server%s", "\033[44m\033[37m", "\033[0m\n");

    int8_t returnError = EXIT_SUCCESS;

    setKeepGoing(true);
    setConnectionState(DISCONNECTED);

    returnError = pthread_create(&myThread, NULL, &run, NULL);
    STOP_ON_ERROR(returnError < 0);

    return 0;
}

extern int8_t PostmanLOG_sendMsg(Trame* trame, uint16_t size) {
    int8_t returnError = EXIT_SUCCESS;

    MqMsg msg = { .size = size, .trame = trame, .flag = SEND };

    returnError = mqSendMessage(&msg);
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}

extern int8_t PostmanLOG_readMsg(Trame* destTrame, uint8_t nbToRead) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = socketReadMessage(destTrame, nbToRead);

    return returnError;
}

extern int8_t PostmanLOG_stop(void) {
    TRACE("%sStop the server%s", "\033[44m\033[37m", "\033[0m\n");

    int8_t returnError = EXIT_SUCCESS;

    setKeepGoing(false);
    if (connectionState == CONNECTED) {
        returnError = disconnectClient();
        STOP_ON_ERROR(returnError < 0);
    }

    returnError = shutdown(myServerSocket, SHUT_RDWR); // wake up accept and recv, do not destroy the socket
    STOP_ON_ERROR(returnError < 0);

    MqMsg msg = { .size = 0, .trame = NULL, .flag = STOP };
    returnError = mqSendMessage(&msg); // wake up the PostmanLOG's thread to stop him
    STOP_ON_ERROR(returnError < 0);

    pthread_join(myThread, NULL);
    TRACE("arret thread postman %s ", "\n");

    return 0;
}

extern int8_t PostmanLOG_free(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = tearDownSocket();
    STOP_ON_ERROR(returnError < 0);

    returnError = mq_unlink(MQ_LABEL);
    STOP_ON_ERROR(returnError < 0);

    returnError = mq_close(myMq);
    STOP_ON_ERROR(returnError < 0);

    returnError = pthread_mutex_destroy(&myMutex);
    STOP_ON_ERROR(returnError < 0);

    return returnError;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
////////////////////////>=////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    if (myClientSocket > 0) {
        returnError = close(myClientSocket);
        assert(returnError >= 0);
    }

    returnError = close(myServerSocket);
    assert(returnError >= 0);

    return returnError;
}

static int8_t socketReadMessage(Trame* destTrame, uint8_t nbToRead) {
    TRACE("%sRead a message%s", "\033[36m", "\033[0m\n");

    int16_t quantityReaddean = 0;

    if (getConnectionState() == CONNECTED) {
        quantityReaddean = recv(myClientSocket, &destTrame + quantityReaddean, nbToRead, RECV_FLAGS);

        if (quantityReaddean < 0) {
            LOG("Error when receiving the message%s", "\n");
            if (errno == ECONNRESET) {
                LOG("Connection aborted by the server%s", "\n");
                stopAll();
                quantityReaddean = 0;
            }
        } else if (quantityReaddean == 0) {
            LOG("Client is disconnect%s", "\n");
            stopAll();
        }
    } else {
        DispatcherLOG_setConnectionState(DISCONNECTED);
    }
    return (quantityReaddean - nbToRead);
}

static int8_t socketSendMessage(Trame* trame, uint8_t size) {
    TRACE("%sSend a message%s", "\033[36m", "\033[0m\n");

    int16_t quantityWritten = 0;
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

static void stopAll(void) {
    disconnectClient();
    MqMsg msg = { .size = 0, .trame = NULL, .flag = STOP };
    mqSendMessage(&msg); // wake up the PostmanLOG's thread to stop him
}

static int8_t connectClient(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = listen(myServerSocket, MAX_PENDING_CONNECTIONS);
    STOP_ON_ERROR(returnError < 0);

    myClientSocket = accept(myServerSocket, NULL, 0);

    if (myClientSocket < 0) {
        if (errno == EINVAL) {
            // Socket no more accept any connection
            LOG("Connection aborted by the server%s", "\n");
        } else {
            LOG("Error when connecting the client%s", "\n");
            returnError = -1;
        }
    } else {
        setConnectionState(CONNECTED);
        DispatcherLOG_setConnectionState(CONNECTED);
    }

    return returnError;
}

static int8_t disconnectClient(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = close(myClientSocket);

    if (returnError < 0) {
        LOG("Error when disconnecting the client%s", "\n");
    } else {
        setConnectionState(DISCONNECTED);
        DispatcherLOG_setConnectionState(DISCONNECTED);
    }

    return returnError;
}

static int8_t mqSendMessage(MqMsg* message) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = mq_send(myMq, (char*) message, sizeof(MqMsg), 0); // put char to avoid a warning
    assert(returnError >= 0);

    return returnError;
}

static int8_t mqReadMessage(MqMsg* dest) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = mq_receive(myMq, (char*) dest, sizeof(MqMsg), NULL); // put char to avoid a warning
    assert(returnError >= 0);

    return returnError;
}

static bool getKeepGoing(void) {
    bool returnValue;
    pthread_mutex_lock(&myMutex);
    returnValue = keepGoing;
    pthread_mutex_unlock(&myMutex);

    return returnValue;
}

static void setKeepGoing(bool newValue) {
    pthread_mutex_lock(&myMutex);
    keepGoing = newValue;
    pthread_mutex_unlock(&myMutex);
}

static ConnectionState getConnectionState(void) {
    bool returnValue;
    pthread_mutex_lock(&myMutex);
    returnValue = connectionState;
    pthread_mutex_unlock(&myMutex);

    return returnValue;
}

static void setConnectionState(ConnectionState newValue) {
    pthread_mutex_lock(&myMutex);
    connectionState = newValue;
    pthread_mutex_unlock(&myMutex);
}

static void* run(void* _) {
    while (getKeepGoing() == true) {
        int8_t returnError = EXIT_SUCCESS;
        MqMsg msg;

        if (getConnectionState() == DISCONNECTED) {
            TRACE("On retente de se connecter %s", "\n");

            returnError = connectClient();
            STOP_ON_ERROR(returnError);
        }

        returnError = mqReadMessage(&msg);
        STOP_ON_ERROR(returnError < 0);

        if (msg.flag == SEND) {
            if (getConnectionState() == CONNECTED && getKeepGoing() == true) {
                returnError = socketSendMessage(msg.trame, msg.size);
                STOP_ON_ERROR(returnError);
            }
            free(msg.trame);
        }
    }
    return NULL;
}
