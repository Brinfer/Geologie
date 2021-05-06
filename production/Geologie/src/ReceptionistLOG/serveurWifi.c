/**
 * @file serveurWifi.c
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

#include "serveurWifi.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IP_SERVER "localhost" //Adresse en wlan0 de la carte
#define PORT_SERVER (12345)
#define MAX_PENDING (2)
#define NO_CLIENT_SOCKET_VALUE (-1)

#define MAX_IP_LENGTH 16 ///taille max de l'ip

typedef struct {
    int integer;
    char data[15];
} Data;

static int s_keepGoing = 1; /// TODO protect with mutex
static int s_socketListen;
static struct sockaddr_in s_serverAddress;
static int s_socketClient[MAX_PENDING] = { NO_CLIENT_SOCKET_VALUE, NO_CLIENT_SOCKET_VALUE };
static pthread_t s_spamThread;
static pthread_t s_socketThread;
static pthread_mutex_t s_mutexSocket = PTHREAD_MUTEX_INITIALIZER;

static void intHandler(int _);
static int createSocket(void);
static void createServerAdress(void);
static int startServer(void);
static void* run(void* _);
static int connectClient(void);
static int readMsg(const int p_clientIndex);
static int disconnectClient(const int p_clientIndex);
static void spamClientSocket(void);
static int sendMsg(const int p_clientIndex);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn int main(int argc, char* argv[])
 * @brief fonction princiipale de serveurWifi.c
 *
 * @param argc premier argument correspondant au nombre d'arguments
 * @param argv arguments
 */
int main(int argc, char* argv[]) {
    int l_returnValue = EXIT_FAILURE;
    signal(SIGINT, intHandler);                                                             /// Si pb avec ctr c

    l_returnValue = createSocket();                                                         /// on créer le socket : AF_INET = IP, SOCK_STREAM = TCP
    if (l_returnValue == EXIT_SUCCESS)     {
        createServerAdress();                                                               /// configuration socket

        l_returnValue = startServer();                                                      /// démarrage du socket et mise en écoute
        if (l_returnValue == EXIT_SUCCESS)         {
            pthread_mutex_init(&s_mutexSocket, NULL);                 ///initialisation                       /// peut etre pas utile, besoin protéger socket en lecture écriture
            //l_returnValue = pthread_create(&s_spamThread, NULL, &spamClientSocket, NULL);      
            /// premier thread pr envoyer
            
            if (l_returnValue == EXIT_SUCCESS)             {
                l_returnValue = pthread_create(&s_socketThread, NULL, &run, NULL);
            /// premier thread pr recevoir
                spamClientSocket(); 
                if (l_returnValue != EXIT_SUCCESS)                 {
                    s_keepGoing = 1;
                }
            }
        }
    }

    void* l_returnValueThread;
    pthread_join(s_spamThread, &l_returnValueThread);
    l_returnValue += *(int*) l_returnValueThread;

    pthread_join(s_socketThread, l_returnValueThread);
    l_returnValue += *(int*) l_returnValueThread;

    pthread_mutex_destroy(&s_mutexSocket);

    return l_returnValue;
}

/**
 * @fn static void intHandler(int _)
 * @brief fonction 
 *
 * @param 
 */
static void intHandler(int _) {
    s_keepGoing = 0;
}

/**
 * @fn static int createSocket(void)
 * @brief création du socket serveur
 *
 */
static int createSocket(void) {
    s_socketListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int l_returnValue = EXIT_FAILURE;

    if (s_socketListen < 0)     {
        printf("%sError socket%s\n", "\033[41m", "\033[0m");
    }     else     {
        l_returnValue = EXIT_SUCCESS;
    }

    return l_returnValue;
}

static void createServerAdress(void) {
    s_serverAddress.sin_family = AF_INET;                               /// Type d'adresse = IP 
    s_serverAddress.sin_port = htons(PORT_SERVER);                      /// Port TCP ou le service est accessible
    s_serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);                /// On s'attache a toutes les interfaces
}


/**
 * @fn static int startServer(void)
 * @brief demarrage du socket
 *
 */

static int startServer(void) {
    int l_returnValue = EXIT_FAILURE;

    printf("%sThe server is serving on port %d at the address %s%s\n",
        "\033[32m", PORT_SERVER, IP_SERVER, "\033[0m");
    printf("%sTo shutdown the server press enter%s\n\n", "\033[36m", "\033[0m");

    bind(s_socketListen, (struct sockaddr*) &s_serverAddress, sizeof(s_serverAddress));   /// On attache le socket a l'adresse indiquee

    if (listen(s_socketListen, MAX_PENDING) < 0)     {                                    /// on met le socket en ecoute et on accepte que MAX_PENDING connexions
        printf("%sError while listenning the port%s\n", "\033[41m", "\033[0m");
    }     else     {
        l_returnValue = EXIT_SUCCESS;
    }

    return l_returnValue;
}
/**
 * @fn static void* run(void* _) {
 * @brief méthode qui va tourner pour
 *
 * @param argc premier argument correspondant au nombre d'arguments
 * @param argv arguments
 */

static void* run(void* _) {
    int l_returnValue = EXIT_FAILURE;
    fd_set l_env;

    while (s_keepGoing)     {
        /* https://broux.developpez.com/articles/c/sockets/#LV-C-2 */
        /* https://www.blaess.fr/christophe/2013/12/27/comprendre-le-fonctionnement-de-select/ */

        FD_ZERO(&l_env);
        FD_SET(STDIN_FILENO, &l_env);
        FD_SET(s_socketListen, &l_env);

        for (int i = 0; i < MAX_PENDING; i++)         {
            pthread_mutex_lock(&s_mutexSocket);
            int l_socketClientValue = s_socketClient[i];
            pthread_mutex_unlock(&s_mutexSocket);
            if (l_socketClientValue != NO_CLIENT_SOCKET_VALUE)             {
                FD_SET(l_socketClientValue, &l_env);
            }
        }

        if (select(FD_SETSIZE, &l_env, NULL, NULL, NULL) == -1)         {
            printf("%sError with %sselect()%s\n", "\033[41m", "\033[21m", "\033[0m");
            break; ///s_keepGoing
        }

        if (FD_ISSET(STDIN_FILENO, &l_env))         {
            printf("\n%sAsk for exit%s\n", "\033[41m", "\033[0m");
            l_returnValue = EXIT_SUCCESS;
            break; ///s_keepGoing
        }         else if (FD_ISSET(s_socketListen, &l_env))         {
            l_returnValue = connectClient();
            if (l_returnValue != EXIT_SUCCESS)             {
                break; ///s_keepGoing
            }
        }         else         {
            for (int i = 0; i < MAX_PENDING; i++)             {
                pthread_mutex_lock(&s_mutexSocket);
                int l_socketClientValue = s_socketClient[i];
                pthread_mutex_unlock(&s_mutexSocket);
                if (l_socketClientValue != NO_CLIENT_SOCKET_VALUE)                 {
                    if (FD_ISSET(l_socketClientValue, &l_env))                     {
                        readMsg(i);
                        break; ///i
                    }
                }
            }
        }
    }

    for (int i = 0; i < MAX_PENDING; i++)     {
        pthread_mutex_lock(&s_mutexSocket);
        int l_socketClientValue = s_socketClient[i];
        pthread_mutex_unlock(&s_mutexSocket);
        if (l_socketClientValue != NO_CLIENT_SOCKET_VALUE)         {
            l_returnValue += disconnectClient(i);
        }
    }
    pthread_exit(&l_returnValue);
}


static int connectClient(void) {
    int l_returnValue = EXIT_FAILURE;
    int l_indexClient = MAX_PENDING;

    for (int i = 0; i < MAX_PENDING; i++)     {
        pthread_mutex_lock(&s_mutexSocket);
        int l_socketClientValue = s_socketClient[i];
        pthread_mutex_unlock(&s_mutexSocket);
        if (l_socketClientValue == NO_CLIENT_SOCKET_VALUE)         {
            l_indexClient = i;
            break; ///i
        }
    }

    if (l_indexClient < MAX_PENDING)     {
        int l_socketValue = accept(s_socketListen, NULL, 0);

        if (l_socketValue < 0)         {
            printf("%sError when connecting the client%s\n", "\033[41m", "\033[0m");
        }         else         {
            printf("%sConnection of a client%s\n", "\033[42m", "\033[0m");
            l_returnValue = EXIT_SUCCESS;
            pthread_mutex_lock(&s_mutexSocket);
            s_socketClient[l_indexClient] = l_socketValue; ///in case of the Macro value is not -1
            pthread_mutex_unlock(&s_mutexSocket);
        }
    }     else     {
        l_returnValue = EXIT_SUCCESS;
    }

    return l_returnValue;
}

static int readMsg(const int p_clientIndex) {
    int l_returnValue = EXIT_FAILURE;

    Data l_data;

    int l_quantityReaddean = 0;
    int l_quantityToRead = sizeof(Data);

    while (l_quantityToRead > 0)     {
        pthread_mutex_lock(&s_mutexSocket);
        l_quantityReaddean = read(s_socketClient[p_clientIndex], &l_data + l_quantityReaddean, l_quantityToRead);
        pthread_mutex_unlock(&s_mutexSocket);

        if (l_quantityReaddean < 0)         {
            printf("%sError when receiving the message%s\n\n", "\033[41m", "\033[0m");
            break; ///l_quantityWritten
        }         else if (l_quantityReaddean == 0)         {
            /* Client leave */
            disconnectClient(p_clientIndex);
            l_returnValue = EXIT_SUCCESS;
            break; ///l_quantityWritten
        }         else         {
            l_quantityToRead -= l_quantityReaddean;
        }
    }

    if (l_quantityToRead == 0)     {
        /* Client send a message, ignore it */
        l_returnValue = EXIT_SUCCESS;
    }

    return l_returnValue;
}

static int disconnectClient(const int p_clientIndex) {
    pthread_mutex_lock(&s_mutexSocket);
    int l_returnValue = close(s_socketClient[p_clientIndex]);
    pthread_mutex_unlock(&s_mutexSocket);

    if (l_returnValue < 0)     {
        l_returnValue = EXIT_FAILURE;
    }     else     {
        printf("%s%sClient %d is disconnect%s\n\n", "\033[43m", "\033[30m", p_clientIndex, "\033[0m");
        pthread_mutex_lock(&s_mutexSocket);
        s_socketClient[p_clientIndex] = NO_CLIENT_SOCKET_VALUE;
        pthread_mutex_unlock(&s_mutexSocket);

        l_returnValue = EXIT_SUCCESS;
    }

    return l_returnValue;
}


/// s'attend à recevoir un pointeur
/// static void* spamClientSocket(void* _)
static void spamClientSocket(void) {            ///tant que le prog fonctionne on continue
    int l_returnValue = EXIT_FAILURE;

    while (s_keepGoing)     {
        for (int i = 0; i < MAX_PENDING; i++)         { /// pour chaque socket de client o envoie un message
            pthread_mutex_lock(&s_mutexSocket);         /// protege acces a tableau en lecture
            int l_socketClientValue = s_socketClient[i];
            pthread_mutex_unlock(&s_mutexSocket);

            if (l_socketClientValue != NO_CLIENT_SOCKET_VALUE)             {  /// si pas client on envoie pas message
                l_returnValue = sendMsg(i);
            }

            if (l_returnValue != EXIT_SUCCESS)             {
                break; ///s_keepGoing
            }
        }
        sleep(1);
    }

    pthread_exit(&l_returnValue);
}

static int sendMsg(const int p_clientIndex) {
    int l_returnValue = EXIT_FAILURE;

    Data l_data;
    int l_quantityWritten = 0;
    int l_quantityToWrite = sizeof(l_data);

    sprintf(l_data.data, "%s", "HelloSocket!!");
    l_data.integer = 666;

    while (l_quantityToWrite > 0)     {   ///pas garantie qu'on écrit tous les octets
        pthread_mutex_lock(&s_mutexSocket);
        l_quantityWritten = write(s_socketClient[p_clientIndex], &l_data + l_quantityWritten, l_quantityToWrite);
        pthread_mutex_unlock(&s_mutexSocket);

        if (l_quantityWritten < 0)         {
            printf("%sError when sending the message %s\n", "\033[41m", "\033[0m");
            break; ///l_quantityWritten
        }         else         {
            l_quantityToWrite -= l_quantityWritten;
        }
    }

    if (l_quantityToWrite == 0)     {
        l_returnValue = EXIT_SUCCESS;
    }
    return l_returnValue;
}

/// read et set dans deux thread différent est il necessaire de verouiller les mutex pour pouvoir écrire et lire
/// risque qu'on écrive et lise sur le meme client en mm tps
/// est il necessaire de proteger par des mutex les sockets
/// est ce que while pr verifier tout vérfier tous les octet nécessaire