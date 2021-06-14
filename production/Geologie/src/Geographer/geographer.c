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
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mqueue.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>

#include "geographer.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* @brief Tableau contenant les positions experimentales
*
*/
static ExperimentalPosition experimentalPositions[EXP_POSITION_NUMBER] = {
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
static CalibrationPosition calibrationPositions[CALIBRATION_POSITION_NUMBER] = {
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
Position traject1[] = {
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
Position traject2[] = {
    {.X = 50, .Y = 1300 },
    {.X = 1050, .Y = 1000 }
};
/**
* @brief Tableau contenant le troisieme trajet experimental
*
*/
Position traject3[] = {
    {.X = 950, .Y = 1000 },
    {.X = 350, .Y = 1200 },
    {.X = 50, .Y = 1000 }
};
/**
* @brief Tableau contenant les trajets experimentaux
*
*/
static ExperimentalTraject experimentalTrajects[EXP_TRAJECT_NUMBER] = {
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
//                                              Fonctions privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn static Date getCurrentDate()
 * @brief Recupere la date ecoulee depuis le 1 er janvier 1970
 * *
 * @return renvoie la date actuelle en secondes
*/
static Date getCurrentDate();

/**
 * @fn static void mqInit ()
 * @brief initialise la boite aux lettres
 *
 * @return 1 si erreur detectee, sinon retourne 0
*/
static void mqInit();

/**
 * @fn static int8_t mqDone ()
 * @brief ferme et detruit la boite aux lettres
 *
 * @return 1 si erreur detectee, sinon retourne 0
*/
static void mqDone();

/**
 * @fn static int8_t mqReceive (MqMsg *this)
 * @brief ferme et detruit la boite aux lettres
 *
 * @return 1 si erreur detectee, sinon retourne 0
 * @param this message a envoyer a la queue
*/
static void mqReceive(MqMsg* this);


/**
 * @fn static void* run()
 * @brief methode appelee par un thread pour recevoir les action a faire grace a une boite au lettre et les effectuer
 *
 * cette metode lira la boite au lettre et effectuera les actions
 */
static void* run();
/**
 * @fn static void performAction(Action_GEOGRAPHER action, MqMsg* msg)
 * @brief methode appelee par run pour executer les actions
 *
 * @param action action a effectuee
 * @param msg structure contenant les donnees a utiliser pour effectuer l'action
 */
static void performAction(Action_GEOGRAPHER action, MqMsg* msg);

/**
 * @fn static void sendMsg(MqMsg* msg, int8_t sizeOfMsg)
 * @brief methode pour envoyer des message a la queue
 *
 * @param sizeOfMsg taille du message a envoyer
 * @param msg structure contenant les donnees a utiliser pour effectuer l'action
 * @return 1 si erreur 0 si
 */
static int8_t sendMsg(MqMsg* msg);



static Date getCurrentDate() {
    Date date = time(NULL);
    return date;
}

static void mqInit(void) {
    attr.mq_flags = 0;
    attr.mq_maxmsg = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof(MqMsg);
    attr.mq_curmsgs = 0;


    if (mq_unlink(BAL) == -1) {
        if (errno != ENOENT) {
            perror("Erreur Unlink : ");
            exit(1);
        }
    }
    descripteur = mq_open(BAL, O_RDWR | O_CREAT, 0777, &attr);
    if (descripteur == -1) {
        perror("Erreur Open :\n");
    } else {
    }

}

static void mqDone(void) {

    mq_close(descripteur); //On ferme la BAL en mettant en paramètre le descripteur


    mq_unlink(BAL); //On détruit la BAL en mettant en paramètre le nom de la BAL
}

static void mqReceive(MqMsg* this) {
    mq_receive(descripteur, (char*) this, sizeof(*this), NULL);
}

static void* run() {
    MqMsg msg;
    Action_GEOGRAPHER act;
    while (myState != S_DEATH) {
        mqReceive(&msg); //Opération privée pour lire dans la BAL de Geographer


        if (stateMachine[myState][msg.event].destinationState == S_FORGET) // aucun état de destination, on ne fait rien
        {
            TRACE("MAE, perte evenement %s  \n", Event_Geographer_getName(msg.event));
        } else
        {
            TRACE("MAE, traitement evenement %s \n", Event_Geographer_getName(msg.event));

            act = stateMachine[myState][msg.event].action;
            TRACE("MAE, traitement action %s \n", Action_Geographer_getName(act));
            performAction(act, &msg);
            myState = stateMachine[myState][msg.event].destinationState;
            TRACE("MAE, va dans etat %s \n", State_Geographer_getName(myState));

        }
    }
    return 0;
}

static void performAction(Action_GEOGRAPHER anAction, MqMsg* msg) {
    switch (anAction) {
        case A_STOP:

            Scanner_ask4StopScanner();

            ProxyLoggerMOB_stop();

            ProxyGUI_stop();


            calibrationCounter = 0;
            connectionState = DISCONNECTED;
            TRACE("debut arret thread geographer %s ", "\n");
            pthread_join(threadGeographer, NULL);
            TRACE("fin arret thread geographer %s ", "\n");
            break;

        case A_NOP:
            break;

        case A_CONNECTION_ESTABLISHED:
            connectionState = CONNECTED;
            ProxyLoggerMOB_setExperimentalPositions(experimentalPositions, EXP_POSITION_NUMBER);
            ProxyLoggerMOB_setExperimentalTrajects(experimentalTrajects, EXP_TRAJECT_NUMBER);
            break;

        case A_CONNECTION_DOWN:
            connectionState = DISCONNECTED;
            break;

        case A_SET_ALL_DATA:
            currentDate = getCurrentDate();
            ProxyLoggerMOB_setAllBeaconsData(msg->beaconsData, msg->beaconsDataSize, currentDate);
            ProxyLoggerMOB_setCurrentPosition(msg->currentPosition, currentDate);
            ProxyLoggerMOB_setProcessorAndMemoryLoad(msg->currentProcessorAndMemoryLoad, currentDate);
            break;

        case A_SET_CALIBRATION_POSITIONS:
            calibrationCounter = 0;
            ProxyGUI_setCalibrationPositions(calibrationPositions, CALIBRATION_POSITION_NUMBER);
            break;

        case A_ASK_4_UPDATE_ATTENUATION_COEFFICIENT:
            Scanner_ask4UpdateAttenuationCoefficientFromPosition(calibrationPositions[calibrationCounter]);
            break;



        case A_END_CALIBRATION:
            ProxyLoggerMOB_setCalibrationData(msg->calibrationData, msg->nbCalibration);
            ProxyGUI_signalEndCalibration();
            break;

        case A_CALIBRATION_COUNTER:
            ProxyGUI_signalEndCalibrationPosition();
            calibrationCounter++;
            break;

        case A_ASK_4_AVERAGE_CALCUL:
            ProxyGUI_signalEndCalibrationPosition();
            Scanner_ask4AverageCalcul();
            break;

        default:
            TRACE("Action inconnue, pb ds la MAE de geographer %s ", "\n");
            break;
    }
}

static int8_t sendMsg(MqMsg* msg) {
    int8_t returnError = EXIT_FAILURE;
    if (mq_send(descripteur, (char*) msg, sizeof(msg), 0) == 0) {
        returnError = EXIT_SUCCESS;
    }
    return returnError;
}

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

            myState = S_WATING_FOR_CONNECTION;
            connectionState = DISCONNECTED;
            calibrationCounter = 0;

            returnError = pthread_create(&threadGeographer, NULL, &run, NULL);
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
