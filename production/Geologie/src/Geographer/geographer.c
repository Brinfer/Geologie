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
#define EXP_POSITION_NUMBER (25)
#define EXP_TRAJECT_NUMBER (3)
#define CALIBRATION_POSITION_NUMBER (25)

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
    S_FORGET = 0,
    S_DEATH,
    S_WATING_FOR_CONNECTION,
    S_IDLE,
    S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION,
    S_WAITING_FOR_BE_PLACED,
    S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT,
    S_CHOICE_CONNEXION,
    S_CHOICE_CALIBRATION,
    NB_STATE_GEOGRAPHER
} State_GEOGRAPHER;

/**
 * @brief Structure contenant les evenement que geographer peut recevoir
 */
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
} Event_GEOGRAPHER;

/**
 * @brief Structure contenant les action que geographer peut effectuer
 */
typedef enum {
    A_NOP = 0,
    A_STOP,
    A_CONNECTION_ESTABLISHED,
    A_CONNECTION_DOWN,
    A_SET_ALL_DATA,
    A_SET_CALIBRATION_POSITIONS,
    A_ASK_4_UPDATE_ATTENUATION_COEFFICIENT,
    A_END_CALIBRATION,
    A_ASK_4_AVERAGE_CALCUL,
    A_CALIBRATION_COUNTER,

    NB_ACTION_GEOGRAPHER
} Action_GEOGRAPHER;

typedef struct {
    State_GEOGRAPHER destinationState;
    Action_GEOGRAPHER action;
} Transition_GEOGRAPHER;

typedef struct {
    Event_GEOGRAPHER event;
    Position* currentPosition;
    BeaconData* beaconsData;
    int8_t beaconsDataSize;
    ProcessorAndMemoryLoad* currentProcessorAndMemoryLoad;
    CalibrationPositionId calibrationPositionId;
    CalibrationData* calibrationData;
    int8_t nbCalibration;
} MqMsg;

static const Transition_GEOGRAPHER stateMachine[NB_STATE_GEOGRAPHER][NB_EVENT_GEOGRAPHER] =
{
    [S_WATING_FOR_CONNECTION][E_CONNECTION_ESTABLISHED] = {S_IDLE, A_CONNECTION_ESTABLISHED},
    [S_WATING_FOR_CONNECTION][E_STOP] = {S_DEATH,A_STOP},

    [S_IDLE][E_ASK_CALIBRATION_POSITIONS] = {S_WAITING_FOR_BE_PLACED, A_SET_CALIBRATION_POSITIONS},
    [S_IDLE][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION, A_CONNECTION_DOWN},
    [S_IDLE][E_DATE_AND_SEND_DATA_ELSE] = {S_IDLE, A_NOP},
    [S_IDLE][E_DATE_AND_SEND_DATA_CONNECTED] = {S_IDLE, A_SET_ALL_DATA},
    [S_IDLE][E_STOP] = {S_DEATH, A_STOP},

    [S_WAITING_FOR_BE_PLACED][E_VALIDATE_POSITION] = {S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION, A_ASK_4_UPDATE_ATTENUATION_COEFFICIENT},
    [S_WAITING_FOR_BE_PLACED][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION ,A_CONNECTION_DOWN},
    [S_WAITING_FOR_BE_PLACED][E_STOP] = {S_DEATH, A_STOP},

    [S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION][E_SIGNAL_END_UPDATE_ATTENUATION_ELSE] = {S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT, A_ASK_4_AVERAGE_CALCUL},
    [S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION][E_SIGNAL_END_UPDATE_ATTENUATION_CALIBRATION] = {S_WAITING_FOR_BE_PLACED, A_CALIBRATION_COUNTER},
    [S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION, A_CONNECTION_DOWN},
    [S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION][E_STOP] = {S_DEATH, A_STOP},

    [S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION, A_CONNECTION_DOWN},
    [S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT][E_SIGNAL_END_AVERAGE_CALCUL] = {S_IDLE, A_END_CALIBRATION},
    [S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT][E_STOP] = {S_DEATH, A_STOP},
};

/**
 * @brief Etat de la connexion.
 */
static ConnectionState connectionState;

/**
 * @brief Compteur de calibration.
 */
static int8_t calibrationCounter;

/**
 * @brief Date actuelle.
 */
static Date currentDate;

static State_GEOGRAPHER currentState;

static pthread_t geographerThread;

static mqd_t geographerMq;

/**
* @brief Tableau contenant les positions experimentales
*
*/
static const ExperimentalPosition experimentalPositions[EXP_POSITION_NUMBER] = {
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
static const  CalibrationPosition calibrationPositions[CALIBRATION_POSITION_NUMBER] = {
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
static const Position traject1[] = {
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
static const Position traject2[] = {
    {.X = 50, .Y = 1300 },
    {.X = 1050, .Y = 1000 }
};

/**
* @brief Tableau contenant le troisieme trajet experimental
*
*/
static const Position traject3[] = {
    {.X = 950, .Y = 1000 },
    {.X = 350, .Y = 1200 },
    {.X = 50, .Y = 1000 }
};

/**
* @brief Tableau contenant les trajets experimentaux
*
*/
static const  ExperimentalTraject experimentalTrajects[EXP_TRAJECT_NUMBER] = {
    {.id = 1, .traject = traject1, .nbPosition = 5},
    {.id = 2, .traject = traject2, .nbPosition = 2},
    {.id = 3, .traject = traject3, .nbPosition = 3}
};


/**
 * @brief Etat de la connexion.
 *
 */
static ConnectionState connectionState;

/**
 * @brief Compteur de calibration.
 *
 */
static int8_t calibrationCounter;

/**
 * @brief Date actuelle.
 *
 */
static Date currentDate;


/**
 * @struct State_GEOGRAPHER
 * @brief structure contenant les etat que peut prendre geographer
 *
 */
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
} State_GEOGRAPHER;

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

static const char* State_Geographer_getName(int8_t i) {
    return State_Geographer_Name[i];
}


/**
 * @struct Event_GEOGRAPHER
 * @brief structure contenant les evenement que geographer peut recevoir
 *
 */
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
} Event_GEOGRAPHER;

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

static const char* Event_Geographer_getName(int8_t i) {
    return Event_Geographer_Name[i];
}

/**
 * @struct Action_GEOGRAPHER
 * @brief structure contenant les action que geographer peut effectuer
 *
 */
typedef enum {
    A_NOP = 0,
    A_STOP,
    A_CONNECTION_ESTABLISHED,
    A_CONNECTION_DOWN,
    A_SET_ALL_DATA,
    A_SET_CALIBRATION_POSITIONS,
    A_ASK_4_UPDATE_ATTENUATION_COEFFICIENT,
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
    "A_END_CALIBRATION",
    "A_ASK_4_AVERAGE_CALCUL",
    "A_CALIBRATION_COUNTER"
};

static const char* Action_Geographer_getName(int8_t i) {
    return Action_Geographer_Name[i];
}


typedef struct {
    State_GEOGRAPHER destinationState;
    Action_GEOGRAPHER action;
} Transition_GEOGRAPHER;

typedef struct {
    Event_GEOGRAPHER event;
    Position* currentPosition;
    BeaconData* beaconsData;
    int8_t beaconsDataSize;
    ProcessorAndMemoryLoad* currentProcessorAndMemoryLoad;
    CalibrationPositionId calibrationPositionId;
    CalibrationData* calibrationData;
    int8_t nbCalibration;
} MqMsg;


static State_GEOGRAPHER myState;
static pthread_t threadGeographer;


/**
 * The name of the queue
 */
static const char BAL[] = "/BALGeographer";

static mqd_t descripteur;

static struct mq_attr attr;



static Transition_GEOGRAPHER stateMachine[NB_STATE_ - 1][NB_EVENT_GEOGRAPHER] =
{
    [S_WATING_FOR_CONNECTION] [E_CONNECTION_ESTABLISHED] = {S_IDLE,A_CONNECTION_ESTABLISHED},
    [S_WATING_FOR_CONNECTION][E_STOP] = {S_DEATH,A_STOP},

    [S_IDLE][E_ASK_CALIBRATION_POSITIONS] = {S_WAITING_FOR_BE_PLACED,A_SET_CALIBRATION_POSITIONS},
    [S_IDLE][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION,A_CONNECTION_DOWN},
    [S_IDLE][E_DATE_AND_SEND_DATA_ELSE] = {S_IDLE,A_NOP},
    [S_IDLE][E_DATE_AND_SEND_DATA_CONNECTED] = {S_IDLE,A_SET_ALL_DATA},
    [S_IDLE][E_STOP] = {S_DEATH,A_STOP},


    [S_WAITING_FOR_BE_PLACED][E_VALIDATE_POSITION] = {S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION,A_ASK_4_UPDATE_ATTENUATION_COEFFICIENT},
    [S_WAITING_FOR_BE_PLACED][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION,A_CONNECTION_DOWN},
    [S_WAITING_FOR_BE_PLACED][E_STOP] = {S_DEATH,A_STOP},

    [S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION][E_SIGNAL_END_UPDATE_ATTENUATION_ELSE] = {S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT,A_ASK_4_AVERAGE_CALCUL},
    [S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION][E_SIGNAL_END_UPDATE_ATTENUATION_CALIBRATION] = {S_WAITING_FOR_BE_PLACED,A_CALIBRATION_COUNTER},
    [S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION,A_CONNECTION_DOWN},
    [S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION][E_STOP] = {S_DEATH,A_STOP},

    [S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION,A_CONNECTION_DOWN},
    [S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT][E_SIGNAL_END_AVERAGE_CALCUL] = {S_IDLE,A_END_CALIBRATION},
    [S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT][E_STOP] = {S_DEATH,A_STOP},


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
static int8_t mqInit();

/**
 * @brief ferme et detruit la boite aux lettres
 *
 * @return -1 si erreur detectee, sinon retourne 0
*/
static int8_t mqDone();

/**
 * @brief ferme et detruit la boite aux lettres
 *
 * @return -1 si erreur detectee, sinon retourne 0
 * @param dest message a envoyer a la queue
*/
static int8_t mqReceive(MqMsg* dest);

/**
 * @brief methode appelee par un thread pour recevoir les action a faire grace a une boite au lettre et les effectuer
 *
 * cette metode lira la boite au lettre et effectuera les actions
 */
static void* run();
/**
 * @brief methode appelee par run pour executer les actions
 *
 * @param action action a effectuee
 * @param msg structure contenant les donnees a utiliser pour effectuer l'action
 */
static void performAction(Action_GEOGRAPHER action, MqMsg* msg);

/**
 * @brief methode pour envoyer des message a la queue
 *
 * @param sizeOfMsg taille du message a envoyer
 * @param msg structure contenant les donnees a utiliser pour effectuer l'action
 * @return 1 si erreur 0 si
 */
static int8_t sendMsg(MqMsg* msg);

#ifndef _TESTING_MODE
static void Geographer_transitionFct(MqMsg msg);
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int8_t Geographer_new() {
    int8_t returnError = EXIT_FAILURE;
    returnError = ProxyGUI_new();

    if (returnError == 0) {
        returnError = ProxyLoggerMOB_new();

        if (returnError >= 0) {
            Scanner_new();
            mqInit();
        }
    }

    return returnError;
}

extern int8_t Geographer_free() {
    int8_t returnError = EXIT_FAILURE;
    mqDone();
    Scanner_free();
    if (returnError == EXIT_SUCCESS) {
        returnError = ProxyLoggerMOB_free();
    }
    if (returnError == EXIT_SUCCESS) {
        returnError = ProxyGUI_free();
    }
    mqDone();
    return returnError;
}

extern int8_t Geographer_askSignalStartGeographer() {
    int8_t returnError = EXIT_FAILURE;
    returnError = ProxyGUI_start();
    if (returnError == EXIT_SUCCESS) {
        returnError = ProxyLoggerMOB_start();

        if (returnError == 0) {
            Scanner_ask4StartScanner();

            currentState = S_WATING_FOR_CONNECTION;
            connectionState = DISCONNECTED;
            calibrationCounter = 0;

            returnError = pthread_create(&geographerThread, NULL, &run, NULL);
            assert(returnError >= 0);
        }
    }
    return returnError;
}

extern int8_t Geographer_askSignalStopGeographer() {
    int8_t returnError = EXIT_FAILURE;

    MqMsg msg = { .event = E_STOP };
    returnError = sendMsg(&msg);

    return returnError;
}

extern int8_t Geographer_askCalibrationPositions() {
    int8_t returnError = EXIT_FAILURE;
    MqMsg msg = { .event = E_ASK_CALIBRATION_POSITIONS };
    returnError = sendMsg(&msg);
    return returnError;
}

extern int8_t Geographer_validatePosition(CalibrationPositionId calibrationPositionId) {
    int8_t returnError = EXIT_FAILURE;
    MqMsg msg = {
        .event = E_VALIDATE_POSITION,
        .calibrationPositionId = calibrationPositionId,
    };

    returnError = sendMsg(&msg);

    return returnError;
}

extern int8_t Geographer_signalEndUpdateAttenuation() {
    int8_t returnError = EXIT_FAILURE;

    if (calibrationCounter == CALIBRATION_POSITION_NUMBER) {
        MqMsg msg = { .event = E_SIGNAL_END_UPDATE_ATTENUATION_CALIBRATION };
        returnError = sendMsg(&msg);

    } else {
        MqMsg msg = { .event = E_SIGNAL_END_UPDATE_ATTENUATION_ELSE };
        returnError = sendMsg(&msg);
    }

    return returnError;
}

extern int8_t Geographer_signalEndAverageCalcul(CalibrationData* calibrationData, int8_t nbCalibration) { //comment
    int8_t returnError = EXIT_FAILURE;

    MqMsg msg = {
        .event = E_SIGNAL_END_AVERAGE_CALCUL,
        .calibrationData = calibrationData,
        .nbCalibration = nbCalibration,
    };
    returnError = sendMsg(&msg);

    return returnError;
}

extern int8_t Geographer_signalConnectionEstablished() {
    int8_t returnError = EXIT_FAILURE;
    MqMsg msg = { .event = E_CONNECTION_ESTABLISHED };
    returnError = sendMsg(&msg);

    return returnError;
}

extern int8_t Geographer_signalConnectionDown() {
    int8_t returnError = EXIT_FAILURE;
    MqMsg msg = { .event = E_CONNECTION_DOWN };
    returnError = sendMsg(&msg);

    return returnError;
}

extern int8_t Geographer_dateAndSendData(BeaconData* beaconsData, int8_t beaconsDataSize, Position* currentPosition, ProcessorAndMemoryLoad* currentProcessorAndMemoryLoad) {
    int8_t returnError = EXIT_FAILURE;
    if (connectionState == CONNECTED) {

        MqMsg msg = {
            .event = E_DATE_AND_SEND_DATA_CONNECTED,
            .beaconsData = beaconsData,
            .beaconsDataSize = beaconsDataSize,
            .currentPosition = currentPosition,
            .currentProcessorAndMemoryLoad = currentProcessorAndMemoryLoad,
        };

        returnError = sendMsg(&msg);

    } else { //si pas connecté

        free(beaconsData);
        free(currentPosition);
        free(currentProcessorAndMemoryLoad);
    }
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

static int8_t mqInit(void) {
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

static int8_t mqDone(void) {
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

static int8_t mqReceive(MqMsg* dest) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = mq_receive(geographerMq, (char*) dest, sizeof(MqMsg), NULL); // put char to avoid a warning
    ERROR(returnError < 0, "[Geographer] Error when reading the message in the queue");

    return returnError;
}

#ifndef _TESTING_MODE
static void Geographer_transitionFct(MqMsg msg)
#else
extern void Geographer_transitionFct(MqMsg msg);
extern void __real_Geographer_transitionFct(MqMsg msg)
#endif
{
    performAction(stateMachine[currentState][msg.event].action, &msg);
    currentState = stateMachine[currentState][msg.event].destinationState;
}

static void* run() {
    MqMsg msg;
    while (currentState != S_DEATH) {
        mqReceive(&msg); //Opération privée pour lire dans la BAL de Geographer

        if (stateMachine[currentState][msg.event].destinationState == S_FORGET) // aucun état de destination, on ne fait rien
        {
        } else {
            Geographer_transitionFct(msg);
        }
    }
    return 0;
}

//Commenter les actionAction

static void actionStop() {
    Scanner_ask4StopScanner();
    ProxyLoggerMOB_stop();
    ProxyGUI_stop();

    calibrationCounter = 0;
    connectionState = DISCONNECTED;
    pthread_join(geographerThread, NULL);
}

static void actionNop() {
    /* Nothing to do */
}

static void actionConnectionEstablished() {
    connectionState = CONNECTED;
    ProxyLoggerMOB_setExperimentalPositions(experimentalPositions, EXP_POSITION_NUMBER);
    ProxyLoggerMOB_setExperimentalTrajects(experimentalTrajects, EXP_TRAJECT_NUMBER);
}

static void actionConnectionDown() {
    connectionState = DISCONNECTED;
}

static void actionSetAllData(MqMsg* msg) {
    currentDate = getCurrentDate();
    ProxyLoggerMOB_setAllBeaconsData(msg->beaconsData, msg->beaconsDataSize, currentDate);
    ProxyLoggerMOB_setCurrentPosition(msg->currentPosition, currentDate);
    ProxyLoggerMOB_setProcessorAndMemoryLoad(msg->currentProcessorAndMemoryLoad, currentDate);
}

static void actionSetCalibrationPositions() {
    calibrationCounter = 0;
    ProxyGUI_setCalibrationPositions(calibrationPositions, CALIBRATION_POSITION_NUMBER);
}

static void actionAsk4UpdateAttenuationCoefficient() {
    Scanner_ask4UpdateAttenuationCoefficientFromPosition(calibrationPositions[calibrationCounter]);
}

static void actionEndCalibration(MqMsg* msg) {
    ProxyLoggerMOB_setCalibrationData(msg->calibrationData, msg->nbCalibration);
    ProxyGUI_signalEndCalibration();
}

static void actionCalibrationCounter() {
    ProxyGUI_signalEndCalibrationPosition();
    calibrationCounter++;
}

static void actionAsk4AverageCalcul() {
    ProxyGUI_signalEndCalibrationPosition();
    Scanner_ask4AverageCalcul();
}

static void performAction(Action_GEOGRAPHER anAction, MqMsg* msg) {
    switch (anAction) {
        case A_STOP:
            actionStop();
            break;

        case A_NOP:
            actionNop();
            break;

        case A_CONNECTION_ESTABLISHED:
            actionConnectionEstablished();
            break;

        case A_CONNECTION_DOWN:
            actionConnectionDown();
            break;

        case A_SET_ALL_DATA:
            actionSetAllData(msg);
            break;

        case A_SET_CALIBRATION_POSITIONS:
            actionSetCalibrationPositions();
            break;

        case A_ASK_4_UPDATE_ATTENUATION_COEFFICIENT:
            actionAsk4UpdateAttenuationCoefficient();
            break;

        case A_END_CALIBRATION:
            actionEndCalibration(msg);
            break;

        case A_CALIBRATION_COUNTER:
            actionCalibrationCounter();
            break;

        case A_ASK_4_AVERAGE_CALCUL:
            actionAsk4AverageCalcul();
            break;

        default:
            break;
    }
}

static int8_t sendMsg(MqMsg* msg) {
    int8_t returnError = EXIT_FAILURE;
    if (mq_send(geographerMq, (char*) msg, sizeof(msg), 0) == 0) {
        returnError = EXIT_SUCCESS;
    }
    return returnError;
}
