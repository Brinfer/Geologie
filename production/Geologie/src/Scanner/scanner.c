/**


 * @file scanner.c


 *


 * @version 2.0


 * @date 7/06/2021


 * @author Gabriel LECENNE


 * @copyright Geo-Boot


 * @license BSD 2-clauses


 */

#include <stdlib.h>
#include "../common.h"
#include "../Receiver/receiver.h"
#include "../Geographer/geographer.h"
#include "../Bookkeeper/bookkeeper.h"
#include "scanner.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Variable et structure privee

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static BeaconsData beaconsData[];
static Position currentPosition;
static ProcessorAndMemoryLoad currentProcessorAndMemoryLoad;
static BeaconCoefficients beaconsCoefficient[];
static BeaconsSignal beaconSignal[];

typedef enum{
    S_BEGINNING = 0,
    S_WAITING_DATA_BEACONS,
    S_COMPUTE_POSITION,
    S_COMPUTE_LOAD,
    NB_STATE
}State_SCANNER;

typedef enum {
    E_STOP = 0,
    E_SET_BEACONS_SIGNAL,
    E_SET_PROCESSOR_AND_MEMORY,
    E_T_MAJ_DONE,
    NB_EVENT_SCANNER
} Event_SCANNER;

typedef enum {
    A_NOP = 0,
    A_STOP,
    A_ASK_BEACONS_SIGNAL,
    A_SET_CURRENT_POSITION,
    A_SET_CURRENT_PROCESSOR_AND_MEMORY,
    NB_ACTION_SCANNER
} Action_SCANNER;

typedef struct {
    State_SCANNER destinationState;
    Action_SCANNER action;
} Transition_SCANNER;

static Transition_SCANNER stateMachine[NB_STATE - 1][NB_EVENT_SCANNER] =
{
    [S_BEGINNING] [E_T_MAJ_DONE] = {S_WAITING_DATA_BEACONS, A_ASK_BEACONS_SIGNAL},
    [S_WAITING_DATA_BEACONS] [E_SET_BEACONS_SIGNAL] = {S_COMPUTE_POSITION, A_SET_CURRENT_POSITION},
    [S_COMPUTE_POSITION] [E_SET_PROCESSOR_AND_MEMORY] = {S_COMPUTE_LOAD, A_SET_CURRENT_PROCESSOR_AND_MEMORY},
    [S_COMPUTE_LOAD] [E_T_MAJ_DONE] = {S_WAITING_DATA_BEACONS, A_ASK_BEACONS_SIGNAL}
    // AJouter les STOP
};

static State_SCANNER myState;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions static

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**


 * @fn TODO


 * @brief TODO


 */

static void Scanner_performAction(Action_SCANNER action){

    switch (action) {
        case A_STOP:
            Receiver_free();
            break;

        case A_ASK_BEACONS_SIGNAL:
            Receiver_ask4BeaconsSignal();        
            break;

        case A_SET_CURRENT_POSITION:
            MathematicianLOG_getCurrentPosition(beaconsData);
            Bookkeeper_ask4CurrentProcessorAndMemoryLoad();
            break;

        case A_SET_CURRENT_PROCESSOR_AND_MEMORY:
            Geographer_dateAndSendData(beaconsData, currentPosition, currentProcessorAndMemoryLoad);
            break;

        default:
            break;
    }

}

/**

 * @fn TODO


 * @brief TODO


 */

static void Scanner_run(Event_SCANNER event){

    Action_SCANNER action = stateMachine[myState][event].action;
	myState =  stateMachine[myState][event].destinationState;
    Receiver_performAction(action);

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions extern

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void Scanner_ask4StartScanner(){
    myState = S_BEGINNING;
    //lancer le timer TMAJ : fonction de retour = ask4AllBeaconsSignal()
}

extern void Scanner_ask4StopScanner(){

}


extern void Scanner_ask4UpdateAttenuationCoefficientFromPosition(Position calibrationPosition){

    int index;
    for(index = 0; index < sizeof(beaconsData); index++){
        beaconsCoefficient[index] = MathematicianLOG_getAttenuationCoefficientFromPosition(beaconsData[index].power, beaconsData[index].Position, calibrationPosition);
    }

    Geographer_signalEndUpdateAttenuation();

}


extern void Scanner_ask4AverageCalcul(){

    /*int index;
    for(index = 0; index < sizeof(beaconsData); index++){
         MathematicianLOG_getAttenuationCoefficientFromPosition();
    }

    Geographer_signalEndAverageCalcul(calibrationData);*/ //TODO

}


extern void Scanner_setAllBeaconsSignal(BeaconsSignal beaconsSignal[]){
    
    int index;
    for(index = 0; index < sizeof(beaconsSignal); index++){
        beaconSignal[index] = beaconsSignal[index];
    }

    Event_SCANNER myEvent = E_SET_BEACONS_SIGNAL;
    Scanner_run(myEvent);    
}


extern void Scanner_setCurrentProcessorAndMemoryLoad(ProcessorAndMemoryLoad currentPAndMLoad){

    currentProcessorAndMemoryLoad = currentPAndMLoad;

    Event_SCANNER myEvent = E_SET_PROCESSOR_AND_MEMORY;
    Scanner_run(myEvent);

}