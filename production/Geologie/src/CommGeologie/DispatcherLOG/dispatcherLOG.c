/**
 * @file dispatcherLOG.c
 *
 * @version 1.0
 * @date 06/06/21
 * @author Nathan BRIENT
 * @copyright BSD 2-clauses
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MQ_MAX_MESSAGES (10)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <unistd.h> // Macros, type Posix and co
#include <pthread.h>
#include <mqueue.h>
#include <time.h>
#include <errno.h>

#include "dispatcherLOG.h"
#include <stdint.h>
#include "../com_common.h"
#include <stdio.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
/*
typedef enum {
    S_FORGET = 0,
    S_DEATH,
    S_WATING_FOR_CONNECTION,
    S_IDLE,
    S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION,
    S_WAITING_FOR_BE_PLACED,
    S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT,
    S_CHOICE_CONNEXION,
    S_CHOICE_CALIBRATION,
    NB_STATE_
} State_GEOGRAPHER; // Déclencheur

const char* const State_Geographer_Name[] = {
    "S_FORGET",
    "S_DEATH",
    "S_WATING_FOR_CONNECTION",
    "S_IDLE",
    "S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION",
    "S_WAITING_FOR_BE_PLACED",
    "S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT",
    "S_CHOICE_CONNEXION",
    "S_CHOICE_CALIBRATION",
    "NB_STATE_"
};

static const char* State_Geographer_getName(uint8_t i) {
    return State_Geographer_Name[i];
}

typedef enum {
    E_STOP = 0,
    E_CONNECTION_ESTABLISHED,
    E_DATE_AND_SEND_DATA_ELSE,
    E_DATE_AND_SEND_DATA_CONNECTED,
    E_ASK_CALIBRATION_POSITIONS,
    E_CONNECTION_DOWN,
    E_VALIDATE_POSITION,
    E_SIGNAL_END_UPDATE_ATTENUATION_CALIBRATION,
    E_SIGNAL_END_UPDATE_ATTENUATION_ELSE,
    E_SIGNAL_END_AVERAGE_CALCUL,
    NB_EVENT_GEOGRAPHER
} Event_GEOGRAPHER; // Déclencheur

const char* const Event_Geographer_Name[] = {
    "E_STOP",
    "E_CONNECTION_ESTABLISHED",
    "E_DATE_AND_SEND_DATA_ELSE",
    "E_DATE_AND_SEND_DATA_CONNECTED",
    "E_ASK_CALIBRATION_POSITIONS",
    "E_CONNECTION_DOWN",
    "E_VALIDATE_POSITION",
    "E_SIGNAL_END_UPDATE_ATTENUATION_CALIBRATION",
    "E_SIGNAL_END_UPDATE_ATTENUATION_ELSE",
    "E_SIGNAL_END_AVERAGE_CALCUL",
    "NB_EVENT_GEOGRAPHER"
};

static const char* Event_Geographer_getName(uint8_t i) {
    return Event_Geographer_Name[i];
}

typedef enum {
    A_NOP = 0,
    A_STOP,
    A_CONNECTION_ESTABLISHED,
    A_CONNECTION_DOWN,
    A_SET_ALL_DATA,
    A_SET_CALIBRATION_POSITIONS,
    A_ASK_4_UPDATE_ATTENUATION_COEFFICIENT,
    A_END_CALIBRATION_POSITION,
    A_END_CALIBRATION,
    A_ASK_4_AVERAGE_CALCUL,
    A_CALIBRATION_COUNTER

} Action_GEOGRAPHER;

const char* const Action_Geographer_Name[] = {
    "A_NOP",
    "A_STOP",
    "A_CONNECTION_ESTABLISHED",
    "A_CONNECTION_DOWN",
    "A_SET_ALL_DATA",
    "A_SET_CALIBRATION_POSITIONS",
    "A_ASK_4_UPDATE_ATTENUATION_COEFFICIENT",
    "A_END_CALIBRATION_POSITION",
    "A_END_CALIBRATION",
    "A_ASK_4_AVERAGE_CALCUL",
    "A_CALIBRATION_COUNTER"
};

static const char* Action_Geographer_getName(uint8_t i) {
    return Action_Geographer_Name[i];
}


//etat de destination et action associé
typedef struct {
    State_GEOGRAPHER destinationState;
    Action_GEOGRAPHER action;
} Transition_GEOGRAPHER;
*/
typedef struct {
    Trame* trame;
} MqMsg; //type de message dans la BAL


static pthread_t myThreadOrder;
static pthread_t myThreadListen;


/**
 * The name of the queue
 */
static const char BAL[] = "/BALDispatcherLOG";

static mqd_t descripteur; //On déclare un descripteur de notre BAL qui permettra de l'ouvrir et de la fermer

static struct mq_attr attr; //On déclare un attribut pour la fonction mq_open qui est une structure spécifique à la file pour la configurer (cf l.64)

/*
static State_GEOGRAPHER myState;

static Transition_GEOGRAPHER stateMachine[NB_STATE_ - 1][NB_EVENT_GEOGRAPHER] =
{
    [S_WATING_FOR_CONNECTION] [E_CONNECTION_ESTABLISHED] = {S_IDLE,A_CONNECTION_ESTABLISHED},

    [S_IDLE][E_ASK_CALIBRATION_POSITIONS] = {S_WAITING_FOR_BE_PLACED,A_SET_CALIBRATION_POSITIONS},
    [S_IDLE][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION,A_CONNECTION_DOWN},
    [S_IDLE][E_DATE_AND_SEND_DATA_ELSE] = {S_IDLE,A_NOP}, //TODO choice if dans appel methode dateAndSend
    [S_IDLE][E_DATE_AND_SEND_DATA_CONNECTED] = {S_IDLE,A_SET_ALL_DATA},


    [S_WAITING_FOR_BE_PLACED][E_VALIDATE_POSITION] = {S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION,A_ASK_4_UPDATE_ATTENUATION_COEFFICIENT},
    [S_WAITING_FOR_BE_PLACED][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION,A_CONNECTION_DOWN},

    [S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION][E_SIGNAL_END_UPDATE_ATTENUATION_ELSE] = {S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT,A_END_CALIBRATION},//TODO choice dans appel signalEndUpdateAttenuation
    [S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION][E_SIGNAL_END_UPDATE_ATTENUATION_CALIBRATION] = {S_WAITING_FOR_BE_PLACED,A_CALIBRATION_COUNTER},//TODO choice dans appel signalEndUpdateAttenuation
    [S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION,A_CONNECTION_DOWN},

    [S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION,A_CONNECTION_DOWN},
    [S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT][E_SIGNAL_END_AVERAGE_CALCUL] = {S_IDLE,A_END_CALIBRATION},


};
*/

static int8_t keepGoing = 1;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * @fn static void mqInit ()
 * @brief initialise la boite aux lettres
 *
 * @return 1 si erreur detectee, sinon retourne 0
*/
static void mqInit();

/**
 * @fn static uint8_t mqDone ()
 * @brief ferme et detruit la boite aux lettres
 *
 * @return 1 si erreur detectee, sinon retourne 0
*/
static void mqDone();

/**
 * @fn static uint8_t mqReceive (MqMsg *this)
 * @brief ferme et detruit la boite aux lettres
 *
 * @return 1 si erreur detectee, sinon retourne 0
 * @param this message a envoyer a la queue
*/
static void mqReceive(MqMsg* this);


/**
 * @fn static void* order();
 * @brief methode appelee par un thread pour recevoir les action a faire grace a une boite au lettre et les effectuer
 *
 * cette metode lira la boite au lettre et effectuera les actions
 */
static void* order();
/**
 * @fn static void performAction(Action_GEOGRAPHER action, MqMsg* msg);
 * @brief methode appelee par order pour executer les actions
 *
 * @param action action a effectuee
 * @param msg structure contenant les donnees a utiliser pour effectuer l'action
 */
static void performAction(Trame* trame, Header* Header);




static void mqInit(void) {
    printf("On entre dans le Init\n");
    attr.mq_flags = 0; //Flags de la file
    attr.mq_maxmsg = MQ_MAX_MESSAGES; //Nombre maximum de messages dans la file
    attr.mq_msgsize = sizeof(MqMsg); //Taille maximale de chaque message
    attr.mq_curmsgs = 0; //Nombre de messages actuellement dans la file

    /* destruction de la BAL si toutefois préexistante */

    if (mq_unlink(BAL) == -1) {
        if (errno != ENOENT) {
            perror("Erreur Unlink : ");
            exit(1);
        }
    }
    /* création et ouverture de la BAL */
    descripteur = mq_open(BAL, O_RDWR | O_CREAT, 0777, &attr);
    /*On ouvre la BAL avec comme arguments : le nom de la BAL, un flag pour ouvrir la file en lecture et en ecriture (pour utiliser recEve et send) ou pour créer une MQ, le droit d'accès,l'attribut crée*/
    if (descripteur == -1) {
        perror("Erreur Open :\n");
    } else {
        printf("BAL ouverte\n");
    }

}

static void mqDone(void) {
    printf("On entre dans le Done\n");

    /* fermeture de la BAL */
    mq_close(descripteur); //On ferme la BAL en mettant en paramètre le descripteur

    /* destruction de la BAL */
    mq_unlink(BAL); //On détruit la BAL en mettant en paramètre le nom de la BAL
}

static void mqReceive(MqMsg* this) {
    printf("On entre dans le mq_receive\n");
    mq_receive(descripteur, (char*) this, sizeof(*this), NULL);
}



static void performAction(Trame* trame, Header* header) {
    switch (header->commande)     {
    case ASK_CALIBRATION_POSITIONS:
        /* code */
        break;
    case SEND_EXPERIMENTAL_POSITIONS:
        /* code */
        break;
    case SEND_EXPERIMENTAL_TRAJECTS:
        /* code */
        break;
    case SEND_MEMORY_PROCESSOR_LOAD:
        /* code */
        break;
    case SEND_ALL_BEACONS_DATA:
        /* code */
        break;
    case SEND_CURRENT_POSITION:
        /* code */
        break;
    case REP_CALIBRATION_POSITIONS:
        /* code */
        break;
    case SIGNAL_CALIBRATION_START:
        /* code */
        break;
    case SIGNAL_CALIBRATION_POSITION:
        /* code */
        break;
    case SIGNAL_CALIRATION_END:
        /* code */
        break;
    default:
        break;
    }


    //free(msg->trame);
}

static uint8_t sendMsgToQueue(MqMsg* msg) { //TODO pas besoin mutex, deja protege/a revoir
    uint8_t returnError = EXIT_FAILURE;
    if (mq_send(descripteur, (char*) &msg, sizeof(msg), 0) == 0) {
        returnError = EXIT_SUCCESS;
    }
    return returnError;
}

static void readHeader(Header* header) {
    Trame headerTrame[SIZE_HEADER];
    PostmanLOG_readMsg(headerTrame, SIZE_HEADER); //on lit le header d'abord
    TranslatorLog_translateTrameToHeader(headerTrame, header);
}



static void* listen() {
    printf("order\n");
    while (keepGoing) {
        Header header;
        readHeader(&header); //on lit d'abord le header

        Trame* trame;
        trame = malloc(header.size);
        PostmanLOG_readMsg(trame, header.size); //on lit ensuite toute la trame //TODO mettre un mutex sur lecture/ecriture de trame et header
        performAction(trame,&header);
    }
    return 0;

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


extern int8_t DispatcherLOG_new() {
    mqInit();


    return 0;
}


extern int8_t DispatcherLOG_free() {
    mqDone();
    return 0;
}

extern int8_t DispatcherLOG_start() {
    int8_t returnError = EXIT_FAILURE;

    if (returnError == EXIT_SUCCESS) {
        returnError = pthread_create(&myThreadListen, NULL, &listen, NULL);
        // premier thread pour recevoir
        if (keepGoing != EXIT_SUCCESS) {
            keepGoing = 0;
        }
    }

    return returnError;
}

extern int8_t DispatcherLOG_stop() {
    int8_t returnError = EXIT_FAILURE;

    if (returnError == EXIT_SUCCESS) {
        returnError = pthread_join(myThreadListen, NULL);

        if (returnError == EXIT_SUCCESS) {
            returnError = pthread_join(myThreadOrder, NULL);
        }
    }
    return returnError;
}

