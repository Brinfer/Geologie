/**
 * @file clientWifi.c
 *
 * @brief Code pour le socket client pour tester le serveur.
 *
 * @version 1.0
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
#include <signal.h>

#include "clientWifi.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEFAULT_IP "192.168.1.74" ///addreesse carte en wlan0 
#define DEFAULT_PORT (12345)
#define MAX_CONNECTION_ATTEMPT (10)

typedef struct {
    int integer;
    unsigned char data[15];
}Data;

static int setServerAdress(void);
static int createSocket(void);
static int closeSocket(void);
static int connection(void);
static int run(void);
static int readMsg(Data* data);
static void displayMsg(Data* data);
static void intHandler(int dummy);

static char ip[16]; // 15 => Size of 000.000.000.000 + EOF
static int port;
static struct sockaddr_in serverAdress;
static int mySocket;
static int keepGoing = 1;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn static void intHandler(int dummy)
 * @brief Met keepGoing a 0 pour 
 */
static void intHandler(int dummy) {
    keepGoing = 0;
}

/**
 * @fn int main(int argc, char* argv[])
 * @brief fonction princiipale de clientWifi.c
 * Elle va permettre de configurer le socket, le crééer, se connecter et 
 *
 * @param argc premier argument correspondant au nombre d'arguments
 * @param argv arguments pris en compte
 */

int main(int argc, char* argv[]) {
    int returnValue = EXIT_FAILURE;
    signal(SIGINT, intHandler);                             /// SIGINT qd on fait un ctr c
                                                            /// appelle intHandler et met keepGoing a 0

    if (argc > 1) {                 ///si on met en parametre une ip on va la choisir
        sprintf(ip, argv[1]);     /// on assigne l'ip prise en paramètre à l'adresse ip du socket
        port = atoi(argv[2]); 
    } else {                        /// sinon on utilise l'ip de base
        sprintf(ip, DEFAULT_IP);
        port = DEFAULT_PORT;
    }

    printf("Try to reach %s on the port %d\n", ip, port);

    returnValue = setServerAdress();                          ///on configure les paramètres de la socket

    if (returnValue == EXIT_SUCCESS && keepGoing) {
        returnValue = createSocket();                         ///on créer la socket

        if (returnValue == EXIT_SUCCESS && keepGoing) {
            returnValue = connection();                       ///on connecte la socket

            if (returnValue == EXIT_SUCCESS && keepGoing) {
                returnValue = run();                          ///on run l'échange de données

            }
        }

        if (returnValue != EXIT_SUCCESS) {
            closeSocket(); // By not saving the returned value, we avoid forgetting the error
        } else {
            returnValue += closeSocket();
        }
    }

    return returnValue;
}

/**
 * @fn static int setServerAdress(void)
 * @brief Permet de set une autre adresse ip du socket que celle par defaut
 */
static int setServerAdress(void) {
    const struct hostent* hostInfo = gethostbyname(ip);    /// gethostbyname ??  TODO
    int returnValue = EXIT_FAILURE;

    if (hostInfo == NULL) {                                   /// si hostInfo==NULL host pas atteignalble, erreur
        printf("%sThe host is not reachable%s\n", "\033[41m", "\033[0m");
    } else {
        serverAdress.sin_port = htons(port); ///on met le port du serveur dans les parametre pr créer le socket
        serverAdress.sin_family = AF_INET;
        serverAdress.sin_addr = *((struct in_addr*) hostInfo->h_addr_list[0]); /// TODO

        returnValue = EXIT_SUCCESS;
    }

    return returnValue;
}

/**
 * @fn static int createSocket(void) 
 * @brief permet de créer le socket client
 */
static int createSocket(void) {
    mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);       ///on choisit un socket TCP (SOCK_STREAM) sur IP (AF_INET)
                                                                /// IPPROTO_TCP ????? TODO quasiment pareil
    int returnValue = EXIT_FAILURE;                           


    if (mySocket < 0) {                                         /// est a -1 si erreur, socket pas créé
        printf("%sError socket%s\n", "\033[41m", "\033[0m");
    } else {
        returnValue = EXIT_SUCCESS;
    }

    return returnValue;
}
/**
 * @fn static int closeSocket(void)
 * @brief ferme le socket client
 * 
 */
static int closeSocket(void) {
    int returnValue = close(mySocket);

    if (returnValue < 0) {
        returnValue = EXIT_FAILURE;
    } else {
        returnValue = EXIT_SUCCESS;
    }

    return returnValue;
}

/**
 * @fn static int connection(void)
 * @brief Se conncete au à l'adresse du socket du serveur
 * 
 */
static int connection(void) {
    printf("%sConnection tent to the server%s\n\n", "\033[34m", "\033[0m");

    int attemptcounter = 0;                                                                           ///compteur de tentatives de connexions    
    int returnValue = EXIT_FAILURE;                                                                   

    while (attemptcounter < MAX_CONNECTION_ATTEMPT) {
        attemptcounter++;

        if (connect(mySocket, (struct sockaddr*) &serverAdress, sizeof(serverAdress)) < 0) {        ///connexion à l'adresse de la socket
            printf("%s%s%sConnection failure, attempt %d / %d%s\n",
                "\033[1A", "\033[K", "\033[33m", attemptcounter, MAX_CONNECTION_ATTEMPT, "\033[0m"); ///TODO ansy code

            sleep(1); /// Sleep and retry after
        } else {
            break; /// while (attemptcounter < MAX_CONNECTION_ATTEMPT)
        }
    }

    if (attemptcounter < MAX_CONNECTION_ATTEMPT) {                                                   
        printf("%s%s%sConnection success after %d attemps%s\n\n", "\033[1A", "\033[K", "\033[32m", attemptcounter, "\033[0m");
        returnValue = EXIT_SUCCESS;
    } else {
        printf("%sLimit on the number of connection attempts exceeded%s\n", "\033[41m", "\033[0m");
    }

    return returnValue;
}

/**
 * @fn static int run(void)
 * @brief Run du client
 * fonction qui va tourner pr faire fonctionner le client
 */

static int run(void) {
    printf("\nPress the enter key to exit the program\n");

    int returnValue = EXIT_FAILURE;
    fd_set env;

    while (keepGoing) {
        /* https://broux.developpez.com/articles/c/sockets/#LV-C-2 */
        /* https://www.blaess.fr/christophe/2013/12/27/comprendre-le-fonctionnement-de-select/ */

        /// on lui psse env une liste a surveiller pour pas que le thread tourne en permanence

        FD_ZERO(&env);                                                                /// Pas compris TODO, on la remet à 0
        FD_SET(STDIN_FILENO, &env);                                                   /// Pas compris //on demande a surveiller STDIN_FILENO, entrée standard pour le terminal
        FD_SET(mySocket, &env);                                                       /// Pas compris //on demande a surveiller socket, si qqn touche au socket

        
        if (select(FD_SETSIZE, &env, NULL, NULL, NULL) == -1) {                       //select dans dans un thread met en pause le thread, attend qu'une condition soit faite pr le reveiller
            printf("%sError with %sselect()%s\n", "\033[41m", "\033[21m", "\033[0m");
            break;
        }
    
        if (FD_ISSET(STDIN_FILENO, &env)) {   ///Regarder ui a été réveillé ? regarder si STDIN_FILENO a reveiller le thread
            /* Keyboard input */
            printf("%sKeyboard input, close the program%s\n", "\033[43m", "\033[0m");
            returnValue = EXIT_SUCCESS;
            break;

        } else if (FD_ISSET(mySocket, &env)) { /// regardrer si le socket a été touché, si on a recu de l'info
            Data message;
            returnValue = readMsg(&message); /// on lit les info du socket et on les met dans message

            if (returnValue != EXIT_SUCCESS) {
                break;
            } else {
                displayMsg(&message);
            }
        }
    }

    return returnValue;
}

/**
 * @fn static int readMsg(Data* data)
 * @brief Lis le message sur le socket
 * 
 * @param Data Donnee contenant les informations qui ont ete recues
 */

//si on se déconnecte on le lit avec select et on arrive ici et le read retournera 0, ce qui veut dire qu'on est déconnecté
static int readMsg(Data* data) {
    int returnValue = EXIT_FAILURE;

    unsigned int quantityReaddean = 0; /// TODO a commenter
    unsigned int quantityToRead = sizeof(Data);

    while (quantityToRead > 0) {
        quantityReaddean = read(mySocket, data + quantityReaddean, quantityToRead); ///read ,e garantit pas quil va lire tous les bits
                                                                                            /// un octet une adresse mémoire
                                                                                            /// si read retourne 0 : deconnecter
                                                                                            // si read retourne 0 : erreur
        
        if (quantityReaddean < 0) {   
            printf("%sError when receiving the message%s\n\n", "\033[41m", "\033[0m");
            break; // l_quantityWritten

        } else if (quantityReaddean == 0) {
            printf("%s%sServer is disconnect%s\n\n", "\033[43m", "\033[30m", "\033[0m");
            closeSocket();
            break; // l_quantityWritten

        } else {
            quantityToRead -= quantityReaddean;
        }
    }

    if (quantityToRead == 0) { //convertir les message pour mieux les afficher, bien
        // NOTE Think to convert correctly int value (ntohl)
        returnValue = EXIT_SUCCESS;
    }

    return returnValue;
}

static void displayMsg(Data* data) {
    // TODO
    static int l_counter = 1;
    printf("Get a new message %d\n", l_counter);
    printf("on a recu la chaine de caractere %s \n", data->data);
        
    printf("On a recu l'entier %i \n", data->integer);

    l_counter++;
}
