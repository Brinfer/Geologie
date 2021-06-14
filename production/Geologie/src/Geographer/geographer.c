/**
 * @file geographer.c
 *
 * @brief Est en quelque sorte le chef d'orchestre de Geologie
 *
 * @version 1.0
 * @date 03-06-2021
 * @author BRIENT Nathan
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NB_EXPERIMENTAL_POSITION (25)
#define NB_EXPERIMENTAL_TRAJECT (3)

#define MQ_MAX_MESSAGES (10)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <errno.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "geographer.h"
#include "../common.h"
#include "../tools.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief The name of the queue
 */
#define MQ_LABEL "/MQ_Geographer"

/**
 * @brief Les indicateurs de la boite au lettre de PostmanLOG.
 *
 */
#define MQ_FLAGS (O_CREAT | O_RDWR)

/**
 * @brief Les modes de la boite aux lettre de PostmanLOG.
 */
#define MQ_MODE (S_IRUSR | S_IWUSR)

/**
 * @brief Structure contenant les etat que peut prendre Geographer
 */
typedef enum {
    S_NONE = 0,

    S_DEATH,
    S_WATING_FOR_CONNECTION,
    S_IDLE,

    // Calibrating
    S_WAITING_FOR_BE_PLACED,
    S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION,
    S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT,
    S_TEST_IF_FINISH_ALL_POSITION,

    S_NB_STATE
} StateGeographer;

/**
 * @brief Structure contenant les evenement que geographer peut recevoir
 */
typedef enum {
    E_NONE = 0,

    E_STOP,
    E_CONNECTION_ESTABLISHED,
    E_CONNECTION_DOWN,

    E_DATE_AND_SEND_DATA,
    E_ASK_CALIBRATION_POSITIONS,
    E_VALIDATE_POSITION,
    E_SIGNAL_END_UPDATE_ATTENUATION,
    E_FINISH_CALIBRATE_ALL_POSITION,
    E_NOT_FINISH_CALIBRATE_ALL_POSITION,
    E_SIGNAL_END_AVERAGE_CALCUL,

    E_NB_EVENT
} EventGeographer;

typedef enum {
    A_NONE,

    A_STOP,
    A_SEND_EXPERIMENTAL_DATA,
    A_SEND_ALL_DATA,
    A_SET_CALIBRATION_DATA,
    A_INCREASE_CALIBRATION_COUNTER,
    A_SIGNAL_END_CALIBRATION_POSITION,
    A_ASK_AVERAGE_CALCUL,
    A_SET_CALIBRATION_POSITION,

    A_NB_ACTION,
} ActionGeographer;

typedef struct {
    EventGeographer event;

    union {
        struct {
            Position* position;
            ProcessorAndMemoryLoad* processorAndMemoryLoad;
            BeaconData* beaconsData;
            int8_t nbBeaconData;            // Same size but more readable in the rest of the program
        } current;
        struct {
            CalibrationData* calibrationData;   // Same size but more readable in the rest of the program
            int8_t nbCalibrationData;
        } calibration;

        CalibrationPositionId calibrationPositionId;
    }data;
} MqMsg;

/**
 * @brief Contient les informations necessaire a une transition entre les etats.
 */
typedef struct {
    StateGeographer destinationState;   /**< L'état suivant */
    ActionGeographer action;            /**< L'action à effectuer lors de la transition */
} TransitionGeographer;


/**
 * @brief Etat de la connexion.
 */
static ConnectionState connectionState;

static StateGeographer currentState;

static pthread_t geographerThread;

static mqd_t geographerMq;

/**
 * @brief Compteur de calibration.
 */
static uint8_t calibrationCounter;

/**
* @brief Tableau contenant les positions experimentales
*
*/
static const ExperimentalPosition experimentalPositions[NB_EXPERIMENTAL_POSITION] = {
    {.id = 1, .position = {.X = 550, .Y = 200}},
    {.id = 2, .position = {.X = 550, .Y = 400}},
    {.id = 3, .position = {.X = 550, .Y = 480}},
    {.id = 4, .position = {.X = 550, .Y = 600}},
    {.id = 5, .position = {.X = 550, .Y = 800}},
    {.id = 6, .position = {.X = 550, .Y = 1000}},
    {.id = 7, .position = {.X = 550, .Y = 1200}},
    {.id = 8, .position = {.X = 500, .Y = 1200}},
    {.id = 9, .position = {.X = 600, .Y = 1200}},
    {.id = 10, .position = {.X = 410, .Y = 1060}},
    {.id = 11, .position = {.X = 360, .Y = 970}},
    {.id = 12, .position = {.X = 260, .Y = 800}},
    {.id = 13, .position = {.X = 160, .Y = 630}},
    {.id = 14, .position = {.X = 150, .Y = 480}},
    {.id = 15, .position = {.X = 350, .Y = 480}},
    {.id = 16, .position = {.X = 750, .Y = 480}},
    {.id = 17, .position = {.X = 950, .Y = 480}},
    {.id = 18, .position = {.X = 930, .Y = 630}},
    {.id = 19, .position = {.X = 800, .Y = 800}},
    {.id = 20, .position = {.X = 750, .Y = 1000}},
    {.id = 21, .position = {.X = 650, .Y = 1150}},
    {.id = 22, .position = {.X = 950, .Y = 1300}},
    {.id = 23, .position = {.X = 150, .Y = 1300}},
    {.id = 24, .position = {.X = 150, .Y = 200}},
    {.id = 25, .position = {.X = 950, .Y = 200}}
};

/**
* @brief Tableau contenant les positions de calibration
*
*/
static const  CalibrationPosition calibrationPositions[NB_CALIBRATION_POSITION] = {
    {.id = 1, .position = {.X = 550, .Y = 200}},
    {.id = 2, .position = {.X = 550, .Y = 400}},
    {.id = 3, .position = {.X = 550, .Y = 480}},
    {.id = 4, .position = {.X = 550, .Y = 600}},
    {.id = 5, .position = {.X = 550, .Y = 800}},
    {.id = 6, .position = {.X = 550, .Y = 1000}},
    {.id = 7, .position = {.X = 550, .Y = 1200}},
    {.id = 8, .position = {.X = 500, .Y = 1200}},
    {.id = 9, .position = {.X = 600, .Y = 1200}},
    {.id = 10, .position = {.X = 410, .Y = 1060}},
    {.id = 11, .position = {.X = 360, .Y = 970}},
    {.id = 12, .position = {.X = 260, .Y = 800}},
    {.id = 13, .position = {.X = 160, .Y = 630}},
    {.id = 14, .position = {.X = 150, .Y = 480}},
    {.id = 15, .position = {.X = 350, .Y = 480}},
    {.id = 16, .position = {.X = 750, .Y = 480}},
    {.id = 17, .position = {.X = 950, .Y = 480}},
    {.id = 18, .position = {.X = 930, .Y = 630}},
    {.id = 19, .position = {.X = 800, .Y = 800}},
    {.id = 20, .position = {.X = 750, .Y = 1000}},
    {.id = 21, .position = {.X = 650, .Y = 1150}},
    {.id = 22, .position = {.X = 950, .Y = 1300}},
    {.id = 23, .position = {.X = 150, .Y = 1300}},
    {.id = 24, .position = {.X = 150, .Y = 200}},
    {.id = 25, .position = {.X = 950, .Y = 200}}
};

/**
* @brief Tableau contenant le premier trajet experimental
*
*/
static Position traject1[] = {
    {.X = 100, .Y = 1300 },
    {.X = 1000, .Y = 1300 },
    {.X = 1000, .Y = 200 },
    {.X = 100, .Y = 200 },
    {.X = 100, .Y = 1300}
};

/**
* @brief Tableau contenant le deuxieme trajet experimental
*
*/
static Position traject2[] = {
    {.X = 50, .Y = 1300 },
    {.X = 1050, .Y = 1000 }
};

/**
* @brief Tableau contenant le troisieme trajet experimental
*
*/
static Position traject3[] = {
    {.X = 950, .Y = 1000 },
    {.X = 350, .Y = 1200 },
    {.X = 50, .Y = 1000 }
};

/**
* @brief Tableau contenant les trajets experimentaux
*
*/
static const ExperimentalTraject experimentalTrajects[NB_EXPERIMENTAL_TRAJECT] = {
    {.id = 1, .traject = traject1, .nbPosition = 5},
    {.id = 2, .traject = traject2, .nbPosition = 2},
    {.id = 3, .traject = traject3, .nbPosition = 3}
};

static const TransitionGeographer stateMachine[S_NB_STATE][E_NB_EVENT] = {
    [S_WATING_FOR_CONNECTION] [E_CONNECTION_ESTABLISHED] = {S_IDLE, A_SEND_EXPERIMENTAL_DATA},
    [S_WATING_FOR_CONNECTION][E_STOP] = {S_DEATH, A_STOP},

    [S_IDLE][E_ASK_CALIBRATION_POSITIONS] = {S_WAITING_FOR_BE_PLACED, A_SET_CALIBRATION_POSITION},
    [S_IDLE][E_DATE_AND_SEND_DATA] = {S_IDLE, A_SEND_ALL_DATA},
    [S_IDLE][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION, A_NONE},
    [S_IDLE][E_STOP] = {S_DEATH, A_STOP},

    [S_WAITING_FOR_BE_PLACED][E_VALIDATE_POSITION] = {S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION, A_NONE},
    [S_WAITING_FOR_BE_PLACED][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION, A_NONE},   
    [S_WAITING_FOR_BE_PLACED][E_STOP] = {S_DEATH, A_STOP},

    [S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION][E_SIGNAL_END_UPDATE_ATTENUATION] = {S_TEST_IF_FINISH_ALL_POSITION, A_SIGNAL_END_CALIBRATION_POSITION},
    [S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION, A_NONE},
    [S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION][E_STOP] = {S_DEATH, A_STOP},

    [S_TEST_IF_FINISH_ALL_POSITION][E_FINISH_CALIBRATE_ALL_POSITION] = {S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT, A_ASK_AVERAGE_CALCUL},
    [S_TEST_IF_FINISH_ALL_POSITION][E_NOT_FINISH_CALIBRATE_ALL_POSITION] = {S_WAITING_FOR_BE_PLACED, A_INCREASE_CALIBRATION_COUNTER},
    [S_TEST_IF_FINISH_ALL_POSITION][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION, A_NONE},
    [S_TEST_IF_FINISH_ALL_POSITION][E_STOP] = {S_DEATH, A_STOP},

    [S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT][E_SIGNAL_END_AVERAGE_CALCUL] = {S_IDLE,A_SET_CALIBRATION_DATA},
    [S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION, A_NONE},
    [S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT][E_STOP] = {S_DEATH, A_STOP},
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Recupere la date ecoulee depuis le 1 er janvier 1970
 * *
 * @return renvoie la date actuelle en secondes
*/
static Date getCurrentDate();

/**
 * @brief initialise la boite aux lettres
 *
 * @return 1 si erreur detectee, sinon retourne 0
*/
static int8_t setUpMq();

/**
 * @brief] Ferme et detruit la boite aux lettres
 *
 * @return -1 si erreur detectee, sinon retourne 0
*/
static int8_t tearDoneMq();

/**
 * @brief] Ferme et detruit la boite aux lettres
 *
 * @return -1 si erreur detectee, sinon retourne 0
 * @param dest message a envoyer a la queue
*/
static int8_t readMsgMq(MqMsg* dest);

/**
 * @brief methode appelee par un thread pour recevoir les action a] Faire grace a une boite au lettre et les effectuer
 *
 * cette metode lira la boite au lettre et effectuera les actions
 */
static void* runGeographer();

/**
 * @brief methode pour envoyer des message a la queue
 *
 * @param sizeOfMsg taille du message a envoyer
 * @param msg structure contenant les donnees a utiliser pour effectuer l'action
 * @return 1 si erreur 0 si
 */
static int8_t sendMsgMq(MqMsg* msg);

static int8_t performAction(ActionGeographer action, const MqMsg* msg);

static int8_t signalFinishCalibrateAllPosition(void);

static int8_t signalNotFinishCalibrateAllPosition(void);

static int8_t actionNone(void);

static int8_t actionStop(void);

static int8_t actionSendExperimentalData(void);

static int8_t actionSendAllData(const BeaconData* beaconData, uint8_t nbBeaconData, const Position* position, const ProcessorAndMemoryLoad* processorAndMemoryLoad);

static int8_t actionSetCalibrationPosition(const CalibrationPosition* calibrationPosition, uint8_t nbCalibrationPosition);

static int8_t actionIncreaseCalibrationCounter(void);

static int8_t actionSignalEndCalibrationPosition(void);

static int8_t actionAskAverageCalcul(void);

static int8_t actionSetCalibrationData(const CalibrationData* calibrationData, uint8_t nbCalibrationData);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int8_t Geographer_new() {
    int8_t returnError;

    ProxyGUI_new();
    ProxyLoggerMOB_new();
    Scanner_new();

    returnError = setUpMq();
    ERROR(returnError < 0, "[Geographer] Error when seting up the queue ... Abandonnement");

    return returnError;
}

extern int8_t Geographer_free() {
    int8_t returnError;

    Scanner_free();
    ProxyLoggerMOB_free();
    ProxyGUI_free();

    returnError = tearDoneMq();
    ERROR(returnError < 0, "[Geographer] Error when tearring down the queue ... Abandonnement");

    return returnError;
}

extern int8_t Geographer_askSignalStartGeographer() {
    int8_t returnError;

    ProxyGUI_start();
    ProxyLoggerMOB_start();
    Scanner_ask4StartScanner();

    currentState = S_WATING_FOR_CONNECTION;

    returnError = pthread_create(&geographerThread, NULL, &runGeographer, NULL);
    ERROR(returnError < 0, "[Geographer] Error when creating the processus ... Abandonnement");

    return returnError;
}

extern int8_t Geographer_askSignalStopGeographer() {
    int8_t returnError;

    MqMsg msg = { .event = E_STOP };
    returnError = sendMsgMq(&msg);

    if (returnError < 0) {
        ERROR(true, "[Geographer] Fail to send the message stop ... Abandonnement");
    } else {
        returnError = pthread_join(geographerThread, NULL);
        ERROR(returnError < 0, "[Geographer] Fail to join the processus ... Abandonnement");
    }

    ERROR(returnError < 0, "[Geographer] Fail to stop Geographer");

    return returnError;
}

extern int8_t Geographer_signalConnectionEstablished() {
    int8_t returnError;

    MqMsg msg = { .event = E_CONNECTION_ESTABLISHED };
    returnError = sendMsgMq(&msg);

    ERROR(returnError < 0, "[Geographer] Fail to send the message signal connection established ... Abandonnement");

    return returnError;
}

extern int8_t Geographer_signalConnectionDown() {
    int8_t returnError;

    MqMsg msg = { .event = E_CONNECTION_DOWN };
    returnError = sendMsgMq(&msg);

    ERROR(returnError < 0, "[Geographer] Fail to send the message signal connection down ... Abandonnement");

    return returnError;
}

extern int8_t Geographer_askCalibrationPositions() {
    int8_t returnError;

    MqMsg msg = { .event = E_ASK_CALIBRATION_POSITIONS };
    returnError = sendMsgMq(&msg);

    ERROR(returnError < 0, "[Geographer] Fail to send the message stop ... retry");

    return returnError;
}

extern int8_t Geographer_validatePosition(CalibrationPositionId calibrationPositionId) {
    int8_t returnError;

    MqMsg msg = {
        .event = E_VALIDATE_POSITION,
        .data.calibrationPositionId = calibrationPositionId,
    };

    returnError = sendMsgMq(&msg);

    ERROR(returnError < 0, "[Geographer] Fail to send the message validate position ... Abandonnement");

    return returnError;
}

extern int8_t Geographer_signalEndUpdateAttenuation() {
    int8_t returnError;

    MqMsg msg = { .event = E_SIGNAL_END_UPDATE_ATTENUATION };
    returnError = sendMsgMq(&msg);

    ERROR(returnError < 0, "[Geographer] Fail to send the message singal end update attenuation ... Abandonnement");

    return returnError;
}

extern int8_t Geographer_signalEndAverageCalcul(CalibrationData* calibrationData, int8_t nbCalibration) { //comment
    int8_t returnError;

    MqMsg msg = { .event = E_SIGNAL_END_AVERAGE_CALCUL };

    returnError = sendMsgMq(&msg);
    ERROR(returnError < 0, "[Geographer] Fail to send the message signal end average calcul ... Abandonnement");

    return returnError;
}

extern int8_t Geographer_dateAndSendData(BeaconData* beaconsData, int8_t beaconsDataSize, Position* currentPosition, ProcessorAndMemoryLoad* currentProcessorAndMemoryLoad) {
    int8_t returnError = EXIT_FAILURE;

    MqMsg msg = {
        .event = E_DATE_AND_SEND_DATA,
        .data.current.position = currentPosition,
        .data.current.processorAndMemoryLoad = currentProcessorAndMemoryLoad,
        .data.current.beaconsData = beaconsData,
        .data.current.nbBeaconData = beaconsDataSize,
    };

    returnError = sendMsgMq(&msg);
    ERROR(returnError < 0, "[Geographer] Fail to send the message date and send data ... Abandonnement");

    return returnError;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static Date getCurrentDate() {
    Date date = time(NULL);
    return date;
}

static int8_t setUpMq(void) {
    int8_t returnError = EXIT_SUCCESS;

    mq_unlink(MQ_LABEL);

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof(MqMsg);
    attr.mq_curmsgs = 0;
    geographerMq = mq_open(MQ_LABEL, MQ_FLAGS, MQ_MODE, &attr);

    if (geographerMq < 0) {
        ERROR(true, "[Geographer] Fail to open the Queue");
        returnError = -1;
    }

    return returnError;
}

static int8_t tearDoneMq(void) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = mq_unlink(MQ_LABEL);

    if (returnError >= 0) {
        returnError = mq_close(geographerMq);
        ERROR(returnError < 0, "[Geographer] Error when closing the queue");
    } else {
        ERROR(true, "[Geographer] Error when unlinking the queue");
    }

    return returnError;
}

static int8_t readMsgMq(MqMsg* dest) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = mq_receive(geographerMq, (char*) dest, sizeof(MqMsg), NULL); // put char to avoid a warning
    ERROR(returnError < 0, "[Geographer] Error when reading the message in the queue");

    return returnError;
}

static int8_t sendMsgMq(MqMsg* msg) {
    int8_t returnError;
    errno = 0;

    returnError = mq_send(geographerMq, (char*) msg, sizeof(MqMsg), 0); // put char to avoid a warning
    ERROR(returnError < 0, "[Geographer] Error when sending the message in the queue");

    return returnError;
}

static void* runGeographer() {
    while (currentState != S_DEATH) {
        MqMsg msg;

        readMsgMq(&msg);
        TransitionGeographer transition = stateMachine[currentState][msg.event];

        if (transition.destinationState != S_NONE) {
            performAction(transition.action, &msg);

            currentState = transition.destinationState;

        } else {
            TRACE("[Geographer] MAE lost an event%s", "\n");
        }

    }
    return 0;
}

static int8_t performAction(ActionGeographer action, const MqMsg* msg) {
    int8_t returnError;

    switch (action) {
        case A_NONE:
        default:
            returnError = actionNone();
            break;

        case A_STOP:
            returnError = actionStop();
            break;

        case A_SEND_EXPERIMENTAL_DATA:
            returnError = actionSendExperimentalData();
            break;

        case A_SEND_ALL_DATA:
            returnError = actionSendAllData(msg->data.current.beaconsData, msg->data.current.nbBeaconData, msg->data.current.position, msg->data.current.processorAndMemoryLoad);
            break;

        case A_SET_CALIBRATION_DATA:
            returnError = actionSetCalibrationData(msg->data.calibration.calibrationData, msg->data.calibration.nbCalibrationData);
            break;

        case A_INCREASE_CALIBRATION_COUNTER:
            returnError = actionIncreaseCalibrationCounter();
            break;

        case A_SIGNAL_END_CALIBRATION_POSITION:
            returnError = actionSignalEndCalibrationPosition();
            break;

        case A_ASK_AVERAGE_CALCUL:
            returnError = actionAskAverageCalcul();
            break;

        case A_SET_CALIBRATION_POSITION:
            returnError = actionSetCalibrationPosition(calibrationPositions, NB_CALIBRATION_POSITION);
            break;
    }

    ERROR(returnError < 0, "[Geographer] Error when performing the action");

    return returnError;
}

static int8_t actionNone(void) {
    /* Nothing to do */
    return 0;
}

static int8_t actionStop() {
    Scanner_ask4StopScanner();
    ProxyLoggerMOB_stop();
    ProxyGUI_stop();

    currentState = S_DEATH;

    return 0;
}

static int8_t actionSendExperimentalData(void) {
    int8_t returnErrorPosition;
    int8_t returnErrorTraject;

    returnErrorPosition = ProxyLoggerMOB_setExperimentalPositions(experimentalPositions, NB_EXPERIMENTAL_POSITION);
    if (returnErrorPosition < 0) {
        ERROR(true, "[Geographer] Fail to send the experimental positions ... Retry");

        returnErrorPosition = ProxyLoggerMOB_setExperimentalPositions(experimentalPositions, NB_EXPERIMENTAL_POSITION);
        ERROR(returnErrorPosition < 0, "[Geographer] Fail to send the experimental positions ... Abandonment");
    }


    returnErrorTraject = ProxyLoggerMOB_setExperimentalTrajects(experimentalTrajects, NB_EXPERIMENTAL_TRAJECT);
    if (returnErrorTraject < 0) {
        ERROR(true, "[Geographer] Fail to send the experimental trajects ... Retry");

        returnErrorTraject = ProxyLoggerMOB_setExperimentalTrajects(experimentalTrajects, NB_EXPERIMENTAL_TRAJECT);
        ERROR(returnErrorTraject < 0, "[Geographer] Fail to send the experimental trajects ... Abandonment");
    }


    ERROR((returnErrorTraject + returnErrorPosition) < 0, "[Geographer] Fail to send the experimental data ... Abandonment");

    return (returnErrorTraject + returnErrorPosition) < 0 ? 0 : -1;
}

static int8_t actionSendAllData(const BeaconData* beaconData, uint8_t nbBeaconData, const Position* position, const ProcessorAndMemoryLoad* processorAndMemoryLoad) {
    Date currentDate = getCurrentDate();
    int8_t returnErrorBeaconData;
    int8_t returnErrorCurrentPosition;
    int8_t returnErrorLoad;

    if (connectionState == CONNECTED) {
        returnErrorBeaconData = ProxyLoggerMOB_setAllBeaconsData(beaconData, nbBeaconData, currentDate);
        if (returnErrorBeaconData < 0) {
            ERROR(true, "[Geographer] Fail to send the beacons data ... Retry");
            returnErrorBeaconData = ProxyLoggerMOB_setAllBeaconsData(beaconData, nbBeaconData, currentDate);
            ERROR(true, "[Geographer] Fail to send the beacons data ... Abandonment");
        }

        returnErrorCurrentPosition = ProxyLoggerMOB_setCurrentPosition(position, currentDate);
        if (returnErrorCurrentPosition < 0) {
            ERROR(true, "[Geographer] Fail to send the current position ... Retry");
            returnErrorCurrentPosition = ProxyLoggerMOB_setCurrentPosition(position, currentDate);
            ERROR(true, "[Geographer] Fail to send the current position ... Abandonment");
        }

        returnErrorLoad = ProxyLoggerMOB_setProcessorAndMemoryLoad(processorAndMemoryLoad, currentDate);
        if (returnErrorLoad < 0) {
            ERROR(true, "[Geographer] Fail to send the current processor and the memory load ... Retry");
            returnErrorLoad = ProxyLoggerMOB_setProcessorAndMemoryLoad(processorAndMemoryLoad, currentDate);
            ERROR(true, "[Geographer] Fail to send the beacons data ... Abandonment");
        }
    }

    ERROR((returnErrorBeaconData + returnErrorCurrentPosition + returnErrorLoad) < 0, "[Geographer] Fail to send a curent data ... Abandonment");

    return (returnErrorBeaconData + returnErrorCurrentPosition + returnErrorLoad) < 0 ? -1 : 0;
}

static int8_t actionSetCalibrationPosition(const CalibrationPosition* calibrationPosition, uint8_t nbCalibrationPosition) {
    int8_t returnErrorSetData;
    int8_t returnErrorSignal;

    calibrationCounter = 0;

    returnErrorSetData = ProxyGUI_setCalibrationPositions(calibrationPosition, nbCalibrationPosition);
    if (returnErrorSetData < 0) {
        ERROR(true, "[Geographer] Fail to send the calibration data ... Retry");

        returnErrorSetData = ProxyGUI_setCalibrationPositions(calibrationPosition, nbCalibrationPosition);
        ERROR(returnErrorSetData < 0, "[Geographer] Fail to send the calibration data ... Abandonment");
    }

    returnErrorSignal = ProxyGUI_signalEndCalibrationPosition();
    if (returnErrorSignal < 0) {
        ERROR(true, "[Geographer] Fail to signal the end of the calibration at the position ... Retry");

        returnErrorSignal = ProxyGUI_signalEndCalibrationPosition();
        ERROR(returnErrorSignal < 0, "[Geographer] Fail to signal the end of the calibration at the position ... Abandonment");
    }

    ERROR((returnErrorSignal + returnErrorSetData) < 0, "[Geographer] Fail to send the end the calibration at the position ... Abandonment");

    return (returnErrorSignal + returnErrorSetData) < 0 ? -1 : 0;
}

static int8_t actionIncreaseCalibrationCounter(void) {
    calibrationCounter++;
    return 0;
}

static int8_t actionSignalEndCalibrationPosition(void) {
    int8_t returnErrorSignal;
    int8_t returnErrorMq;

    returnErrorSignal = ProxyGUI_signalEndCalibrationPosition();
    if (returnErrorSignal < 0) {
        ERROR(true, "[Geographer] Fail to signal end of the calibration at the position ... Retry");

        returnErrorSignal = ProxyGUI_signalEndCalibrationPosition();
        ERROR(returnErrorSignal < 0, "[Geographer] Fail to signal the end of the calibration at the position ... Abandonment");
    }

    if (calibrationCounter >= NB_CALIBRATION_POSITION) {
        returnErrorMq = signalFinishCalibrateAllPosition();
    } else {
        returnErrorMq = signalNotFinishCalibrateAllPosition();
    }

    if (returnErrorMq < 0) {
        ERROR(true, "[Geographer] Fail to emit the internal signal for the the next state ... Retry");

        if (calibrationCounter >= NB_CALIBRATION_POSITION) {
            returnErrorMq = signalFinishCalibrateAllPosition();
        } else {
            returnErrorMq = signalNotFinishCalibrateAllPosition();
        }

        if (returnErrorMq < 0) {
            ERROR(true, "[Geographer] Fail to emit the internal signal for the the next state ... Re set up the queue");

            tearDoneMq();

            returnErrorMq = setUpMq();

            if (returnErrorMq < 0) {
                ERROR(true, "[Geographer] Fail to re set up ... Abandonment");
            } else {

                if (calibrationCounter >= NB_CALIBRATION_POSITION) {
                    returnErrorMq = signalFinishCalibrateAllPosition();
                } else {
                    returnErrorMq = signalNotFinishCalibrateAllPosition();
                }

                ERROR(returnErrorMq < 0, "[Geographer] Fail to emit the internal signal for the the next state ... Abandonment");
            }
        }
    }

    ERROR((returnErrorMq + returnErrorSignal) < 0, "[Geographer] Fail to signal the end of the calibration at the position ... Abandonment");

    return (returnErrorMq + returnErrorSignal) < 0 ? -1 : 0;
}

static int8_t actionAskAverageCalcul(void) {
    Scanner_ask4AverageCalcul();

    return 0;
}

static int8_t actionSetCalibrationData(const CalibrationData* calibrationData, uint8_t nbCalibrationData) {
    int8_t returnErrorData;
    int8_t returnErrorSignal;

    returnErrorData = ProxyLoggerMOB_setCalibrationData(calibrationData, nbCalibrationData);
    if (returnErrorData < 0) {
        ERROR(true, "[Geographer] Fail to send the calibration data ... Retry");
        returnErrorData = ProxyLoggerMOB_setCalibrationData(calibrationData, nbCalibrationData);
        ERROR(returnErrorData < 0, "[Geographer] Fail to send the calibration data ... Abandonment");
    }

    returnErrorSignal = ProxyGUI_signalEndCalibration();
    if (returnErrorSignal < 0) {
        ERROR(true, "[Geographer] Fail to signal the end of the calibration ... Retry");

        returnErrorSignal = ProxyGUI_signalEndCalibration();
        ERROR(returnErrorSignal < 0, "[Geographer] Fail to signal the end of the calibration ... Abandonment");
    }

    ERROR((returnErrorSignal + returnErrorData) < 0, "[Geographer] Fail to send the end the calibration ... Abandonment");

    return (returnErrorSignal + returnErrorData) < 0 ? -1 : 0;
}

static int8_t signalFinishCalibrateAllPosition(void) {
    MqMsg msg = { .event = E_FINISH_CALIBRATE_ALL_POSITION };

    int8_t returnError = sendMsgMq(&msg);
    ERROR(returnError < 0, "[Geographer] Fail to signal in intern the message finnish calibrate all position ... Abandonnement");

    return returnError;
}

static int8_t signalNotFinishCalibrateAllPosition(void) {
    MqMsg msg = { .event = E_NOT_FINISH_CALIBRATE_ALL_POSITION };

    int8_t returnError = sendMsgMq(&msg);
    ERROR(returnError < 0, "[Geographer] Fail to signal in intern the message not finnish calibrate all position ... Abandonnement");

    return returnError;
}
