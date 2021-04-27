/*
 * @file testHostReachable
 *
 * @brief TODO
 *
 * @version 1.0									TODO
 * @date 27 avr. 2021
 * @author GAUTIER Pierre-Louis
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEFAULT_IP "142.250.201.174" // equivalent to google.com TODO use STM32MP1 IP
#define DEFAULT_PORT (80) // default http port TODO use STM32MP1 port
#define MAX_CONNECTION_ATTEMPT (10)

static int setServerAdress(const char p_ipAdress[], const int p_port, struct sockaddr_in* p_serverAdress);
static int createSocket(void);
static int connection(const int p_socket, const struct sockaddr_in* p_serverAdress);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[]) {
    int l_returnValue = EXIT_FAILURE;

    char l_host[15] = DEFAULT_IP; // 15 => Taille de 000.000.000.000
    int l_port = DEFAULT_PORT;
    struct sockaddr_in l_serverAdress;
    int l_socket;

    if (argc > 1) {
        sprintf(l_host, argv[1]);
        l_port = atoi(argv[2]);
    }

    printf("Try to reach %s on the port %d\n", l_host, l_port);

    l_returnValue = setServerAdress(l_host, l_port, &l_serverAdress);
    if (l_returnValue == EXIT_SUCCESS) {
        l_socket = createSocket();

        if (l_socket >= 0) {
            l_returnValue = connection(l_socket, &l_serverAdress);
        }
        else {
            l_returnValue = EXIT_FAILURE;
        }
    }

    return l_returnValue;
}

static int setServerAdress(const char p_ipAdress[], const int p_port, struct sockaddr_in* p_serverAdress) {
    const struct hostent* l_hostInfo = gethostbyname(p_ipAdress);
    int l_returnValue = EXIT_FAILURE;

    if (l_hostInfo == NULL) {
        printf("%sThe host is not reachable%s\n", "\033[41m", "\033[0m");
    }
    else {
        p_serverAdress->sin_port = htons(p_port);
        p_serverAdress->sin_family = AF_INET;
        p_serverAdress->sin_addr = *((struct in_addr*) l_hostInfo->h_addr_list[0]);

        l_returnValue = EXIT_SUCCESS;
    }

    return l_returnValue;
}

static int createSocket(void) {
    int l_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (l_socket < 0) {
        printf("%sError socket%s\n", "\033[41m", "\033[0m");
        l_socket = EXIT_FAILURE;
    }

    return l_socket;
}

static int connection(const int p_socket, const struct sockaddr_in* p_serverAdress) {
    printf("%sConnection tent to the server%s\n\n", "\033[34m", "\033[0m");

    int l_attemptCounter = 0;
    int l_returnValue = EXIT_FAILURE;

    while (l_attemptCounter < MAX_CONNECTION_ATTEMPT) {
        l_attemptCounter++;

        if (connect(p_socket, (struct sockaddr*) p_serverAdress, sizeof(*p_serverAdress)) < 0) {
            printf("%s%s%sConnection failure, attempt %d / %d%s\n",
                "\033[1A", "\033[K", "\033[33m", l_attemptCounter, MAX_CONNECTION_ATTEMPT, "\033[0m");

            sleep(1); // Sleep and retry after
        }
        else {
            break; // while (l_attemptCounter < MAX_CONNECTION_ATTEMPT)
        }
    }

    if (l_attemptCounter < MAX_CONNECTION_ATTEMPT) {
        printf("%s%s%sConnection success after %d attemps%s\n\n", "\033[1A", "\033[K", "\033[32m", l_attemptCounter, "\033[0m");
    }
    else {
        printf("%sLimit on the number of connection attempts exceeded%s\n", "\033[41m", "\033[0m");
        l_returnValue = EXIT_SUCCESS;
    }

    return l_returnValue;
}
