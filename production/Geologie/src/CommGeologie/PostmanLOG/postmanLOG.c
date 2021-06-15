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
#include <errno.h>
#include <mqueue.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>

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

/**
 * @brief L'enumeration des different type de message ecrit dans la MQueue de
 * PostmanLOG.
 */
typedef enum {
    SEND = 0,   /**< Le message contenue doit etre envoye. */
    STOP,       /**< Le message indique l'arret du processus. */
    WAKE_UP,    /**< Message uniquement destine a reveiller le thread bloque sur la lecture de la queue */
} Flag;

/**
 * @brief La structure correspondant aux message passant par la boite aux letres de PostmanLOG.
 */
typedef struct {
    Trame* trame;   /**< Un pointeur vers la #Trame a faire passer. */
    uint16_t size;  /**< La taille de la #Trame a faire passer. */
    Flag flag;      /**< #Flag indiquant le type du message*/
} MqMsgPostmanLOG;

/**
 * @brief Socket serveur qui ecoute les connexions sur le port #ROBOT_PORT.
 */
static int32_t myServerSocket;

/**
 * @brief Socket client utilise par GEOMOBILE pour communiquer avec GEOLOGIE.
 */
static int32_t myClientSocket;

/**
 * @brief La boite aux lettre de PostmanLOG.
 */
static mqd_t myMq;

/**
 * @brief Le thread de PostmanLOG
 */
static pthread_t myThreadMq;

/**
 * @brief Le mutex de PostmanLOG.
 *
 */
static pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Le status de la connexion.
 */
static ConnectionState connectionState = DISCONNECTED;

/**
 * @brief Boolean indiquant a Bookkeeper s'il doit continuer son processus.
 */
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
 * @return int8_t -1 en cas d'erreur, 1 en cas de deconnexion du client, 0 sinon.
 */
static int8_t socketReadMessage(Trame* destTrame, uint8_t nbToRead);

/**
 * @brief Envoie @a size d'octet et contenue dans @a trame .
 *
 * @param trame La trame a envoye.
 * @param size La taille de la la trame a envoye.
 * @return int8_t -1 en cas d'erreur, 0 sinon.
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
 * @brief Initialise la queue #myMq.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t setUpMq(void);

/**
 * @brief Ferme la queue #myMq.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t tearDownMq(void);

/**
 * @brief Ecrit la #MqMsgPostmanLOG dans la boite aux lettres de PostmanLOG.
 *
 * @param message Le message a ecrire.
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 *
 * @warning La fonction appelante est en charge de la garantie de la validite du message (passage de pointeur).
 */
static int8_t mqSendMessage(MqMsgPostmanLOG* message);

/**
 * @brief Lie une #MqMsgPostmanLOG dans la boite au lettre de PostmanLOG.
 *
 * Le message lue est place dans @a dest.
 *
 * @param dest La #MqMsgPostmanLOG ou place le message lue.
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t mqReadMessage(MqMsgPostmanLOG* dest);

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
 * @param newValue false le thread de PostmanLOG doit arreter sa routine, sinon il doit continuer
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
    ERROR(returnError < 0, "[PostmanLOG] Fail to set up the socket");

    /* Init Mq */
    if (returnError >= 0) {
        returnError = setUpMq();
        ERROR(returnError < 0, "[PostmanLOG] Fail to set up the queue");
    }

    /* Init mutex */
    if (returnError >= 0) {
        returnError = pthread_mutex_init(&myMutex, NULL);
        ERROR(returnError < 0, "[PostmanLOG] Fail to set up the mutex");

        /* Set value */
        if (returnError >= 0) {
            setKeepGoing(false);
            setConnectionState(DISCONNECTED);
        }
    }

    return returnError;
}

extern int8_t PostmanLOG_start(void) {
    TRACE("%sStart the server%s", "\033[44m\033[37m", "\033[0m\n");

    int8_t returnError = EXIT_SUCCESS;

    setKeepGoing(true);
    setConnectionState(DISCONNECTED);

    returnError = pthread_create(&myThreadMq, NULL, &run, NULL);
    ERROR(returnError < 0, "[PostmanLOG] Fail to create PostmanLOG processus");

    return returnError;
}

extern int8_t PostmanLOG_sendMsg(Trame* trame, uint16_t size) {
    int8_t returnError = EXIT_SUCCESS;

    MqMsgPostmanLOG msg = { .size = size, .trame = trame, .flag = SEND };

    returnError = mqSendMessage(&msg);

    if (returnError < 0) {
        LOG("[PostmanLOG] Fail to send the message in the queue ... Re set up the queue%s", "\n");
        returnError = setUpMq();

        if (returnError < 0) {
            LOG("[PostmanLOG] Fail to re set up the queue ... Abandonment%s", "\n");
        } else {
            LOG("[PostmanLOG] Success to re set up the message queue ... Send the message.%s", "\n");
            returnError = mqSendMessage(&msg);
            ERROR(returnError < 0, "[PostmanLOG] Fail to send the message in the queue ... Abandonment");
        }
    }

    return returnError;
}

extern int8_t PostmanLOG_readMsg(Trame* destTrame, uint8_t nbToRead) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = socketReadMessage(destTrame, nbToRead);

    if (returnError < 0) {
        LOG("[PostmanLOG] Fail to read the message in the queue ... Re set up the queue%s", "\n");
        tearDownSocket();
        returnError = setUpSocket();

        if (returnError < 0) {
            LOG("[PostmanLOG] Fail to re set up the queue ... Abandonment%s", "\n");
        } else {
            LOG("[PostmanLOG] Success to re set up the message queue ... Read the message.%s", "\n");
            returnError = socketReadMessage(destTrame, nbToRead);
            ERROR(returnError < 0, "[PostmanLOG] Fail to read the message in the queue ... Abandonment");
        }
    }

    return returnError;
}

extern int8_t PostmanLOG_stop(void) {
    TRACE("%sStop the server%s", "\033[44m\033[37m", "\033[0m\n");

    int8_t returnError;

    setKeepGoing(false);
    if (getConnectionState() == CONNECTED) {
        disconnectClient();
    }

    returnError = shutdown(myServerSocket, SHUT_RDWR); // wake up accept and recv, do not destroy the socket
    ERROR(returnError < 0, "[PostmanLOG] Error when shutdown the socket");

    if (returnError >= 0) {
        MqMsgPostmanLOG msg = { .size = 0, .trame = NULL, .flag = STOP };
        returnError = mqSendMessage(&msg); // wake up the PostmanLOG's thread to stop him
        ERROR(returnError < 0, "[PostmanLOG] Error when sending the STOP message");

        if (returnError >= 0) {
            returnError = pthread_join(myThreadMq, NULL);
            ERROR(returnError < 0, "[PostmanLOG] Error to join the PostmanLOG's processsus");
        }
    }

    return returnError;
}

extern int8_t PostmanLOG_free(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError -= tearDownSocket();
    ERROR(returnError < 0, "[PostmanLOG] Error when closing the socket");

    returnError -= tearDownMq();
    ERROR(returnError < 0, "[PostmanLOG] Error when closing the queue");

    returnError -= pthread_mutex_destroy(&myMutex);
    ERROR(returnError < 0, "[PostmanLOG] Error when destroy the mutex");

    return returnError < 0 ? -1 : 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int8_t setUpSocket(void) {
    TRACE("%sSet up the server%s", "\033[44m\033[37m", "\033[0m\n");

    int8_t returnError = EXIT_SUCCESS;
    struct sockaddr_in serverAddress;

    myServerSocket = socket(AF_INET, SOCK_STREAM, 0);                                               // Socket creation : AF_INET = IP, SOCK_STREAM = TCP

    if (myServerSocket < 0) {
        ERROR(true, "[PostmanLOG] Error to create the socket");
        returnError = -1;
    } else {
        serverAddress.sin_family = AF_INET;                                                             // Address type = IP (source internet family)
        serverAddress.sin_port = htons(ROBOT_PORT);                                                     // TCP port where the service is accessible (port)
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);                                              // We focus on all interfaces

        returnError = bind(myServerSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));   // We attach the socket to the indicated address

        ERROR(returnError < 0, "[PostmanLOG] Error to attach the socket");
    }

    return returnError;
}

static int8_t tearDownSocket(void) {
    TRACE("%sTear down the server%s", "\033[44m\033[37m", "\033[0m\n");
    int8_t returnError = EXIT_SUCCESS;

    if (myClientSocket > 0) {
        returnError = close(myClientSocket);

        ERROR(returnError < 0, "[PostmanLOG] Error when closing the client socket");
    }

    returnError -= close(myServerSocket);
    ERROR(returnError < 0, "[PostmanLOG] Error when closing the server socket");

    return returnError < 0 ? -1 : 0;
}

static int8_t socketReadMessage(Trame* destTrame, uint8_t nbToRead) {
    int16_t returnError = 0;
    errno = 0;

    if (getConnectionState() == CONNECTED) {
        returnError = recv(myClientSocket, destTrame, nbToRead, RECV_FLAGS);

        if (returnError < 0) {
            ERROR(errno != ECONNRESET, "[PostmanLOG] Error when receiving the message in the socket");
            returnError = -1;

        } else if (returnError == 0) {
            LOG("[PostmanLOG] Client is disconnect.%s", "\n");
            disconnectClient();
            MqMsgPostmanLOG msg = { .size = 0, .trame = NULL, .flag = WAKE_UP };
            returnError = mqSendMessage(&msg); // wake up the PostmanLOG's thread to stop him

            if (returnError < 0) {
                ERROR(true, "[PostmanLOG] Error when sending the internal signal WAKE_UP ... Retry");
                returnError = mqSendMessage(&msg); // wake up the PostmanLOG's thread to stop him

                ERROR(returnError < 0, "[PostmanLOG] Error when sending the internal signal WAKE_UP ... Abandonment");
            }

            if (returnError >= 0) {
                returnError = 1;
            }
        } else {
            returnError = 0;
            TRACE("%sRead a message%s", "\033[36m", "\033[0m\n");
        }
    } else {
        // DispatcherLOG_setConnectionState(DISCONNECTED);
    }

    return returnError;
}

static int8_t socketSendMessage(Trame* trame, uint8_t size) {
    int16_t quantityWritten = 0;
    uint8_t quantityToWrite = size;
    int8_t returnError = EXIT_SUCCESS;

    while (quantityToWrite > 0 && returnError >= 0) {
        quantityWritten = send(myClientSocket, trame + quantityWritten, quantityToWrite, SEND_FLAGS);

        if (quantityWritten < 0) {
            ERROR(errno != ECONNRESET, "[PostmanLOG] Error when sending the message in the socket");
            returnError = -1;
        } else {
            quantityToWrite -= quantityWritten;
        }
    }
    return returnError;
}

static int8_t connectClient(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = listen(myServerSocket, MAX_PENDING_CONNECTIONS);
    ERROR(returnError < 0, "[PostmanLOG] Error when set up the client connection");

    if (returnError >= 0) {
        myClientSocket = accept(myServerSocket, NULL, 0);

        if (myClientSocket < 0) {
            if (errno == EINVAL) {
                // Socket no more accept any connection
                LOG("[PostmanLOG] Connection aborted by the server.%s", "\n");
            } else {
                ERROR(true, "[PostmanLOG] Error when connecting the client");
                returnError = -1;
            }
        } else {
            LOG("[PostmanLOG] New client is connected%s", "\n");
            setConnectionState(CONNECTED);
            DispatcherLOG_setConnectionState(CONNECTED);
        }
    }

    return returnError;
}

static int8_t disconnectClient(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = close(myClientSocket);
    ERROR(returnError < 0, "[PostmanLOG] Error when closing the client socket");

    if (returnError >= 0) {
        setConnectionState(DISCONNECTED);
        DispatcherLOG_setConnectionState(DISCONNECTED);
    }

    return returnError;
}

static int8_t setUpMq(void) {
    int8_t returnError = EXIT_SUCCESS;

    mq_unlink(MQ_LABEL);

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof(MqMsgPostmanLOG);
    attr.mq_curmsgs = 0;
    myMq = mq_open(MQ_LABEL, MQ_FLAGS, MQ_MODE, &attr);

    if (myMq < 0) {
        ERROR(true, "[PostmanLOG] Fail to open the Queue");
        returnError = -1;
    }

    return returnError;
}

static int8_t tearDownMq(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = mq_unlink(MQ_LABEL);

    if (returnError >= 0) {
        returnError = mq_close(myMq);
        ERROR(returnError < 0, "[PostmanLOG] Error when closing the queue");
    } else {
        ERROR(true, "[PostmanLOG] Error when unlinking the queue");
    }

    return returnError;
}

static int8_t mqSendMessage(MqMsgPostmanLOG* message) {
    int8_t returnError = EXIT_SUCCESS;
    errno = 0;

    returnError = mq_send(myMq, (char*) message, sizeof(MqMsgPostmanLOG), 0); // put char to avoid a warning
    ERROR(returnError < 0, "[PostmanLOG] Error when sending the message in the queue");

    return returnError;
}

static int8_t mqReadMessage(MqMsgPostmanLOG* dest) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = mq_receive(myMq, (char*) dest, sizeof(MqMsgPostmanLOG), NULL); // put char to avoid a warning
    ERROR(returnError < 0, "[PostmanLOG] Error when reading the message in the queue");

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
        MqMsgPostmanLOG msg;

        if (getConnectionState() == DISCONNECTED) {
            LOG("[PostmanLOG] Try to connect to the client%s", "\n");

            returnError = connectClient();

            if (returnError < 0) {
                ERROR(true, "[PostmanLOG] Error when trying to etablish a connection ... Retry");

                returnError = connectClient();
                if (returnError < 0) {
                    ERROR(true, "[PostmanLOG] Error when trying to etablish a connection ... Stop the processus");
                    setKeepGoing(false);
                    break;
                }
            }
        }

        if (returnError >= 0 && getKeepGoing() == true) {
            returnError = mqReadMessage(&msg);

            if (returnError < 0) {
                LOG("[PostmanLOG] Can't read the message in the queue ... Re set up the queue.%s", "\n");
                returnError = setUpMq();

                if (returnError < 0) {
                    LOG("[PostmanLOG] Can't re set-up the message queue ... Stop the processus.%s", "\n");
                    setKeepGoing(false);
                } else {
                    returnError = mqReadMessage(&msg);
                    if (returnError < 0) {
                        LOG("[PostmanLOG] Can't read the message in the queue ... Stop the processus.%s", "\n");
                        setKeepGoing(false);
                    }
                }
            }
        }

        if (returnError >= 0 && getKeepGoing() == true) {
            if (msg.flag == SEND) {
                if (getConnectionState() == CONNECTED) {
                    returnError = socketSendMessage(msg.trame, msg.size);
                    if (returnError < 0) {
                        LOG("[PostmanLOG] Can't read the message in the socket ... Re set up the socket%s", "\n");
                        tearDownSocket();
                        returnError = setUpSocket();

                        if (returnError < 0) {
                            LOG("[PostmanLOG] Can't re set up the socket ... Stop the processus%s", "\n");
                            setKeepGoing(false);
                        } else {
                            returnError = socketSendMessage(msg.trame, msg.size);
                            if (returnError < 0) {
                                LOG("[PostmanLOG] Can't read message in the socket ... Stop the processus%s", "\n");
                                setKeepGoing(false);
                            }
                        }
                    }
                    free(msg.trame);
                }
            } else if (msg.flag == STOP) {
                setKeepGoing(false);
            } else if (msg.flag != WAKE_UP) {
                LOG("[PostmanLOG] Unknown message in the message queue, flag's value: %d ... Ignore it.%s", msg.flag, "\n");
            }
        }
    }

    LOG("[PostmanLOG] Server is down, can't send more message%s", "\n");

    return NULL;
}
