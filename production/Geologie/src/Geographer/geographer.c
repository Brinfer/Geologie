/**
 * @file geographer.c
 *
 * @version 1.0
 * @date 03/06/21
 * @author Nathan BRIENT
 * @copyright BSD 2-clauses
 *
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
#include <stdlib.h>
#include <unistd.h> // Macros, type Posix and co
#include <pthread.h>
#include <mqueue.h>
#include <time.h>
#include <errno.h>

#include "geographer.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* @brief Tableau d'entier
* creation d'un tableau constant d'entier
*/
static ExperimentalPosition experimentalPositions[]={
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

static CalibrationPosition calibrationPositions[]={
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


Position traject1[]={
    {.X = 100, .Y = 1300 },
    {.X = 1000, .Y = 1300 },
    {.X = 1000, .Y = 200 },
    {.X = 100, .Y = 200 },
    {.X = 100, .Y = 1300}
};
Position traject2[]={
    {.X = 50, .Y = 1300 },
    {.X = 1050, .Y = 1000 }
};
Position traject3[]={
    {.X = 950, .Y = 1000 },
    {.X = 350, .Y = 1200 },
    {.X = 50, .Y = 1000 }
};

static ExperimentalTraject experimentalTrajects[]={
    {.id= 1, .traject = traject1, .nbPosition=5},
    {.id= 2, .traject = traject2, .nbPosition=2},
    {.id= 3, .traject = traject3, .nbPosition=3},
};

static ConnectionState connectionState;

static uint8_t calibrationCounter;

static Date currentDate;

/////////////////////////////////////////////////////////////////////////////
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

typedef struct {
    Event_GEOGRAPHER event; //evenement interne
    Position* currentPosition;
    BeaconData* beaconsData;
    uint8_t beaconsDataSize;
    ProcessorAndMemoryLoad* currentProcessorAndMemoryLoad;
    CalibrationPositionId calibrationPositionId;
    CalibrationData* calibrationData;
    uint8_t nbCalibration;
} MqMsg; //type de message dans la BAL


static State_GEOGRAPHER myState;
static pthread_t myThread;


/**
 * The name of the queue
 */
static const char BAL[] = "/BALGeographer";

static mqd_t descripteur; //On déclare un descripteur de notre BAL qui permettra de l'ouvrir et de la fermer

static struct mq_attr attr; //On déclare un attribut pour la fonction mq_open qui est une structure spécifique à la file pour la configurer (cf l.64)



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
 * @fn static void* run();
 * @brief methode appelee par un thread pour recevoir les action a faire grace a une boite au lettre et les effectuer
 *
 * cette metode lira la boite au lettre et effectuera les actions
 */
static void* run();
/**
 * @fn static void performAction(Action_GEOGRAPHER action, MqMsg* msg);
 * @brief methode appelee par run pour executer les actions
 *
 * @param action action a effectuee
 * @param msg structure contenant les donnees a utiliser pour effectuer l'action
 */
static void performAction(Action_GEOGRAPHER action, MqMsg* msg);

/**
 * @fn static void sendMsg(MqMsg* msg, uint8_t sizeOfMsg);
 * @brief methode pour envoyer des message a la queue
 *
 * @param sizeOfMsg taille du message a envoyer
 * @param msg structure contenant les donnees a utiliser pour effectuer l'action
 * @return 1 si erreur 0 si
 */
static uint8_t sendMsg(MqMsg* msg);

static Date getCurrentDate() {
    Date date = time(NULL);    //TODO determiner si c'est en secondes ou en millisecondes sur la discovery
    return date;
}

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

static void* run() {
    printf("run\n");
    MqMsg msg; //message prenant l'event
    Action_GEOGRAPHER act;
    while (myState != S_DEATH) {
        mqReceive(&msg); //Opération privée pour lire dans la BAL de AdminUI
        if (stateMachine[myState][msg.event].destinationState == S_FORGET) // aucun état de destination, on ne fait rien
        {
            TRACE("MAE, perte evenement %s  \n", Event_Geographer_getName(msg.event));
        } else /* on tire la transition */
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
        ProxyGUI_setCalibrationPositions(calibrationPositions,CALIBRATION_POSITION_NUMBER);
        break;

    case A_ASK_4_UPDATE_ATTENUATION_COEFFICIENT:
        Scanner_ask4UpdateAttenuationCoefficientFromPosition(calibrationPositions[calibrationCounter]);
        break;

    case A_END_CALIBRATION_POSITION:

        ProxyLoggerMOB_setCalibrationData(msg->calibrationData, msg->nbCalibration);
        break;

    case A_END_CALIBRATION: //TODO
        break;

    case A_CALIBRATION_COUNTER:
        calibrationCounter++;
        break;

    case A_ASK_4_AVERAGE_CALCUL:
        Scanner_ask4AverageCalcul();
        break;

    default:
        TRACE("Action inconnue, pb ds la MAE de geographer %s ","\n");
        break;
    }
}

static uint8_t sendMsg(MqMsg* msg) { //TODO pas besoin mutex, deja protege/a revoir
    uint8_t returnError = EXIT_FAILURE;
    if (mq_send(descripteur, (char*) &msg, sizeof(msg), 0) == 0) {
        returnError = EXIT_SUCCESS;
    }
    return returnError;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern uint8_t Geographer_new() {
    uint8_t returnError = EXIT_FAILURE;
    printf("Geographer_New\n");
    returnError = ProxyGUI_new();

    if (returnError == EXIT_SUCCESS) {
        ProxyLoggerMOB_new();
    }
    if (returnError == EXIT_SUCCESS) {
        Scanner_new();
    }
    mqInit();

    return returnError;
}

extern uint8_t Geographer_free() {
    uint8_t returnError = EXIT_FAILURE;
    mqDone();


    //returnError = Scanner_free();

    if (returnError == EXIT_SUCCESS) {
        ProxyLoggerMOB_free();
    }
    if (returnError == EXIT_SUCCESS) {
        ProxyGUI_free();
    }
    mqDone();
    return returnError;
}


extern uint8_t Geographer_askSignalStartGeographer() {
    uint8_t returnError = EXIT_FAILURE;
    returnError = ProxyGUI_start();
    if (returnError == EXIT_SUCCESS) {
        ProxyLoggerMOB_start();
    }
    if (returnError == EXIT_SUCCESS) {
        Scanner_ask4StartScanner();
    }
    myState = S_WATING_FOR_CONNECTION;
    connectionState = DISCONNECTED;
    calibrationCounter = 0;

    pthread_create(&myThread, NULL, &run, NULL);

    return returnError;
}


extern uint8_t Geographer_askSignalStopGeographer() {
    uint8_t returnError = EXIT_FAILURE;
    //returnError = Scanner_ask4StopScanner();

    if (returnError == EXIT_SUCCESS) {
        ProxyLoggerMOB_stop();
    }
    if (returnError == EXIT_SUCCESS) {
        ProxyGUI_stop();
    }
    pthread_join(myThread, NULL);

    return returnError;
}


extern uint8_t Geographer_askCalibrationPositions() {
    uint8_t returnError = EXIT_FAILURE;
    printf("Geographer_askCalibrationPositions\n");
    MqMsg msg = { .event = E_ASK_CALIBRATION_POSITIONS };
    returnError = sendMsg(&msg);

    return returnError;
}


extern uint8_t Geographer_validatePosition(CalibrationPositionId calibrationPositionId) {
    uint8_t returnError = EXIT_FAILURE;
    printf("Geographer_validatePosition\n");
    //tempCalibrationPositionId = calibrationPositionId;
    MqMsg msg = {
        .event = E_VALIDATE_POSITION,
        .calibrationPositionId = calibrationPositionId,
    };

    returnError = sendMsg(&msg);

    return returnError;
}


extern uint8_t Geographer_signalEndUpdateAttenuation() {
    uint8_t returnError = EXIT_FAILURE;
    printf("Geographer_signalEndUpdateAttenuation\n");

    if (calibrationCounter == CALIBRATION_POSITION_NUMBER) {
        MqMsg msg = { .event = E_SIGNAL_END_UPDATE_ATTENUATION_CALIBRATION };
        returnError = sendMsg(&msg );

    } else {
        MqMsg msg = { .event = E_SIGNAL_END_UPDATE_ATTENUATION_ELSE };
        returnError = sendMsg(&msg );
    }

    return returnError;
}


extern uint8_t Geographer_signalEndAverageCalcul(CalibrationData* calibrationData, uint8_t nbCalibration) { //comment
    uint8_t returnError = EXIT_FAILURE;
    printf("Geographer_signalEndAverageCalcul\n");

    MqMsg msg = {
        .event = E_SIGNAL_END_AVERAGE_CALCUL,
        .calibrationData = calibrationData,
        .nbCalibration = nbCalibration,
    };
    returnError = sendMsg(&msg );

    return returnError;
}


extern uint8_t Geographer_signalConnectionEstablished() {
    uint8_t returnError = EXIT_FAILURE;
    printf("Geographer_signalConnectionEstablished\n");
    MqMsg msg = { .event = E_CONNECTION_ESTABLISHED };
    returnError = sendMsg(&msg );

    return returnError;
}


extern uint8_t Geographer_signalConnectionDown() {
    uint8_t returnError = EXIT_FAILURE;
    printf("Geographer_signalConnectionDown\n");
    MqMsg msg = { .event = E_CONNECTION_DOWN };
    returnError = sendMsg(&msg );

    return returnError;
}



extern uint8_t Geographer_dateAndSendData(BeaconData* beaconsData, uint8_t beaconsDataSize, Position* currentPosition, ProcessorAndMemoryLoad * currentProcessorAndMemoryLoad) {
    uint8_t returnError = EXIT_FAILURE;
    printf("Geographer_dateAndSendData\n");
    if (connectionState == CONNECTED) {

        MqMsg msg = {
            .event = E_DATE_AND_SEND_DATA_CONNECTED,
            .beaconsData = &(*beaconsData),
            .beaconsDataSize = beaconsDataSize,
            .currentPosition = currentPosition,
            .currentProcessorAndMemoryLoad = currentProcessorAndMemoryLoad,
        };


        returnError = sendMsg(&msg);

    } else { //si pas connecté
    /*
        MqMsg msg = { .event = E_DATE_AND_SEND_DATA_ELSE };
        returnError = sendMsg(&msg, sizeof(msg));*/
        free(beaconsData);
        free(currentPosition);
        free(currentProcessorAndMemoryLoad);

    }

    return returnError;
}


//TODO rajouter mutex methode extern avec sendMsg
//mettre mutex dans la methode
