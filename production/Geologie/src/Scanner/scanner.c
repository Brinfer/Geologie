/**
 * @file scanner.c
 * @version 2.0
 * @date 7/06/2021
 * @author Gabriel LECENNE
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */


//check pointeur de tableau
//check sizeof()
//retrouver nb_beacons_available a partir de receiver

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <pthread.h>
#include <mqueue.h>
#include <errno.h>
#include <stdbool.h>

#include "../tools.h"
#include "../common.h"
#include "../Receiver/receiver.h"
#include "../Geographer/geographer.h"
#include "../Bookkeeper/bookkeeper.h"
#include "../Watchdog/watchdog.h"
#include "scanner.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MQ_MAX_MESSAGES (5)
#define MAX_BEACONS_COEFFICIENTS (30) //Check
#define NB_BEACONS_MAX (10)
#define BEACON_ID_LENGTH (3)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Variable et structure privee

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static BeaconData* beaconsData;
static Position currentPosition;
static ProcessorAndMemoryLoad currentProcessorAndMemoryLoad;
static BeaconCoefficients* beaconsCoefficients;
static BeaconSignal* beaconsSignal;
static CalibrationData* calibrationData;
static uint32_t beaconsIds[NB_BEACONS_MAX];
static uint32_t nbBeaconsCoefficients;
static uint32_t nbBeaconsAvailable;
static uint32_t beaconsCoefficientsindex;

typedef enum {
    S_FORGET,
    S_DEATH,
    S_BEGINNING,
    S_WAITING_DATA_BEACONS,
    S_COMPUTE_POSITION,
    S_COMPUTE_LOAD,
    // S_COMPUTE_CALIBRATION_POSITION,
    // S_COMPUTE_CALIBRATION_AVERAGE,
    NB_STATE
}State_SCANNER;

typedef enum {
    E_STOP = 0,
    //E_ASK_BEACONS_SIGNAL,
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
    A_ASK_CALIBRATION_FROM_POSITION_TIMER,
    A_ASK_CALIBRATION_AVERAGE,
    A_ASK_CALIBRATION_AVERAGE_TIMER,
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
    [S_BEGINNING][E_STOP] = {S_DEATH, A_STOP},

    [S_WAITING_DATA_BEACONS][E_SET_BEACONS_SIGNAL] = {S_COMPUTE_POSITION, A_SET_CURRENT_POSITION},
    [S_WAITING_DATA_BEACONS][E_STOP] = {S_DEATH, A_STOP},
    [S_WAITING_DATA_BEACONS][E_ASK_UPDATE_COEF_FROM_POSITION] = {S_WAITING_DATA_BEACONS, A_ASK_CALIBRATION_FROM_POSITION},
    [S_WAITING_DATA_BEACONS][E_ASK_AVERAGE_CALCUL] = {S_WAITING_DATA_BEACONS, A_ASK_CALIBRATION_AVERAGE},

    [S_COMPUTE_POSITION][E_SET_PROCESSOR_AND_MEMORY] = {S_COMPUTE_LOAD, A_SET_CURRENT_PROCESSOR_AND_MEMORY},
    [S_COMPUTE_POSITION][E_STOP] = {S_DEATH, A_STOP},
    [S_COMPUTE_POSITION][E_ASK_UPDATE_COEF_FROM_POSITION] = {S_COMPUTE_POSITION, A_ASK_CALIBRATION_FROM_POSITION},
    [S_COMPUTE_POSITION][E_ASK_AVERAGE_CALCUL] = {S_COMPUTE_POSITION, A_ASK_CALIBRATION_AVERAGE},

    [S_COMPUTE_LOAD][E_TIME_OUT] = {S_WAITING_DATA_BEACONS, A_ASK_BEACONS_SIGNAL},
    [S_COMPUTE_LOAD][E_STOP] = {S_DEATH, A_STOP},
    [S_COMPUTE_LOAD][E_ASK_UPDATE_COEF_FROM_POSITION] = {S_COMPUTE_LOAD, A_ASK_CALIBRATION_FROM_POSITION_TIMER},
    [S_COMPUTE_LOAD][E_ASK_AVERAGE_CALCUL] = {S_COMPUTE_LOAD, A_ASK_CALIBRATION_AVERAGE_TIMER},

};

typedef struct {
    Event_SCANNER event;
    BeaconSignal* beaconsSignal;
    ProcessorAndMemoryLoad currentProcessorAndMemoryLoad;
    CalibrationPosition calibrationPosition;
    uint32_t nbBeaconsAvailable;
}MqMsgScanner;

static State_SCANNER myState;
static pthread_t myThreadMq;

static const char BAL[] = "/BALScanner";
static mqd_t descripteur;
static struct mq_attr attr;

static Watchdog* wtd_TMaj;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions privee

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
 * @fn static void sendMsg(MqMsgScanner* this)
 * @brief Envoie des messages a la BAL
 *
 * @param this structure du message envoye a la BAL
 * @return renvoie 1 si une erreur est detectee, sinon 0
*/
static void sendMsg(MqMsgScanner* msg);

/**
 * @fn static void mqReceive(MqMsgScanner* this)
 * @brief Va chercher les messages dans la BAL
 *
 * @param msg structure du message a recuperer
 * @return renvoie 1 si une erreur est detectee, sinon 0
*/
static void mqReceive(MqMsgScanner* this);

/**
 * @fn translateBeaconsSignalToBeaconsData(BeaconSignal * beaconsSignal, BeaconData * dest)
 * @brief Traduit un tableau de BeaconSignal en un tableau de BeaconData
 *
 * @param beaconsSignal pointeur vers le tableau de beaconsSignal
 * @param dest pointeur du tableau de BeaconData
*/
static void translateBeaconsSignalToBeaconsData(BeaconSignal* beaconsSignal, BeaconData* dest);

/**
 * @fn static void sortBeaconsCoefficientId(BeaconCoefficients * beaconsCoefficients)
 * @brief recupère l'ensemble des identifiants des balises scannees
 *
 * @param beaconsCoefficients pointeur vers un tableau de beaconCoefficient a partir duquel seront extraits les identifiants
*/
static void sortBeaconsCoefficientId(BeaconCoefficients* beaconsCoefficients);

/**
 * @fn static void perform_setCurrentPosition(MqMsgScanner * msg)
 * @brief perform_action dans le cas de A_SET_CURRENT_POSITION
 *
 * @param msg message qui contient les donnees necessaire a l'execution de la fonction
*/
static void perform_setCurrentPosition(MqMsgScanner* msg);

/**
 * @fn perform_setCurrentProcessorAndMemoryLoad(MqMsgScanner * msg)
 * @brief perform_action dans le cas de A_SET_CURRENT_PROCESSOR_AND_MEMORY
 *
 * @param msg message qui contient les donnees necessaire a l'execution de la fonction
*/
static void perform_setCurrentProcessorAndMemoryLoad(MqMsgScanner* msg);

/**
 * @fn static void perform_askCalibrationFromPosition(MqMsgScanner * msg)
 * @brief perform_action dans le cas de A_ASK_CALIBRATION_FROM_POSITION
 *
 * @param msg message qui contient les donnees necessaire a l'execution de la fonction
*/
static void perform_askCalibrationFromPosition(MqMsgScanner* msg);

/**
 * @fn static void perform_askCalibrationAverage(MqMsgScanner * msg)
 * @brief perform_action dans le cas de A_ASK_CALIBRATION_AVERAGE
 *
 * @param msg message qui contient les donnees necessaire a l'execution de la fonction
*/
static void perform_askCalibrationAverage(MqMsgScanner* msg);

/**
 * @fn static void perform_stop()
 * @brief perform_action dans le cas de A_STOP
*/
static void perform_stop();

/**
 * @fn static void perform_askBeaconsSignal()
 * @brief perform_action dans le cas de A_ASK_BEACONS_SIGNAL
*/
static void perform_askBeaconsSignal();

/**
 * @fn static void performAction(Action_SCANNER action, MqMsgScanner * msg)
 * @brief execute les fonctions a realiser en fonction du parametre action
 *
 * @param action action a executer
 * @param msg message qui contient les donnees necessaire a l'execution de la fonction
*/
static void scanner_performAction(Action_SCANNER action, MqMsgScanner* msg);

/**
 * @fn static void * run()
 * @brief thread qui lit la BAL et met a jour l'action a realiser
*/
static void* run();





static void mqInit() {

    attr.mq_flags = 0; //Flags de la file
    attr.mq_maxmsg = MQ_MAX_MESSAGES; //Nombre maximum de messages dans la file
    attr.mq_msgsize = sizeof(MqMsgScanner); //Taille maximale de chaque message
    attr.mq_curmsgs = 0; //Nombre de messages actuellement dans la file

    /* destruction de la BAL si toutefois préexistante */

    mq_unlink(BAL);


    /* création et ouverture de la BAL */

    descripteur = mq_open(BAL, O_RDWR | O_CREAT, 0777, &attr);

    /*On ouvre la BAL avec comme arguments : le nom de la BAL, un flag pour ouvrir la file en lecture et en ecriture (pour utiliser recEve et send) ou pour créer une MQ, le droit d'accès,l'attribut crée*/

    if (descripteur == -1) {
        perror("Erreur Open :\n");
    }
    TRACE("[Scanner] BAL init%s", "\n");
}

static void sendMsg(MqMsgScanner* msg) {
    mq_send(descripteur, (char*) msg, sizeof(MqMsgScanner), 0);
}

static void mqReceive(MqMsgScanner* msg) {
    mq_receive(descripteur, (char*) msg, sizeof(MqMsgScanner), NULL);
}

static void translateBeaconsSignalToBeaconsData(BeaconSignal* beaconsSignal, BeaconData* dest) {
    uint32_t i;
    uint32_t j;
    for (i = 0; i < nbBeaconsAvailable; i++) {
        BeaconData data;
        memcpy(data.ID, beaconsSignal[i].name, BEACON_ID_LENGTH);
        data.position = beaconsSignal[i].position;
        data.power = beaconsSignal[i].rssi;
        data.coefficientAverage = 3;

        for (j = 0; j < nbBeaconsAvailable; j++) {
            if (strcmp(data.ID, calibrationData[j].beaconId) == 0) {
                data.coefficientAverage = calibrationData[j].coefficientAverage;
            }
        }

        dest[i] = data;
    }
}

static void sortBeaconsCoefficientId(BeaconCoefficients* beaconsCoefficients) {
    TRACE("[Scanner] sortCoefficient%s", "\n");
    uint32_t index_beaconCoef;
    uint32_t index_beaconsIds = 0;
    uint32_t j;
    bool idFind;
    for (index_beaconCoef = 0; index_beaconCoef < nbBeaconsCoefficients; index_beaconCoef++) {
        if (index_beaconCoef == 0) {
            beaconsIds[index_beaconsIds] = beaconsCoefficients[index_beaconCoef].beaconId[2];
            index_beaconsIds++;
        } else {
            idFind = true;
            for (j = 0; j < nbBeaconsAvailable; j++) {
                if (beaconsCoefficients[index_beaconCoef].beaconId[2] != beaconsIds[j]) {
                    idFind = false;
                }
            }
            if (idFind == false) {
                beaconsIds[index_beaconsIds] = beaconsCoefficients[index_beaconCoef].beaconId[2];
                index_beaconsIds++;
            }
        }
    }
}



static void perform_setCurrentPosition(MqMsgScanner* msg) {
    nbBeaconsAvailable = msg->nbBeaconsAvailable;

    beaconsData = malloc(nbBeaconsAvailable * sizeof(BeaconData));

    translateBeaconsSignalToBeaconsData(msg->beaconsSignal, beaconsData);
    Mathematician_getCurrentPosition(beaconsData, nbBeaconsAvailable, &currentPosition);
    Bookkeeper_ask4CurrentProcessorAndMemoryLoad();

}

static void perform_setCurrentProcessorAndMemoryLoad(MqMsgScanner* msg) {
    currentProcessorAndMemoryLoad = msg->currentProcessorAndMemoryLoad;

    Geographer_dateAndSendData(beaconsData, nbBeaconsAvailable, &(currentPosition), &(currentProcessorAndMemoryLoad));

    Watchdog_start(wtd_TMaj);

}

static void perform_askCalibrationFromPosition(MqMsgScanner* msg) {
    if (beaconsCoefficients == NULL) { //on fait ca pour pas redefinir a chaque fois qu'on retourne dans cette methode
        nbBeaconsCoefficients = 25 * nbBeaconsAvailable;
        beaconsCoefficients = malloc(nbBeaconsCoefficients * sizeof(BeaconCoefficients)); //TODO TODO TODO METTRE UN DEFINE
        beaconsCoefficientsindex = 0;
    }

    for (uint32_t index = 0; index < nbBeaconsAvailable; index++) {
        BeaconCoefficients coef;
        memcpy(coef.beaconId, beaconsData[index].ID, sizeof(beaconsData[index].ID));
        coef.positionId = msg->calibrationPosition.id;
        coef.attenuationCoefficient = Mathematician_getAttenuationCoefficient(&(beaconsData[index].power), &(beaconsData[index].position), &(msg->calibrationPosition));
        beaconsCoefficients[beaconsCoefficientsindex] = coef;
        beaconsCoefficientsindex++;
    }
    Geographer_signalEndUpdateAttenuation();
}

static void perform_askCalibrationAverage(MqMsgScanner* msg) {
    sortBeaconsCoefficientId(beaconsCoefficients);
    for (uint32_t index_balise = 0; index_balise < nbBeaconsAvailable; index_balise++) {
        CalibrationData cd;
        uint32_t index_coef = 0;
        for (uint32_t j = 0; j < nbBeaconsCoefficients; j++) {
            if (beaconsCoefficients[j].beaconId[2] == beaconsIds[index_balise]) {
                index_coef++;
            }
        }

        BeaconCoefficients* coef = calloc(1, index_coef);

        for (uint32_t j = 0; j < nbBeaconsCoefficients; j++) {
            if (beaconsCoefficients[j].beaconId[2] == beaconsIds[index_balise]) {
                coef[j] = beaconsCoefficients[j];
                memcpy(cd.beaconId, beaconsCoefficients[j].beaconId, sizeof(beaconsCoefficients[j].beaconId));
            }
        }
    }
    free(beaconsCoefficients);
    beaconsCoefficients = NULL;
    Geographer_signalEndAverageCalcul(calibrationData, nbBeaconsAvailable * sizeof(calibrationData)); //TODO
}
static void perform_askCalibrationFromPositionTimer(MqMsgScanner* msg) {
    Watchdog_cancel(wtd_TMaj);
    perform_askCalibrationFromPosition(msg);
    Watchdog_start(wtd_TMaj);
}
static void perform_askCalibrationAverageTimer(MqMsgScanner* msg) {
    Watchdog_cancel(wtd_TMaj);
    perform_askCalibrationAverage(msg);
    Watchdog_start(wtd_TMaj);
}


static void perform_stop() {
    Receiver_ask4StopReceiver();
    Bookkeeper_askStopBookkeeper();
}

static void perform_askBeaconsSignal() {
    Receiver_ask4BeaconsSignal();

}

static void scanner_performAction(Action_SCANNER action, MqMsgScanner* msg) {
    switch (action) {
        case A_NOP:
            break;
        case A_ASK_CALIBRATION_FROM_POSITION_TIMER:
            perform_askCalibrationFromPositionTimer(msg);
            break;

        case A_ASK_CALIBRATION_AVERAGE_TIMER:
            perform_askCalibrationAverageTimer(msg);
            break;
        case A_STOP:
            perform_stop();
            break;
        case A_ASK_BEACONS_SIGNAL:
            perform_askBeaconsSignal();
            break;

        case A_SET_CURRENT_POSITION:
            perform_setCurrentPosition(msg);
            break;

        case A_SET_CURRENT_PROCESSOR_AND_MEMORY:
            perform_setCurrentProcessorAndMemoryLoad(msg);
            break;

        case A_ASK_CALIBRATION_FROM_POSITION:
            perform_askCalibrationFromPosition(msg);
            break;

        case A_ASK_CALIBRATION_AVERAGE:
            perform_askCalibrationAverage(msg);
            break;

        default:
            break;
    }
}


/**
 * @fn static void ScannerTime_out()
 * @brief fonction de callback du watchdog wtd_TMaj
*/
#ifndef _TESTING_MODE
static void ScannerTime_out()
#else                                           //cas ou l'on teste
void ScannerTime_out();
void __real_ScannerTime_out()
#endif
{
    TRACE("[Scanner] Time Out%s", "\n");
    MqMsgScanner msg = {
        .event = E_TIME_OUT
    };
    sendMsg(&msg);
}

#ifndef _TESTING_MODE
static void Scanner_transitionFct(MqMsgScanner msg)
#else                                           //cas ou l'on teste
void Scanner_transitionFct(MqMsgScanner msg);
void __real_Scanner_transitionFct(MqMsgScanner msg)
#endif
{
    Action_SCANNER action;
    State_SCANNER nextState;

    action = stateMachine[myState][msg.event].action;
    nextState = stateMachine[myState][msg.event].destinationState;

    if (nextState != S_FORGET) {
        scanner_performAction(action, &msg);
        myState = nextState;
    } else {
        TRACE("Scanner lost an event%s", "\n");
    }
}
/**
 * @fn TODO
 * @brief TODO
*/

static void* run() {

    MqMsgScanner msg;

    while (myState != S_DEATH) {
        mqReceive(&msg);
        if (msg.event == E_STOP) {
            perform_stop();
        } else if (msg.event > NB_EVENT_SCANNER) {

        } else {
            Scanner_transitionFct(msg);
        }
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void Scanner_new() {
    mqInit();
    wtd_TMaj = Watchdog_construct(1 * 1000 * 1000, &(ScannerTime_out));
    Receiver_new();
    Bookkeeper_new();

    beaconsCoefficients = malloc(sizeof(beaconsCoefficients[25]));
    beaconsSignal = malloc(sizeof(beaconsSignal[3]));
    calibrationData = malloc(sizeof(CalibrationData[25]));

}


extern void Scanner_free() {
    myState = S_DEATH;
    Watchdog_destroy(wtd_TMaj);
    Receiver_free();
    Bookkeeper_free();
}


extern void Scanner_ask4StartScanner() {
    myState = S_BEGINNING;
    Receiver_ask4StartReceiver();
    Bookkeeper_askStartBookkeeper();
    sleep(1);
    Watchdog_start(wtd_TMaj);
    pthread_create(&myThreadMq, NULL, &run, NULL);

}

extern void Scanner_ask4StopScanner() {
    MqMsgScanner msg = {
            .event = E_STOP
    };

    sendMsg(&msg);
    pthread_join(myThreadMq, NULL);
}


extern void Scanner_ask4UpdateAttenuationCoefficientFromPosition(const CalibrationPosition* calibrationPosition) {
    MqMsgScanner msg = {
                .event = E_ASK_UPDATE_COEF_FROM_POSITION,
                .calibrationPosition = *calibrationPosition
    };

    sendMsg(&msg);
}


extern void Scanner_ask4AverageCalcul() {
    MqMsgScanner msg = {
                .event = E_ASK_AVERAGE_CALCUL
    };

    sendMsg(&msg);
}


extern void Scanner_setAllBeaconsSignal(BeaconSignal* beaconsSignal, uint32_t nbBeaconsAvailable) {
    MqMsgScanner msg = {
                .event = E_SET_BEACONS_SIGNAL,
                .beaconsSignal = beaconsSignal,
                .nbBeaconsAvailable = nbBeaconsAvailable
    };

    sendMsg(&msg);
}


extern void Scanner_setCurrentProcessorAndMemoryLoad(ProcessorAndMemoryLoad currentPAndMLoad) {
    MqMsgScanner msg = {
                .event = E_SET_PROCESSOR_AND_MEMORY,
                .currentProcessorAndMemoryLoad = currentPAndMLoad
    };

    sendMsg(&msg);
}
