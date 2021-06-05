/**

 * @file receiver.c

 *

 * @version 2.0

 * @date 5/06/2021

 * @author Gabriel LECENNE

 * @copyright Geo-Boot

 * @license BSD 2-clauses

 */

#include "receiver.h"

#include <stdio.h>

#include <stdlib.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Variable et structure privee

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static BeaconsSignal beaconsSignal[100];

static BeaconsChannel beaconsChannel[100];


typedef enum{
    S_SCANNING = 0,
    S_TRANSLATING,
    S_DEATH,
    NB_STATE
}State_RECEIVER;

typedef enum {
    E_STOP = 0,
    E_ASK_BEACONS_SIGNAL,
    E_MAJ_BEACONS_CHANNEL,
    E_TRANSLATE_BEACONS_CHANNELS,
    NB_EVENT_RECEIVER
} Event_RECEIVER;

typedef enum {
    A_NOP = 0,
    A_STOP,
    A_SEND_BEACONS_SIGNAL,
    A_MAJ_BEACONS_CHANNELS,
    A_TRANSLATE_BEACONS_CHANNELS,
    NB_ACTION_RECEIVER
} Action_RECEIVER;

typedef struct {
    State_RECEIVER destinationState;
    Action_RECEIVER action;
} Transition_RECEIVER;

static Transition_RECEIVER stateMachine[NB_STATE - 1][NB_EVENT_RECEIVER] =
{
    [S_SCANNING] [E_STOP] = {S_DEATH, A_STOP},
    [S_SCANNING] [E_ASK_BEACONS_SIGNAL] = {S_SCANNING, A_SEND_BEACONS_SIGNAL},
    [S_SCANNING] [E_MAJ_BEACONS_CHANNEL] = {S_SCANNING, A_MAJ_BEACONS_CHANNELS},
    [S_SCANNING] [E_TRANSLATE_BEACONS_CHANNELS] = {S_TRANSLATING, A_TRANSLATE_BEACONS_CHANNELS},
    [S_TRANSLATING] [E_STOP] = {S_DEATH, A_STOP},
    [S_TRANSLATING] [E_MAJ_BEACONS_CHANNEL] = {S_SCANNING, A_MAJ_BEACONS_CHANNELS}
};

State_RECEIVER myState;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions static

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**

 * @fn static void Receiver_getAllBeaconsChannel()

 * @brief methode privee permettant de memoriser dans beaconsChannel l'ensemble des trames des balises

 */

static void Receiver_getAllBeaconsChannel(){

}

/**

 * @fn static void Receiver_translateChannelToBeaconsSignal(BeaconsChannel channel[])()

 * @brief methode privee permettant traduire les trames memorisees dans beaconsChannel

 * @param Tableau de BeaconsChannel a traduire

 */

static void Receiver_translateChannelToBeaconsSignal(BeaconsChannel channel[]){
    Event_RECEIVER myEvent = E_TRANSLATE_BEACONS_CHANNELS;
    Receiver_run(myEvent);
}

/**

 * @fn static void Receiver_run() TODO

 */

static void Receiver_run(Event_RECEIVER event){

    Action_RECEIVER action = stateMachine[myState][event].action;
	myState =  stateMachine[myState][event].destinationState;
    Receiver_performAction(action);

}

/**

 * @fn static void Receiver_performAction()

 * @brief methode privee permettant de definir les actions a effectuer

 * @param TODO

 */

static void Receiver_performAction(Action_RECEIVER action){
    switch (action) {
        case A_STOP:
            Receiver_free();
            break;

        case A_SEND_BEACONS_SIGNAL:
            //Scanner_setAllBeaconsSignal();
            //Thread getChannel()
            
            break;

        case A_TRANSLATE_BEACONS_CHANNELS:
            Receiver_translateChannelToBeaconsSignal(beaconsChannel);
            //Thread getChannel()
            break;

        case A_MAJ_BEACONS_CHANNELS:
            Receiver_getAllBeaconsChannel();
            //Timer TMAJ
            //Timer TScan
            break;

        default:
            break;
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions extern

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


extern void Receiver_new(){
    myState = S_SCANNING;
}

extern void Receiver_ask4StartReceiver(){
    Event_RECEIVER myEvent = E_ASK_BEACONS_SIGNAL;
    Receiver_run(myEvent);
}

extern void Receiver_ask4StopReceiver(){
    Event_RECEIVER myEvent = E_STOP;
    Receiver_run(myEvent);
}

extern void Receiver_ask4BeaconsSignal(){
    Event_RECEIVER myEvent = E_ASK_BEACONS_SIGNAL;
    Receiver_run(myEvent);
}

extern void Receiver_free(){

}