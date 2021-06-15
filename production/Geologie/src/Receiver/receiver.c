/**
 * @file receiver.c
 *
 * @version 2.0
 * @date 5/06/2021
 * @author Gabriel LECENNE
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#include <stdlib.h>
#include <errno.h>
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <inttypes.h>
#include <mqueue.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>

#include "receiver.h"
#include "../TranslatorBeacon/translatorBeacon.h"
#include "../Watchdog/watchdog.h"
#include "../Scanner/scanner.h"
#include "../common.h"
#include "../tools.h"
#include "stdbool.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Variable et structure privee

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NB_BEACONS_AVAILABLE (5)
#define NB_MAX_ADVERTISING_CHANNEL (100)
#define NB_MAX_BEACONS_AVAILABLE (4)

#define BEACONS_UUID_1 (0x18)
#define BEACONS_UUID_2 (0x1A)

#define MQ_MAX_MESSAGES (10)

static BeaconSignal beaconsSignal[NB_MAX_BEACONS_AVAILABLE] = {
    {{'B','1','\0'},  {BEACONS_UUID_1, BEACONS_UUID_2}, -69.51544993, {400, 700}},
    {{'B','2','\0'},  {BEACONS_UUID_1, BEACONS_UUID_2}, -68.45673382, {980, 100}},
    {{'B','3','\0'},  {BEACONS_UUID_1, BEACONS_UUID_2}, -72.97218376, {1300, 800}},
    {{'B','4','\0'},  {BEACONS_UUID_1, BEACONS_UUID_2}, -98, {1500, 1500}}
};

static BeaconsChannel* beaconsChannel;
static uint32_t NbBeaconsSignal = 3;

typedef enum {
    S_FORGET = 0,
    S_SCANNING,
    S_TRANSLATING,
    S_DEATH,
    NB_STATE
}State_RECEIVER;

typedef enum {
    E_STOP = 0,
    E_ASK_BEACONS_SIGNAL,
    E_MAJ_BEACONS_CHANNEL,
    E_TIME_OUT,
    E_TRANSLATING_DONE,
    NB_EVENT_RECEIVER
} Event_RECEIVER;

typedef enum {
    A_NOP = 0,
    A_STOP,
    A_SEND_BEACONS_SIGNAL,
    A_MAJ_BEACONS_CHANNELS,
    A_TRANSLATE,
    NB_ACTION_RECEIVER
} Action_RECEIVER;

typedef struct {
    State_RECEIVER destinationState;
    Action_RECEIVER action;
} Transition_RECEIVER;

static Transition_RECEIVER stateMachine[NB_STATE - 1][NB_EVENT_RECEIVER] =
{
    [S_SCANNING] [E_MAJ_BEACONS_CHANNEL] = {S_SCANNING, A_MAJ_BEACONS_CHANNELS},
    [S_SCANNING][E_ASK_BEACONS_SIGNAL] = {S_SCANNING, A_SEND_BEACONS_SIGNAL},
    [S_SCANNING][E_TIME_OUT] = {S_TRANSLATING, A_TRANSLATE},
    [S_SCANNING][E_STOP] = {S_DEATH, A_STOP},

    [S_TRANSLATING][E_ASK_BEACONS_SIGNAL] = {S_SCANNING, A_SEND_BEACONS_SIGNAL},
    [S_TRANSLATING][E_TRANSLATING_DONE] = {S_SCANNING, A_MAJ_BEACONS_CHANNELS},
    [S_TRANSLATING][E_STOP] = {S_DEATH, A_STOP},
};

struct hci_request ble_hci_request(uint16_t ocf, uint8_t clen, void* status, void* cparam) {
    struct hci_request rq;
    memset(&rq, 0, sizeof(rq));
    rq.ogf = OGF_LE_CTL;
    rq.ocf = ocf;
    rq.cparam = cparam;
    rq.clen = clen;
    rq.rparam = status;
    rq.rlen = 1;
    return rq;
}

State_RECEIVER myState;
static pthread_t myThreadMq;

static const char BAL[] = "/BALReceiver";
static mqd_t descripteur;
static struct mq_attr attr;

typedef struct {
    Event_RECEIVER event;
} MqMsgReceiver;

static Watchdog* wtd_TScan;
static pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn static void mqInit()
 * @brief Initialise la boite aux lettres
 *
 * @return renvoie 1 si une erreur est detectee, sinon 0
*/
static void mqInit();

/**
 * @fn static void sendMsg(MqMsgReceiver* this)
 * @brief Envoie des messages a la BAL
 *
 * @param this structure du message envoye a la BAL
 * @return renvoie 1 si une erreur est detectee, sinon 0
*/
static int8_t sendMsg(MqMsgReceiver* this);

/**
 * @fn static void mqReceive(MqMsgReceiver* this)
 * @brief Va chercher les messages dans la BAL
 *
 * @param msg structure du message a recuperer
 * @return renvoie 1 si une erreur est detectee, sinon 0
*/
static void mqReceive(MqMsgReceiver* this);

/**
 * @fn static void Receiver_translateChannelToBeaconsSignal()
 * @brief traduit le tableau en variable globale de BeaconChannel en BeaconSignal
 */

static void Receiver_translateChannelToBeaconsSignal();

/**
 * @fn static void Receiver_getAllBeaconsChannel()
 * @brief methode privee permettant de memoriser dans beaconsChannel l'ensemble des trames des balises
 */

static void Receiver_getAllBeaconsChannel();

/**
 * @fn static void performAction(Action_SCANNER action, MqMsgReceiver * msg)
 * @brief execute les fonctions a realiser en fonction du parametre action
 *
 * @param action action a executer
 * @param msg message qui contient les donnees necessaire a l'execution de la fonction
*/
static void performAction(Action_RECEIVER action, MqMsgReceiver* msg);

/**
 * @fn static void * run()
 * @brief thread qui lit la BAL et met a jour l'action a realiser
*/
static void* run(void* _);

/**
 * @fn static void time_out()
 * @brief fonction de callback du watchdog wtd_TScan
*/
static void time_out();

static void mqInit() {

    attr.mq_flags = 0; //Flags de la file
    attr.mq_maxmsg = MQ_MAX_MESSAGES; //Nombre maximum de messages dans la file
    attr.mq_msgsize = sizeof(MqMsgReceiver); //Taille maximale de chaque message
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
    }
}

static int8_t sendMsg(MqMsgReceiver* msg) {
    int8_t returnError = EXIT_FAILURE;
    if (mq_send(descripteur, (char*) msg, sizeof(MqMsgReceiver), 0) == 0) {
        returnError = EXIT_SUCCESS;
    }

    return returnError;
}

static void mqReceive(MqMsgReceiver* msg) {
    mq_receive(descripteur, (char*) msg, sizeof(MqMsgReceiver), NULL);
}

static void Receiver_translateChannelToBeaconsSignal() {
    MqMsgReceiver msg = { .event = E_TRANSLATING_DONE };
    sendMsg(&msg);
}

static void Receiver_getAllBeaconsChannel() {
    // Do something in a other process
}

static void performAction(Action_RECEIVER action, MqMsgReceiver* msg) {
    switch (action) {

        case A_SEND_BEACONS_SIGNAL:
            for (uint8_t i = 0; i < NbBeaconsSignal; i++) {
                beaconsSignal[i].rssi = -((rand() % 104) - 4);
            }
            Scanner_setAllBeaconsSignal(beaconsSignal, NbBeaconsSignal);
            Watchdog_start(wtd_TScan);
            break;

        case A_MAJ_BEACONS_CHANNELS:
            Watchdog_start(wtd_TScan);
            Receiver_getAllBeaconsChannel();
            break;

        case A_TRANSLATE:
            Receiver_translateChannelToBeaconsSignal(beaconsChannel);

        case A_STOP:
            break;
        default:
            break;
    }

}

static void* run(void* _) {
    while (myState != S_DEATH) {
        MqMsgReceiver msg;
        Action_RECEIVER action;

        mqReceive(&msg);

        action = stateMachine[myState][msg.event].action;
        if (stateMachine[myState][msg.event].destinationState != S_FORGET) {
            performAction(action, &msg);
            myState = stateMachine[myState][msg.event].destinationState;
        } else {
            TRACE("Receiver lost an event %d at state %d%s", msg.event, myState, "\n");
        }
    }
    return NULL;
}

static void time_out() {
    TRACE("[Receiver] Time out%s", "\n");
    MqMsgReceiver msg = { .event = E_TIME_OUT };
    sendMsg(&msg);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


extern void Receiver_new() {
    mqInit();
    pthread_mutex_init(&myMutex, NULL);

    wtd_TScan = Watchdog_construct(1000000, (WatchdogCallback) time_out);
}

extern int8_t Receiver_ask4StartReceiver() {
    int8_t returnError = EXIT_FAILURE;
    myState = S_SCANNING;
    returnError = pthread_create(&myThreadMq, NULL, &run, NULL);
    Watchdog_start(wtd_TScan);

    return returnError;
}

extern int8_t Receiver_ask4StopReceiver() {
    MqMsgReceiver msg = { .event = E_STOP };
    sendMsg(&msg);
    int8_t returnError = EXIT_FAILURE;
    returnError = pthread_join(myThreadMq, NULL);
    return returnError;
}

extern void Receiver_free() {
    myState = S_DEATH;
    pthread_mutex_destroy(&myMutex);

    Watchdog_destroy(wtd_TScan);
}

extern int8_t Receiver_ask4BeaconsSignal() {
    int8_t returnError = EXIT_FAILURE;
    MqMsgReceiver msg = {
                .event = E_ASK_BEACONS_SIGNAL
    };
    returnError = sendMsg(&msg);
    return returnError;
}
