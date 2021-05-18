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

#define IP_SERVER "localhost" //Adresse du pc (pour tester)
//"192.168.7.1" correspond a l'adresse en usb0 de la carte
//"192.168.1.74" correspond a l'adresse en wlan0 de la carte

#define PORT_SERVER (1234)
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
static int readMsg(Data* data);
static int readHeaderMsg(Data* data);
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
 * si on a une erreur, on la mettra a 0 et cela permettra de stopper le programme
 *
 * @param
 */
static void intHandler(int _) { /// _ pour dire que ca sera ignore
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
 * @brief mthode qui va tourner en boucle pour recevoir lire le socket
 *
 * @param argc premier argument correspondant au nombre d'arguments
 * @param argv arguments
 */

static void* runLOG(void* _) {
    int returnValue = EXIT_FAILURE;
    fd_set env;

    Data stopData[] = "stop"; //data sevant a comparer si on recoit un messagge stop

    while (keepGoing) {

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
            if (socketClientValue != NO_CLIENT_SOCKET_VALUE) { //si on a un client de detecte on va read
                if (FD_ISSET(socketClientValue, &env)) {
                    static Data previousData[] = { 0 };
                    Data noData[60] = { 0 };
                    //Data headerData[6]={0}; //en cours d'ecriture, pour lire une partie de la trame et ainsi identifier sa nature, taille ...
                    //returnValue = readHeaderMsg(headerData);
                    Data data[60] = { 0 };             //remise a zero de data
                    returnValue = readMsg(data);
                    sendMsg(data, sizeof(data));
                    if (strncmp(data, stopData, 4) == 0) {        //compare si message est stop (4 premiers caracteres, si oui on arrete
                        //PRINT("%s\n", "il va falloir arreter");
                        disconnectToClient();
                    }

                    if (strncmp(data, noData,10) == 0) { //si deux fois le meme message sans rien, cela veut dire qu'on est deconnecte
                        socketClient = NO_CLIENT_SOCKET_VALUE;
                        PRINT("%s%sOn a recu 2 messages identiques : client surement deconnecte%s\n\n", "\033[43m", "\033[30m", "\033[0m");

                        //disconnectToClient();
                    }
                    strcpy(previousData, data);
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
 * @brief methode permettant de se connecter au client, se mettre a l'ecoute
 *
 */

static int connectToClient(void) {
    int returnValue = EXIT_FAILURE;

    pthread_mutex_lock(&mutexSocket);
    int socketClientValue = socketClient;
    pthread_mutex_unlock(&mutexSocket);
    if (socketClientValue != NO_CLIENT_SOCKET_VALUE) {
        PRINT("%sError when connecting the client, no socket client%s\n", "\033[41m", "\033[0m");
    } else {
        int socketValue = accept(socketListen, NULL, 0);

        if (socketValue < 0) {
            PRINT("%sError when connecting the client, not accepted%s\n", "\033[41m", "\033[0m");
        } else {
            PRINT("%sConnection of a client%s\n", "\033[42m", "\033[0m");
            returnValue = EXIT_SUCCESS;
            pthread_mutex_lock(&mutexSocket);
            socketClient = socketValue; //in case of the Macro value is not -1
            pthread_mutex_unlock(&mutexSocket);
            returnValue = EXIT_SUCCESS;
            Data connectionData[] = "connect";
            sendMsg(connectionData, sizeof(connectionData)); //un tableau est un pointeur, pas besoin mettre &
        }

    }

    return returnValue;
}

/**
 * @fn static int readMsg()
 * @brief mthode pour lire contenu socket
 *
 * @param data chaine qui prendra les donnees reçues
 */
static int readMsg() {
    int returnValue = EXIT_FAILURE;

    Data data;

    int quantityToRead = 60;

    pthread_mutex_lock(&mutexSocket);
    read(socketClient, data, 60); //peut etre changer 60 par sizeof(data)
    pthread_mutex_unlock(&mutexSocket);
    PRINT("la taille de data est %i \n", quantityToRead);
    PRINT("on recoit : %s\n", data);

    return returnValue;
}

/**
 * @fn static int readHeaderMsg(Data* headerData)
 * @brief methode pour lire contenu du socket
 *
 * @param headerData chaine qui prendra les donnees du header de la trame
 */
static int readHeaderMsg(Data* headerData) {
    PRINT("%s\n", "dans readHeaderMsg");

    int returnValue = EXIT_SUCCESS;



    pthread_mutex_lock(&mutexSocket);
    read(socketClient, headerData, sizeof(headerData));
    pthread_mutex_unlock(&mutexSocket);
    PRINT("la taille du header est: %i\n", (int) sizeof(headerData));

    PRINT("on recoit le header: %s\n", headerData);

    return returnValue;
}

/**
 * @fn static int disconnectToClient()
 * @brief mthode pour se dconnecter d'un client
 *
 * @param clientIndex correspond à l'index du client (2 clients max)
 */
static int disconnectToClient() {
    PRINT("%s\n", "dans disconnectClient");
    Data deconnexionData[] = "deconnexion"; //on envoie l'information comme quoi on va se deconnecter
    sendMsg(deconnexionData, sizeof(deconnexionData));

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
 * @brief mthode pour envoyer des messages au client de facon repetitive
 *
 */

static void* sendToClientSocket(void* data) {            //tant que le prog fonctionne on continue
    //Data DataToSend =(Data) _;
    int returnValue = EXIT_FAILURE;
    Data dataToSend[sizeof(data)] = { 0 };

    strcpy(dataToSend, data);
    PRINT("on va envoyer en boucle %s \n", dataToSend);
    while (keepGoing) {
        pthread_mutex_lock(&mutexSocket);         // protege acces a tableau en lecture
        int socketClientValue = socketClient;
        pthread_mutex_unlock(&mutexSocket);

        if (socketClientValue != NO_CLIENT_SOCKET_VALUE) {  // si pas client on envoie pas message
            returnValue = sendMsg(dataToSend, sizeof(dataToSend));                       // on envoie un message au client
            if (returnValue != EXIT_SUCCESS) {
                break; //keepGoing
            }
        }

        if (returnValue != EXIT_SUCCESS)             {
            break; ///keepGoing
        }

        sleep(1);
    }

    pthread_exit(&returnValue);
}

/**
 * @fn static int sendMsg(const Data* dataToSend, const int taille)
 * @brief methode pour envoyer des messages au client
 *
 * @param clientIndex correspond à l'index du client auquel on va envoyer (2 clients max)
 */

static int sendMsg(const Data* dataToSend, const int taille) { //dataToSend etant adresse de notre tableau

    int returnValue = EXIT_SUCCESS; /**< variable pour gestion erreur */

    int quantityToWrite = taille;

    while (quantityToWrite > 0)     {   ///pas garantie qu'on crit tous les octets
        pthread_mutex_lock(&mutexSocket);
        quantityWritten = write(socketClient, &data + quantityWritten, quantityToWrite);
        pthread_mutex_unlock(&mutexSocket);

    pthread_mutex_lock(&mutexSocket);
    write(socketClient, dataToSend, quantityToWrite);
    pthread_mutex_unlock(&mutexSocket);

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
 * @brief fonction qui crer le socket
 *
 */
extern int ReceptionistLOG_new(void) {                                                                /// configuration socket
    int returnValue = EXIT_FAILURE;

    returnValue = createSocketLOG();                                                         /// on crer le socket : AF_INET = IP, SOCK_STREAM = TCP
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
    signal(SIGINT, intHandler);                                                             // Si pb avec ctr c

    returnValue = startServerLOG();
    pthread_mutex_init(&mutexSocket, NULL);                 //initialisation                       // peut etre pas utile, besoin protger socket en lecture ecriture

   // connectToClient();                                                  // dmarrage du socket et mise en coute
    if (returnValue == EXIT_SUCCESS) {
        returnValue = pthread_create(&socketThreadListen, NULL, &receiveFromClient, NULL);
        //thread pr recevoir
        if (returnValue == EXIT_SUCCESS) {
            //returnValue = pthread_create(&socketThreadWrite, NULL, &sendToClientSocket, NULL);
            //thread pr envoyer
            Data test[] = "test";
            ReceptionistLOG_sendMsg(test, sizeof(test)); //on appelle le thread pour envoyer
            //sendToClientSocket();
            //if (returnValue != EXIT_SUCCESS) {
            //    keepGoing = 1;
            //}
        }
    }


    void* returnValueThread;     //TODO
    returnValueThread = malloc(sizeof(int));
    pthread_join(socketThreadListen, returnValueThread);
    returnValue += *(int*) returnValueThread;

    /*
    pthread_join(socketThreadWrite, returnValueThread);
    returnValue += *(int*) returnValueThread;
    */
    disconnectToClient();
    pthread_mutex_destroy(&mutexSocket);

    return returnValue;
}

extern int ReceptionistLOG_sendMsg(Data dataToSend[], int taille) {

    int returnValue = EXIT_FAILURE;
    void* returnValueThread;
    //PRINT("dans receptionistLOG_sendMsg data :%s \n", dataToSend);
    returnValueThread = malloc(sizeof(int));

    returnValue = pthread_create(&socketThreadWrite, NULL, &sendToClientSocket, dataToSend);
    pthread_join(socketThreadWrite, returnValueThread);

    return returnValue;
}

// read et set dans deux thread diffrent est il necessaire de verouiller les mutex pour pouvoir ecrire et lire
// risque qu'on crive et lise sur le meme client en mm tps
// est il necessaire de proteger par des mutex les sockets
// est ce que while pr verifier tout vrfier tous les octet ncessaire
