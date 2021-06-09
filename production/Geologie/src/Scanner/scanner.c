/**


 * @file scanner.c


 * @version 2.0


 * @date 7/06/2021


 * @author Gabriel LECENNE


 * @copyright Geo-Boot


 * @license BSD 2-clauses


 */

#include <stdlib.h>
#include <pthread.h>
#include <mqueue.h>
#include <errno.h>

#include "../common.h"
#include "../Receiver/receiver.h"
#include "../Geographer/geographer.h"
#include "../Bookkeeper/bookkeeper.h"
#include "../Watchdog/watchdog.h"
#include "scanner.h"

#define MQ_MAX_MESSAGES (5)
#define MAX_BEACONS_SIGNAL 10
#define MAX_BEACONS_COEFFICIENTS 10
#define MAX_BEACONS_DATA 10

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Variable et structure privee

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static BeaconData * beaconsData;
static Position currentPosition;
static ProcessorAndMemoryLoad currentProcessorAndMemoryLoad;
static BeaconCoefficients * beaconsCoefficient;
static BeaconSignal * beaconSignal;
//static CalibrationData * calibrationData;

typedef enum{
    S_DEATH = 0,
    S_BEGINNING,
    S_WAITING_DATA_BEACONS,
    S_COMPUTE_POSITION,
    S_COMPUTE_LOAD,
    S_COMPUTE_CALIBRATION_POSITION,
    S_COMPUTE_CALIBRATION_AVERAGE,
    NB_STATE
}State_SCANNER;

typedef enum {
    E_STOP = 0,
    E_ASK_BEACONS_SIGNAL,
    E_ASK_UPDATE_COEF_FROM_POSITION,
    E_ASK_AVERAGE_CALCUL,
    E_SET_BEACONS_SIGNAL,
    E_SET_PROCESSOR_AND_MEMORY,
    E_TIME_OUT,
    NB_EVENT_SCANNER
} Event_SCANNER;

typedef enum {
    A_NOP = 0,
    A_STOP,
    A_ASK_CALIBRATION_FROM_POSITION,
    A_ASK_CALIBRATION_AVERAGE,
    A_ASK_BEACONS_SIGNAL,
    A_SET_CURRENT_POSITION,
    A_SET_CURRENT_PROCESSOR_AND_MEMORY,
    NB_ACTION_SCANNER
} Action_SCANNER;

typedef struct {
    State_SCANNER destinationState;
    Action_SCANNER action;
} Transition_SCANNER;

static Transition_SCANNER stateMachine[NB_STATE][NB_EVENT_SCANNER] =
{
    [S_BEGINNING] [E_TIME_OUT] = {S_WAITING_DATA_BEACONS, A_ASK_BEACONS_SIGNAL},
    [S_WAITING_DATA_BEACONS] [E_SET_BEACONS_SIGNAL] = {S_COMPUTE_POSITION, A_SET_CURRENT_POSITION},
    [S_COMPUTE_POSITION] [E_SET_PROCESSOR_AND_MEMORY] = {S_COMPUTE_LOAD, A_SET_CURRENT_PROCESSOR_AND_MEMORY},
    [S_COMPUTE_LOAD] [E_TIME_OUT] = {S_WAITING_DATA_BEACONS, A_ASK_BEACONS_SIGNAL},
    [S_WAITING_DATA_BEACONS] [E_ASK_UPDATE_COEF_FROM_POSITION] = {S_COMPUTE_CALIBRATION_POSITION, A_ASK_CALIBRATION_FROM_POSITION},
    [S_COMPUTE_POSITION] [E_ASK_UPDATE_COEF_FROM_POSITION] = {S_COMPUTE_CALIBRATION_POSITION, A_ASK_CALIBRATION_FROM_POSITION},
    [S_COMPUTE_LOAD] [E_ASK_UPDATE_COEF_FROM_POSITION] = {S_COMPUTE_CALIBRATION_POSITION, A_ASK_CALIBRATION_FROM_POSITION},   
    [S_WAITING_DATA_BEACONS] [E_ASK_AVERAGE_CALCUL] = {S_COMPUTE_CALIBRATION_AVERAGE, A_ASK_CALIBRATION_AVERAGE},
    [S_COMPUTE_POSITION] [E_ASK_AVERAGE_CALCUL] = {S_COMPUTE_CALIBRATION_AVERAGE, A_ASK_CALIBRATION_AVERAGE},
    [S_COMPUTE_LOAD] [E_ASK_AVERAGE_CALCUL] = {S_COMPUTE_CALIBRATION_AVERAGE, A_ASK_CALIBRATION_AVERAGE}
};

typedef struct {
    Event_SCANNER event;
    BeaconSignal * beaconsSignal;
    ProcessorAndMemoryLoad currentProcessorAndMemoryLoad;
    CalibrationPosition calibrationPosition;
} MqMsg;

static State_SCANNER myState;
static pthread_t myThread;

static const char BAL[] = "/BALScanner";
static mqd_t descripteur;
static struct mq_attr attr;

Watchdog * wtd_TMaj;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions static

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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

static void sendMsg(MqMsg* msg) {

    mq_send(descripteur, (char*) &msg, sizeof(msg), 0);

}

static void mqReceive(MqMsg* this) {

    mq_receive(descripteur, (char*) this, sizeof(*this), NULL);

}

static BeaconData * translateBeaconsSignalToBeaconsData(BeaconSignal * beaconSignal){
    /*BeaconData * bd;
    BeaconData data[sizeof(beaconSignal)]; //revoir size of beaconSignal
    int index;
    int i;
    for (index = 0; index < sizeof(beaconSignal); index++) //revoir size of beaconSignal
    {
        data[index].ID = beaconSignal[index].name;
        data[index].position = beaconSignal[index].position;
        data[index].power = beaconSignal[index].rssi;
        for (i = 0; i < sizeof(calibrationData); i++) //revoir calibrationData
        {
            if(calibrationData[i].beaconId == data[index].ID)
            {
                data[index].coefficientAverage = calibrationData[i].coefficientAverage;
            }
        }
        bd = *(data);
        return bd;*/
        return 0;
}


/**


 * @fn TODO


 * @brief TODO


 */

static void performAction(Action_SCANNER action, MqMsg * msg){

    switch (action) {
        case A_STOP:
            Receiver_free();
            break;

        case A_ASK_BEACONS_SIGNAL:
            Receiver_ask4BeaconsSignal();
            break;

        case A_SET_CURRENT_POSITION:
            beaconSignal = msg->beaconsSignal;
            beaconsData = translateBeaconsSignalToBeaconsData(msg->beaconsSignal);
            Mathematician_getCurrentPosition(beaconsData);
            Bookkeeper_ask4CurrentProcessorAndMemoryLoad();
            break;

        case A_SET_CURRENT_PROCESSOR_AND_MEMORY:
            currentProcessorAndMemoryLoad = msg->currentProcessorAndMemoryLoad;
            Geographer_dateAndSendData(beaconsData, sizeof(BeaconData), &(currentPosition), &(currentProcessorAndMemoryLoad)); //revoir 2è arg
            MqMsg message = { 
                        .event = E_ASK_BEACONS_SIGNAL
                        };
            sendMsg(&message);
            Watchdog_start(wtd_TMaj);
            break;

        case A_ASK_CALIBRATION_FROM_POSITION:
            /*for(int index = 0; index < sizeof(beaconsData); index++){
                Mathematician_getAttenuationCoefficient(&(beaconsData[index].power), &(beaconsData[index].position), &(msg->calibrationPosition)); //alimente beaconsCoeffcicient
            }
            Geographer_signalEndUpdateAttenuation();*/
            break;

        case A_ASK_CALIBRATION_AVERAGE:
            /*for(int index = 0; index < sizeof(beaconsCoefficient); index++){
                Mathematician_getAverageCalcul(&(beaconsCoefficient[index])); //va dans calibrationData
            }
            //Geographer_signalEndAverageCalcul(calibrationData, ); //TODO calibrationData et demander pour nbCalibration*/
            break;

        default:
            break;
    }

}

/**

 * @fn TODO


 * @brief TODO


 */

static void * run(){
    
    MqMsg msg;

    Action_SCANNER action;

    while (myState != S_DEATH) {

        mqReceive(&msg); 
        action = stateMachine[myState][msg.event].action;
        performAction(action, &msg);
        myState =  stateMachine[myState][msg.event].destinationState;

    }
        
   return 0;
}

static void time_out(){

    MqMsg msg = { 
                .event = E_TIME_OUT
                };

    sendMsg(&msg);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions extern

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void Scanner_new(){
    wtd_TMaj = Watchdog_construct(1000000, &(time_out));
    myState = S_BEGINNING;
    Receiver_new();
    Bookkeeper_new();
}

extern void Scanner_free(){
    myState = S_DEATH;
    Watchdog_destroy(wtd_TMaj);
    Receiver_free();
    Bookkeeper_free();
}


extern void Scanner_ask4StartScanner(){
    printf("start\n");
    Receiver_ask4StartReceiver();
    Bookkeeper_askStartBookkeeper();
    mqInit();
    MqMsg msg = { 
                .event = E_ASK_BEACONS_SIGNAL
                };
    sendMsg(&msg);
    Watchdog_start(wtd_TMaj);
    pthread_create(&myThread, NULL, &run, NULL);
}

extern void Scanner_ask4StopScanner(){
    Receiver_ask4StopReceiver();
    Bookkeeper_askStopBookkeeper();
    pthread_join(myThread, NULL);
}


extern void Scanner_ask4UpdateAttenuationCoefficientFromPosition(CalibrationPosition calibrationPosition){

    MqMsg msg = { 
                .event = E_ASK_UPDATE_COEF_FROM_POSITION,
                .calibrationPosition = calibrationPosition
                };

    sendMsg(&msg);

}


extern void Scanner_ask4AverageCalcul(){
    
    MqMsg msg = { 
                .event = E_ASK_AVERAGE_CALCUL
                };

    sendMsg(&msg);

}


extern void Scanner_setAllBeaconsSignal(BeaconSignal * beaconsSignal){
    MqMsg msg = { 
                .event = E_SET_BEACONS_SIGNAL,
                .beaconsSignal = beaconsSignal
                };

    sendMsg(&msg);
}


extern void Scanner_setCurrentProcessorAndMemoryLoad(ProcessorAndMemoryLoad currentPAndMLoad){

    MqMsg msg = { 
                .event = E_SET_PROCESSOR_AND_MEMORY,
                .currentProcessorAndMemoryLoad = currentPAndMLoad
                };

    sendMsg(&msg);

}
