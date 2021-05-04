/**
 * @file testHostReachable
 *
 * @brief Permet de tester s'il est possible de se connecter à un server a un port donne.
 *
 * @version 1.0
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
#include <signal.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEFAULT_IP "192.168.7.1" 
#define DEFAULT_PORT (12345)
#define MAX_CONNECTION_ATTEMPT (10)

typedef struct {
    int integer;
    char msg[15];
} Data;

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


static void intHandler(int p_dummy) {
    s_keepGoing = 0;
}

int main(int argc, char* argv[]) {
    int l_returnValue = EXIT_FAILURE;
    signal(SIGINT, intHandler);

    if (argc > 1) {
        sprintf(s_ip, argv[1]);
        s_port = atoi(argv[2]);
    } else {
        sprintf(s_ip, DEFAULT_IP);
        s_port = DEFAULT_PORT;
    }

    printf("Try to reach %s on the port %d\n", s_ip, s_port);

    l_returnValue = setServerAdress();

    if (l_returnValue == EXIT_SUCCESS && s_keepGoing) {
        l_returnValue = createSocket();

        if (l_returnValue == EXIT_SUCCESS && s_keepGoing) {
            l_returnValue = connection();

            if (l_returnValue == EXIT_SUCCESS && s_keepGoing) {
                l_returnValue = run();

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

static int setServerAdress(void) {
    const struct hostent* l_hostInfo = gethostbyname(s_ip);
    int l_returnValue = EXIT_FAILURE;

    if (l_hostInfo == NULL) {
        printf("%sThe host is not reachable%s\n", "\033[41m", "\033[0m");
    } else {
        s_serverAdress.sin_port = htons(s_port);
        s_serverAdress.sin_family = AF_INET;
        s_serverAdress.sin_addr = *((struct in_addr*) l_hostInfo->h_addr_list[0]);

        l_returnValue = EXIT_SUCCESS;
    }

    return l_returnValue;
}

static int createSocket(void) {
    s_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int l_returnValue = EXIT_FAILURE;


    if (s_socket < 0) {
        printf("%sError socket%s\n", "\033[41m", "\033[0m");
    } else {
        l_returnValue = EXIT_SUCCESS;
    }

    return l_returnValue;
}

static int closeSocket(void) {
    int l_returnValue = close(s_socket);

    if (l_returnValue < 0) {
        l_returnValue = EXIT_FAILURE;
    } else {
        l_returnValue = EXIT_SUCCESS;
    }

    return l_returnValue;
}

static int connection(void) {
    printf("%sConnection tent to the server%s\n\n", "\033[34m", "\033[0m");

    int l_attemptCounter = 0;
    int l_returnValue = EXIT_FAILURE;

    while (l_attemptCounter < MAX_CONNECTION_ATTEMPT) {
        l_attemptCounter++;

        if (connect(s_socket, (struct sockaddr*) &s_serverAdress, sizeof(s_serverAdress)) < 0) {
            printf("%s%s%sConnection failure, attempt %d / %d%s\n",
                "\033[1A", "\033[K", "\033[33m", l_attemptCounter, MAX_CONNECTION_ATTEMPT, "\033[0m");

            sleep(1); // Sleep and retry after
        } else {
            break; // while (l_attemptCounter < MAX_CONNECTION_ATTEMPT)
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

static int run(void) {
    printf("\nPress the enter key to exit the program\n");

    int l_returnValue = EXIT_FAILURE;
    fd_set l_env;

    while (s_keepGoing) {
        FD_ZERO(&l_env);
        FD_SET(STDIN_FILENO, &l_env);
        FD_SET(s_socket, &l_env);

        if (select(FD_SETSIZE, &l_env, NULL, NULL, NULL) == -1) {
            printf("%sError with %sselect()%s\n", "\033[41m", "\033[21m", "\033[0m");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &l_env)) {
            /* Keyboard input */
            printf("%sKeyboard input, close the program%s\n", "\033[43m", "\033[0m");
            l_returnValue = EXIT_SUCCESS;
            break;

        } else if (FD_ISSET(s_socket, &l_env)) {
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

static int readMsg(Data* p_data) {
    int l_returnValue = EXIT_FAILURE;

    unsigned int l_quantityReaddean = 0;
    unsigned int l_quantityToRead = sizeof(Data);

    while (l_quantityToRead > 0) {
        l_quantityReaddean = read(s_socket, p_data + l_quantityReaddean, l_quantityToRead);

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

    if (l_quantityToRead == 0) {
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
