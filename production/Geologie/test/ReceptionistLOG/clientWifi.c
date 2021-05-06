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

#define DEFAULT_IP "localhost" 
#define DEFAULT_PORT (12345)
#define MAX_CONNECTION_ATTEMPT (10)

typedef struct {
    int integer;
    char data[15];
}Data;

static int setServerAdress(void);
static int createSocket(void);
static int closeSocket(void);
static int connection(void);
static int run(void);
static int readMsg(Data* p_data);
static void displayMsg(Data* p_data);
static void intHandler(int p_dummy);

static char s_ip[16]; // 15 => Size of 000.000.000.000 + EOF
static int s_port;
static struct sockaddr_in s_serverAdress;
static int s_socket;
static int s_keepGoing = 1;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn static void intHandler(int p_dummy)
 * @brief Met s_keepGoing a 0 pour 
 */
static void intHandler(int p_dummy) {
    s_keepGoing = 0;
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
    int l_returnValue = EXIT_FAILURE;
    signal(SIGINT, intHandler);                             /// SIGINT qd on fait un ctr c
                                                            /// appelle intHandler et met s_keepGoing a 0

    if (argc > 1) {                 ///si on met en parametre une ip on va la choisir
        sprintf(s_ip, argv[1]);     /// on assigne l'ip prise en paramètre à l'adresse ip du socket
        s_port = atoi(argv[2]); 
    } else {                        /// sinon on utilise l'ip de base
        sprintf(s_ip, DEFAULT_IP);
        s_port = DEFAULT_PORT;
    }

    printf("Try to reach %s on the port %d\n", s_ip, s_port);

    l_returnValue = setServerAdress();                          ///on configure les paramètres de la socket

    if (l_returnValue == EXIT_SUCCESS && s_keepGoing) {
        l_returnValue = createSocket();                         ///on créer la socket

        if (l_returnValue == EXIT_SUCCESS && s_keepGoing) {
            l_returnValue = connection();                       ///on connecte la socket

            if (l_returnValue == EXIT_SUCCESS && s_keepGoing) {
                l_returnValue = run();                          ///on run l'échange de données

            }
        }

        if (l_returnValue != EXIT_SUCCESS) {
            closeSocket(); // By not saving the returned value, we avoid forgetting the error
        } else {
            l_returnValue += closeSocket();
        }
    }

    return l_returnValue;
}

/**
 * @fn static int setServerAdress(void)
 * @brief Permet de set une autre adresse ip du socket que celle par defaut
 */
static int setServerAdress(void) {
    const struct hostent* l_hostInfo = gethostbyname(s_ip);    /// gethostbyname ??  TODO
    int l_returnValue = EXIT_FAILURE;

    if (l_hostInfo == NULL) {                                   /// si l_hostInfo==NULL host pas atteignalble, erreur
        printf("%sThe host is not reachable%s\n", "\033[41m", "\033[0m");
    } else {
        s_serverAdress.sin_port = htons(s_port); ///on met le port du serveur dans les parametre pr créer le socket
        s_serverAdress.sin_family = AF_INET;
        s_serverAdress.sin_addr = *((struct in_addr*) l_hostInfo->h_addr_list[0]); /// TODO

        l_returnValue = EXIT_SUCCESS;
    }

    return l_returnValue;
}

/**
 * @fn static int createSocket(void) 
 * @brief permet de créer le socket client
 */
static int createSocket(void) {
    s_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);       ///on choisit un socket TCP (SOCK_STREAM) sur IP (AF_INET)
                                                                /// IPPROTO_TCP ????? TODO quasiment pareil
    int l_returnValue = EXIT_FAILURE;                           


    if (s_socket < 0) {                                         /// est a -1 si erreur, socket pas créé
        printf("%sError socket%s\n", "\033[41m", "\033[0m");
    } else {
        l_returnValue = EXIT_SUCCESS;
    }

    return l_returnValue;
}
/**
 * @fn static int closeSocket(void)
 * @brief ferme le socket client
 * 
 */
static int closeSocket(void) {
    int l_returnValue = close(s_socket);

    if (l_returnValue < 0) {
        l_returnValue = EXIT_FAILURE;
    } else {
        l_returnValue = EXIT_SUCCESS;
    }

    return l_returnValue;
}

/**
 * @fn static int connection(void)
 * @brief Se conncete au à l'adresse du socket du serveur
 * 
 */
static int connection(void) {
    printf("%sConnection tent to the server%s\n\n", "\033[34m", "\033[0m");

    int l_attemptCounter = 0;                                                                           ///compteur de tentatives de connexions    
    int l_returnValue = EXIT_FAILURE;                                                                   

    while (l_attemptCounter < MAX_CONNECTION_ATTEMPT) {
        l_attemptCounter++;

        if (connect(s_socket, (struct sockaddr*) &s_serverAdress, sizeof(s_serverAdress)) < 0) {        ///connexion à l'adresse de la socket
            printf("%s%s%sConnection failure, attempt %d / %d%s\n",
                "\033[1A", "\033[K", "\033[33m", l_attemptCounter, MAX_CONNECTION_ATTEMPT, "\033[0m"); ///TODO ansy code

            sleep(1); /// Sleep and retry after
        } else {
            break; /// while (l_attemptCounter < MAX_CONNECTION_ATTEMPT)
        }
    }

    if (l_attemptCounter < MAX_CONNECTION_ATTEMPT) {                                                   
        printf("%s%s%sConnection success after %d attemps%s\n\n", "\033[1A", "\033[K", "\033[32m", l_attemptCounter, "\033[0m");
        l_returnValue = EXIT_SUCCESS;
    } else {
        printf("%sLimit on the number of connection attempts exceeded%s\n", "\033[41m", "\033[0m");
    }

    return l_returnValue;
}

/**
 * @fn static int run(void)
 * @brief Run du client
 * fonction qui va tourner pr faire fonctionner le client
 */

static int run(void) {
    printf("\nPress the enter key to exit the program\n");

    int l_returnValue = EXIT_FAILURE;
    fd_set l_env;

    while (s_keepGoing) {
        /* https://broux.developpez.com/articles/c/sockets/#LV-C-2 */
        /* https://www.blaess.fr/christophe/2013/12/27/comprendre-le-fonctionnement-de-select/ */

        /// on lui psse l_env une liste a surveiller pour pas que le thread tourne en permanence

        FD_ZERO(&l_env);                                                                /// Pas compris TODO, on la remet à 0
        FD_SET(STDIN_FILENO, &l_env);                                                   /// Pas compris //on demande a surveiller STDIN_FILENO, entrée standard pour le terminal
        FD_SET(s_socket, &l_env);                                                       /// Pas compris //on demande a surveiller s_socket, si qqn touche au socket

        
        if (select(FD_SETSIZE, &l_env, NULL, NULL, NULL) == -1) {                       //select dans dans un thread met en pause le thread, attend qu'une condition soit faite pr le reveiller
            printf("%sError with %sselect()%s\n", "\033[41m", "\033[21m", "\033[0m");
            break;
        }
    
        if (FD_ISSET(STDIN_FILENO, &l_env)) {   ///Regarder ui a été réveillé ? regarder si STDIN_FILENO a reveiller le thread
            /* Keyboard input */
            printf("%sKeyboard input, close the program%s\n", "\033[43m", "\033[0m");
            l_returnValue = EXIT_SUCCESS;
            break;

        } else if (FD_ISSET(s_socket, &l_env)) { /// regardrer si le socket a été touché, si on a recu de l'info
            Data l_message;
            l_returnValue = readMsg(&l_message);

            if (l_returnValue != EXIT_SUCCESS) {
                break;
            } else {
                displayMsg(&l_message);
            }
        }
    }

    return l_returnValue;
}

/**
 * @fn static int readMsg(Data* p_data)
 * @brief Lis le message sur le socket
 * 
 * @param Data Donnee contenant les informations qui ont ete recues
 */

//si on se déconnecte on le lit avec select et on arrive ici et le read retournera 0, ce qui veut dire qu'on est déconnecté
static int readMsg(Data* p_data) {
    int l_returnValue = EXIT_FAILURE;

    unsigned int l_quantityReaddean = 0; /// TODO a commenter
    unsigned int l_quantityToRead = sizeof(Data);

    while (l_quantityToRead > 0) {
        l_quantityReaddean = read(s_socket, p_data + l_quantityReaddean, l_quantityToRead); ///read ,e garantit pas quil va lire tous les bits
                                                                                            /// un octet une adresse mémoire
                                                                                            /// si read retourne 0 : deconnecter
                                                                                            // si read retourne 0 : erreur
        
        if (l_quantityReaddean < 0) {   
            printf("%sError when receiving the message%s\n\n", "\033[41m", "\033[0m");
            break; // l_quantityWritten

        } else if (l_quantityReaddean == 0) {
            printf("%s%sServer is disconnect%s\n\n", "\033[43m", "\033[30m", "\033[0m");
            closeSocket();
            break; // l_quantityWritten

        } else {
            l_quantityToRead -= l_quantityReaddean;
        }
    }

    if (l_quantityToRead == 0) { //convertir les message pour mieux les afficher, bien
        // NOTE Think to convert correctly int value (ntohl)
        l_returnValue = EXIT_SUCCESS;
    }

    return l_returnValue;
}

static void displayMsg(Data* p_data) {
    // TODO
    static int l_counter = 1;
    printf("Get a new message %d\n", l_counter);
    l_counter++;
}
