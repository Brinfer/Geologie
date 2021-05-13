/**
 * @file receptionistLOG.c
 *
 * @brief Permet d'envoyer des informations à travers les sockets de manire continue.
 *
 * @version 1.0.1
 * @date 5 mai 2021
 * @author BRIENT Nathan
 * @copyright BSD 2-clauses
 *
 */

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////

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
#include "config.h"
/////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////

//#define IP_SERVER "192.168.1.74" //Adresse en wlan0 de la carte
#define IP_SERVER "localhost" //Adresse du pc (pour tester)
//#define IP_SERVER "192.168.7.1" //Adresse en usb0 de la carte

#define PORT_SERVER (12345)
#define NO_CLIENT_SOCKET_VALUE (-1)
#define DATA_LENGTH 16



static int keepGoing = 1; // TODO protect with mutex
static int socketListen;
static struct sockaddr_in serverAddress;
static int socketClient;
static pthread_t socketThreadWrite;
static pthread_t socketThreadListen;
static pthread_mutex_t mutexSocket = PTHREAD_MUTEX_INITIALIZER;

static void intHandler(int _);
static int createSocketLOG(void);
static void configureServerAdressLOG(void);
static int startServerLOG(void);
static void* receiveFromClient(void* _);
static int connectToClient(void);
static int readMsg(Data* data);
static int disconnectToClient();
static void* sendToClientSocket(void* _);
static int sendMsg(const Data* dataToSend, int taille);

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////


/**
 * @fn static void intHandler(int _)
 * @brief fonction permettant de mettre a jour la variable keepGoing
 * si on a une erreur, on la mettra a 0
 *
 * @param
 */
static void intHandler(int _) { // _ pour dire que ca sera ignore
    keepGoing = 0;
}

/**
 * @fn static int createSocketLOG(void)
 * @brief cration du socket sur serveurLOG
 *
 */
static int createSocketLOG(void) {
    socketListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int returnValue = EXIT_FAILURE;

    if (socketListen < 0) {
        PRINT("%sError socket%s\n", "\033[41m", "\033[0m");
    } else {
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
    serverAddress.sin_family = AF_INET;                               // Type d'adresse = IP
    serverAddress.sin_port = htons(PORT_SERVER);                      // Port TCP ou le service est accessible
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);                // On s'attache a toutes les interfaces
}


/**
 * @fn static int startServerLOG(void)
 * @brief demarrage du socket
 *
 */

static int startServerLOG(void) {
    PRINT("%s\n","dans startServerLOG");
    int returnValue = EXIT_FAILURE;

    PRINT("%sThe server is serving on port %d at the address %s%s\n",
        "\033[32m", PORT_SERVER, IP_SERVER, "\033[0m");
    PRINT("%sTo shutdown the server press enter%s\n\n", "\033[36m", "\033[0m");

    bind(socketListen, (struct sockaddr*) &serverAddress, sizeof(serverAddress));   // On attache le socket a l'adresse indiquee

    if (listen(socketListen, 1) < 0) {                                    // on met le socket en ecoute et on accepte que MAX_PENDING connexions
        PRINT("%sError while listenning the port%s\n", "\033[41m", "\033[0m");
    } else {
        returnValue = EXIT_SUCCESS;
    }

    return returnValue;
}
/**
 * @fn static void* receiveFromClient(void* _) {
 * @brief methode routine qui va tourner en boucle pour recevoir lire le socket
 *
 * @param argc premier argument correspondant au nombre d'arguments
 * @param argv arguments
 */

static void* receiveFromClient(void* _) {
    PRINT("%s\n","dans receiveFromClient");

    int returnValue = EXIT_FAILURE;
    fd_set env;
    Data data[60]={0}; //normalement c'est la taille maximale du tableau recu

    while (keepGoing) {
        /* https://broux.developpez.com/articles/c/sockets/#LV-C-2 */
        /* https://www.blaess.fr/christophe/2013/12/27/comprendre-le-fonctionnement-de-select/ */

        FD_ZERO(&env);
        FD_SET(STDIN_FILENO, &env);
        FD_SET(socketListen, &env);

        pthread_mutex_lock(&mutexSocket);
        int socketClientValue = socketClient;
        pthread_mutex_unlock(&mutexSocket);
        if (socketClientValue != NO_CLIENT_SOCKET_VALUE) {
            FD_SET(socketClientValue, &env);
        }


        if (select(FD_SETSIZE, &env, NULL, NULL, NULL) == -1) {
            PRINT("%sError with %sselect()%s\n", "\033[41m", "\033[21m", "\033[0m");
            break; //keepGoing
        }

        if (FD_ISSET(STDIN_FILENO, &env)) {
            PRINT("\n%sAsk for exit%s\n", "\033[41m", "\033[0m");
            returnValue = EXIT_SUCCESS;
            break; //keepGoing
        } else if (FD_ISSET(socketListen, &env)) {
            returnValue = connectToClient();
            if (returnValue != EXIT_SUCCESS) {
                break; //keepGoing
            }
        } else {
            pthread_mutex_lock(&mutexSocket);
            int socketClientValue = socketClient;
            pthread_mutex_unlock(&mutexSocket);
            if (socketClientValue != NO_CLIENT_SOCKET_VALUE) {
                if (FD_ISSET(socketClientValue, &env)) {
                    returnValue=readMsg(data);
                    break; //i
                }
            }

        }
        
    }

    /*
    pthread_mutex_lock(&mutexSocket);
    int socketClientValue = socketClient;
    pthread_mutex_unlock(&mutexSocket);
    
    
    */
    int socketClientValue = socketClient;

    if (socketClientValue != NO_CLIENT_SOCKET_VALUE) {
        //returnValue += disconnectToClient();
    }
    pthread_exit(&returnValue);
}

/**
 * @fn static int connectToClient(void)
 * @brief Connection au client
 *
 */

static int connectToClient(void) {
    PRINT("%s\n","dans connectToClient");

    int returnValue = EXIT_FAILURE;

    pthread_mutex_lock(&mutexSocket);
    int socketClientValue = socketClient;
    pthread_mutex_unlock(&mutexSocket);
    if (socketClientValue == NO_CLIENT_SOCKET_VALUE) {
        PRINT("%sError when connecting the client%s\n", "\033[41m", "\033[0m");
    }


    int socketValue = accept(socketListen, NULL, 0);

    if (socketValue < 0) {
        PRINT("%sError when connecting the client%s\n", "\033[41m", "\033[0m");
    } else {
        PRINT("%sConnection of a client%s\n", "\033[42m", "\033[0m");
        returnValue = EXIT_SUCCESS;
        pthread_mutex_lock(&mutexSocket);
        socketClient = socketValue; //in case of the Macro value is not -1
        pthread_mutex_unlock(&mutexSocket);
        returnValue = EXIT_SUCCESS;

    }
    Data connectionData[8] = "connect";
    sendMsg(connectionData, 8); //un tableau est un pointeur, pas besoin mettre &
    //PRINT("%s \n","connecte");
    return returnValue;
}

/**
 * @fn static int readMsg(Data* data)
 * @brief mthode pour lire contenu socket
 *
 */
static int readMsg(Data* data) {
    PRINT("%s\n","dans readMsg");

    int returnValue = EXIT_SUCCESS;



    //int quantityReaddean = 0;
    int quantityToRead = sizeof(data);

    //while (quantityToRead > 0) {
    pthread_mutex_lock(&mutexSocket);
    read(socketClient, data, quantityToRead);
    pthread_mutex_unlock(&mutexSocket);
    PRINT("%s\n",data);

/*
        if (quantityReaddean < 0) {
            PRINT("%sError when receiving the message%s\n\n", "\033[41m", "\033[0m");
            break; //quantityWritten
        } else if (quantityReaddean == 0) {
            //Client leave
            disconnectToClient();
            returnValue = EXIT_SUCCESS;
            break; //quantityWritten
        } else {
            quantityToRead -= quantityReaddean;
        }
    }

    if (quantityToRead == 0) {
        //Client send a message, ignore it
        returnValue = EXIT_SUCCESS;
    }
    */
    return returnValue;
}

/**
 * @fn static int disconnectToClient()
 * @brief mthode pour se dconnecter d'un client
 *
 * @param clientIndex correspond à l'index du client (2 clients max)
 */
static int disconnectToClient() {
    PRINT("%s\n","dans readMsg");

    pthread_mutex_lock(&mutexSocket);
    int returnValue = close(socketClient);
    pthread_mutex_unlock(&mutexSocket);

    if (returnValue < 0) {
        returnValue = EXIT_FAILURE;
    } else {
        PRINT("%s%sClient is disconnect%s\n\n", "\033[43m", "\033[30m", "\033[0m");
        pthread_mutex_lock(&mutexSocket);
        socketClient = NO_CLIENT_SOCKET_VALUE;
        pthread_mutex_unlock(&mutexSocket);

        returnValue = EXIT_SUCCESS;
    }

    return returnValue;
}


// s'attend à recevoir un pointeur
// static void* sendToClientSocket(void* _)

/**
 * @fn static void sendToClientSocket(void)
 * @brief mthode pour envoyer des messages au client de facon repetitive
 *
 */

static void* sendToClientSocket(void* _) {            //tant que le prog fonctionne on continue
    int returnValue = EXIT_FAILURE;

    Data spamData[50] = "spam";                                  //donnee que l'on va spam

    while (keepGoing) {
        pthread_mutex_lock(&mutexSocket);         // protege acces a tableau en lecture
        int socketClientValue = socketClient;
        pthread_mutex_unlock(&mutexSocket);

        if (socketClientValue != NO_CLIENT_SOCKET_VALUE) {  // si pas client on envoie pas message
            returnValue = sendMsg(spamData, 50);                                   // on envoie un message au client i
        }

        if (returnValue != EXIT_SUCCESS) {
            break; //keepGoing
        }

        sleep(1);
    }

    pthread_exit(&returnValue);
}

/**
 * @fn static void sendToClientSocket(void)
 * @brief mthode pour envoyer des messages au client
 *
 * @param clientIndex correspond à l'index du client auquel on va envoyer (2 clients max)
 */

static int sendMsg(const Data* dataToSend, int taille) { //dataToSend etant adresse de notre tableau
    PRINT("%s \n", "dans sendMsg");

    int returnValue = EXIT_SUCCESS; /**< variable pour gestion erreur */


    //int quantityWritten = 0;
    int quantityToWrite = taille;


    //while (quantityToWrite > 0) {   //pas garantie qu'on creer tous les octets
    pthread_mutex_lock(&mutexSocket);
    write(socketClient, dataToSend, quantityToWrite);
    pthread_mutex_unlock(&mutexSocket);
/*
        if (quantityWritten < 0) {
            PRINT("%sError when sending the message %s\n", "\033[41m", "\033[0m");
            break; //quantityWritten
        } else {
            quantityToWrite -= quantityWritten;
        }
    }

    if (quantityToWrite == 0) {
        returnValue = EXIT_SUCCESS;
    }
    */
    return returnValue;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////


extern int ReceptionistLOG_new(void) {                                                                // configuration socket
    PRINT("%s \n", "dans ReceptionistLOG_new");

    int returnValue = EXIT_FAILURE;

    returnValue = createSocketLOG();                                                         // on crer le socket : AF_INET = IP, SOCK_STREAM = TCP
    if (returnValue == EXIT_SUCCESS) {
        configureServerAdressLOG();
    }
    return returnValue;
}


extern int ReceptionistLOG_start(void) {
    PRINT("%s \n", "dans ReceptionistLOG_start");
    int returnValue = EXIT_FAILURE;
    signal(SIGINT, intHandler);                                                             // Si pb avec ctr c

    returnValue = startServerLOG();                                                      // dmarrage du socket et mise en coute
    if (returnValue == EXIT_SUCCESS) {
        pthread_mutex_init(&mutexSocket, NULL);                 //initialisation                       // peut etre pas utile, besoin protger socket en lecture criture
        returnValue = pthread_create(&socketThreadWrite, NULL, &sendToClientSocket, NULL);
        // premier thread pr envoyer

        if (returnValue == EXIT_SUCCESS) {
            returnValue = pthread_create(&socketThreadListen, NULL, &receiveFromClient, NULL);
            // premier thread pr recevoir
            //sendToClientSocket();
            if (returnValue != EXIT_SUCCESS) {
                keepGoing = 1;
            }
        }
    }


    void* returnValueThread;     //TODO
    returnValueThread = malloc(sizeof(int));
    PRINT("%s \n", "on ferme les threads");
    pthread_join(socketThreadListen, returnValueThread);
    returnValue += *(int*) returnValueThread;

    pthread_join(socketThreadWrite, returnValueThread);
    returnValue += *(int*) returnValueThread;
    
    pthread_mutex_destroy(&mutexSocket);

    return returnValue;
}

// read et set dans deux thread diffrent est il necessaire de verouiller les mutex pour pouvoir crire et lire
// risque qu'on crive et lise sur le meme client en mm tps
// est il necessaire de proteger par des mutex les sockets
// est ce que while pr verifier tout vrfier tous les octet ncessaire
