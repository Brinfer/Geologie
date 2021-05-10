/**
 * @file receptionistLOG.c
 *
 * @brief Permet d'envoyer des informations à travers les sockets de manière continue.
 *
 * @version 1.0.1
 * @date 5 mai 2021
 * @author BRIENT Nathan
 * @copyright BSD 2-clauses
 *
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>

#include "tools.h"
#include "receptionistLOG.h"
#include <string.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///#define IP_SERVER "192.168.1.74" //Adresse en wlan0 de la carte
///#define IP_SERVER "localhost" //Adresse du pc (pour tester)
#define IP_SERVER "192.168.7.1" //Adresse en usb0 de la carte

#define PORT_SERVER (12345)
#define NO_CLIENT_SOCKET_VALUE (-1)
#define DATA_LENGTH 16


typedef char Data[DATA_LENGTH];                         ///TODO changer nom structure

static int keepGoing = 1; /// TODO protect with mutex
static int socketListen;
static struct sockaddr_in serverAddress;
static int socketClient;
// static pthread_t spamThread;
static pthread_t socketThread;
static pthread_mutex_t mutexSocket = PTHREAD_MUTEX_INITIALIZER;

static void intHandler(int _);
static int createSocketLOG(void);
static void configureServerAdressLOG(void);
static int startServerLOG(void);
static void* runLOG(void* _);
static int connectToClient(void);
static int readMsg();
static int disconnectToClient();
static void spamClientSocket(void);
static int sendMsg(Data dataToSend);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * @fn static void intHandler(int _)
 * @brief fonction permettant de mettre a jour la variable keepGoing
 * si on a une erreur, on la mettra a 0
 *
 * @param
 */
static void intHandler(int _) { /// _ pour dire que ca sera ignore
    keepGoing = 0;
}

/**
 * @fn static int createSocketLOG(void)
 * @brief création du socket sur serveurLOG
 *
 */
static int createSocketLOG(void) {
    socketListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int returnValue = EXIT_FAILURE;

    if (socketListen < 0)     {
        PRINT("%sError socket%s\n", "\033[41m", "\033[0m");
    }     else     {
        returnValue = EXIT_SUCCESS;
    }

    return returnValue;
}

/**
 * @fn static void configureServerAdressLOG(void)
 * @brief configure le serveur, les ports, ...
 *
 */
static void configureServerAdressLOG(void) {
    serverAddress.sin_family = AF_INET;                               /// Type d'adresse = IP
    serverAddress.sin_port = htons(PORT_SERVER);                      /// Port TCP ou le service est accessible
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);                /// On s'attache a toutes les interfaces
}


/**
 * @fn static int startServerLOG(void)
 * @brief demarrage du socket
 *
 */

static int startServerLOG(void) {
    int returnValue = EXIT_FAILURE;

    PRINT("%sThe server is serving on port %d at the address %s%s\n",
        "\033[32m", PORT_SERVER, IP_SERVER, "\033[0m");
    PRINT("%sTo shutdown the server press enter%s\n\n", "\033[36m", "\033[0m");

    bind(socketListen, (struct sockaddr*) &serverAddress, sizeof(serverAddress));   /// On attache le socket a l'adresse indiquee

    if (listen(socketListen,1) < 0)     {                                    /// on met le socket en ecoute et on accepte que MAX_PENDING connexions
        PRINT("%sError while listenning the port%s\n", "\033[41m", "\033[0m");
    }     else     {
        returnValue = EXIT_SUCCESS;
    }

    return returnValue;
}
/**
 * @fn static void* runLOG(void* _) {
 * @brief méthode qui va tourner en boucle pour recevoir lire le socket
 *
 * @param argc premier argument correspondant au nombre d'arguments
 * @param argv arguments
 */

static void* runLOG(void* _) {
    int returnValue = EXIT_FAILURE;
    fd_set env;

    while (keepGoing)     {
        /* https://broux.developpez.com/articles/c/sockets/#LV-C-2 */
        /* https://www.blaess.fr/christophe/2013/12/27/comprendre-le-fonctionnement-de-select/ */

        FD_ZERO(&env);
        FD_SET(STDIN_FILENO, &env);
        FD_SET(socketListen, &env);

        pthread_mutex_lock(&mutexSocket);
        int socketClientValue = socketClient;
        pthread_mutex_unlock(&mutexSocket);
        if (socketClientValue != NO_CLIENT_SOCKET_VALUE){
            FD_SET(socketClientValue, &env);
        }


        if (select(FD_SETSIZE, &env, NULL, NULL, NULL) == -1)         {
            PRINT("%sError with %sselect()%s\n", "\033[41m", "\033[21m", "\033[0m");
            break; ///keepGoing
        }

        if (FD_ISSET(STDIN_FILENO, &env))         {
            PRINT("\n%sAsk for exit%s\n", "\033[41m", "\033[0m");
            returnValue = EXIT_SUCCESS;
            break; ///keepGoing
        }
        else if (FD_ISSET(socketListen, &env))         {
            returnValue = connectToClient();
            if (returnValue != EXIT_SUCCESS)             {
                break; ///keepGoing
            }
        } else         {
            pthread_mutex_lock(&mutexSocket);
            int socketClientValue = socketClient;
            pthread_mutex_unlock(&mutexSocket);
            if (socketClientValue != NO_CLIENT_SOCKET_VALUE)                 {
                if (FD_ISSET(socketClientValue, &env))                     {
                    readMsg();
                       break; ///i
                }
            }

        }
    }

    pthread_mutex_lock(&mutexSocket);
    int socketClientValue = socketClient;
    pthread_mutex_unlock(&mutexSocket);
    if (socketClientValue != NO_CLIENT_SOCKET_VALUE)         {
        returnValue += disconnectToClient();
    }

    pthread_exit(&returnValue);
}

/**
 * @fn static int connectToClient(void)
 * @brief Connection au client
 *
 */

static int connectToClient(void) {
    int returnValue = EXIT_FAILURE;

    pthread_mutex_lock(&mutexSocket);
    int socketClientValue = socketClient;
    pthread_mutex_unlock(&mutexSocket);
    if (socketClientValue == NO_CLIENT_SOCKET_VALUE)         {
        PRINT("%sError when connecting the client%s\n", "\033[41m", "\033[0m");
    }


    int socketValue = accept(socketListen, NULL, 0);

    if (socketValue < 0)         {
        PRINT("%sError when connecting the client%s\n", "\033[41m", "\033[0m");
    }         else         {
        PRINT("%sConnection of a client%s\n", "\033[42m", "\033[0m");
        returnValue = EXIT_SUCCESS;
        pthread_mutex_lock(&mutexSocket);
        socketClient = socketValue; ///in case of the Macro value is not -1
        pthread_mutex_unlock(&mutexSocket);
        returnValue = EXIT_SUCCESS;

    }
    Data connectionData="connect";
    sendMsg(connectionData);
    //PRINT("%s \n","connecte");
    return returnValue;
}

/**
 * @fn static int readMsg()
 * @brief méthode pour lire contenu socket
 *
 * @param clientIndex correspond à l'index du client (2 clients max)
 */
static int readMsg() {
    int returnValue = EXIT_FAILURE;

    Data data;

    int quantityReaddean = 0;
    int quantityToRead = sizeof(Data);

    while (quantityToRead > 0)     {
        pthread_mutex_lock(&mutexSocket);
        quantityReaddean = read(socketClient, &data + quantityReaddean, quantityToRead);
        pthread_mutex_unlock(&mutexSocket);

        if (quantityReaddean < 0)         {
            PRINT("%sError when receiving the message%s\n\n", "\033[41m", "\033[0m");
            break; ///quantityWritten
        }         else if (quantityReaddean == 0)         {
            /* Client leave */
            disconnectToClient();
            returnValue = EXIT_SUCCESS;
            break; ///quantityWritten
        }         else         {
            quantityToRead -= quantityReaddean;
        }
    }

    if (quantityToRead == 0)     {
        /* Client send a message, ignore it */
        returnValue = EXIT_SUCCESS;
    }

    return returnValue;
}

/**
 * @fn static int disconnectToClient()
 * @brief méthode pour se déconnecter d'un client
 *
 * @param clientIndex correspond à l'index du client (2 clients max)
 */
static int disconnectToClient() {
    pthread_mutex_lock(&mutexSocket);
    int returnValue = close(socketClient);
    pthread_mutex_unlock(&mutexSocket);

    if (returnValue < 0)     {
        returnValue = EXIT_FAILURE;
    }     else     {
        PRINT("%s%sClient is disconnect%s\n\n", "\033[43m", "\033[30m","\033[0m");
        pthread_mutex_lock(&mutexSocket);
        socketClient = NO_CLIENT_SOCKET_VALUE;
        pthread_mutex_unlock(&mutexSocket);

        returnValue = EXIT_SUCCESS;
    }

    return returnValue;
}


/// s'attend à recevoir un pointeur
/// static void* spamClientSocket(void* _)

/**
 * @fn static void spamClientSocket(void)
 * @brief méthode pour envoyer des messages au client de facon repetitive
 *
 */

static void spamClientSocket(void) {            ///tant que le prog fonctionne on continue
    int returnValue = EXIT_FAILURE;

    Data spamData="spaaaaam";                                  ///donnee que l'on va spam

    while (keepGoing)     {
        pthread_mutex_lock(&mutexSocket);         /// protege acces a tableau en lecture
        int socketClientValue = socketClient;
        pthread_mutex_unlock(&mutexSocket);

        if (socketClientValue != NO_CLIENT_SOCKET_VALUE)             {  /// si pas client on envoie pas message
            returnValue = sendMsg(spamData);                                   /// on envoie un message au client i
        }

        if (returnValue != EXIT_SUCCESS)             {
            break; ///keepGoing
        }

        sleep(1);
    }

    pthread_exit(&returnValue);
}

/**
 * @fn static void spamClientSocket(void)
 * @brief méthode pour envoyer des messages au client
 *
 * @param clientIndex correspond à l'index du client auquel on va envoyer (2 clients max)
 */

static int sendMsg(Data dataToSend) {
    int returnValue = EXIT_FAILURE;

    Data data;
    int quantityWritten = 0;
    int quantityToWrite = sizeof(data);
    sprintf(data,"%s", dataToSend); //la data a envoyer dans une data

    while (quantityToWrite > 0)     {   ///pas garantie qu'on écrit tous les octets
        pthread_mutex_lock(&mutexSocket);
        quantityWritten = write(socketClient, &data + quantityWritten, quantityToWrite);
        pthread_mutex_unlock(&mutexSocket);

        if (quantityWritten < 0)         {
            PRINT("%sError when sending the message %s\n", "\033[41m", "\033[0m");
            break; ///quantityWritten
        }         else         {
            quantityToWrite -= quantityWritten;
        }
    }

    if (quantityToWrite == 0)     {
        returnValue = EXIT_SUCCESS;
    }
    return returnValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn int ReceptionistLOG_new()
 * @brief fonction qui créer le socket
 *
 */
extern int ReceptionistLOG_new(void) {                                                                /// configuration socket
    int returnValue = EXIT_FAILURE;

    returnValue = createSocketLOG();                                                         /// on créer le socket : AF_INET = IP, SOCK_STREAM = TCP
    if (returnValue == EXIT_SUCCESS)     {
        configureServerAdressLOG();
    }
    return returnValue;
}

/**
 * @fn int ReceptionistLOG_start()
 * @brief fonction principale de receptionistLOG.c qui sera appelee de l'exterieur
 *  pour l'instant
 */
extern int ReceptionistLOG_start(void) {
    int returnValue = EXIT_FAILURE;
    signal(SIGINT, intHandler);                                                             /// Si pb avec ctr c

    returnValue = startServerLOG();                                                      /// démarrage du socket et mise en écoute
    if (returnValue == EXIT_SUCCESS)         {
        pthread_mutex_init(&mutexSocket, NULL);                 ///initialisation                       /// peut etre pas utile, besoin protéger socket en lecture écriture
        //returnValue = pthread_create(&spamThread, NULL, &spamClientSocket, NULL);
        /// premier thread pr envoyer

        if (returnValue == EXIT_SUCCESS)             {
            returnValue = pthread_create(&socketThread, NULL, &runLOG, NULL);
            /// premier thread pr recevoir
            spamClientSocket();
            if (returnValue != EXIT_SUCCESS)                 {
                keepGoing = 1;
            }
        }
    }


    void* returnValueThread;     ///TODO
    returnValueThread=malloc(sizeof(int));
    pthread_join(socketThread, returnValueThread);
    returnValue += *(int*) returnValueThread;

    pthread_mutex_destroy(&mutexSocket);

    return returnValue;
}

/// read et set dans deux thread différent est il necessaire de verouiller les mutex pour pouvoir écrire et lire
/// risque qu'on écrive et lise sur le meme client en mm tps
/// est il necessaire de proteger par des mutex les sockets
/// est ce que while pr verifier tout vérfier tous les octet nécessaire
