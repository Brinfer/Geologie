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
#define EXP_POSITION_NUMBER 12
#define EXP_TRAJECT_NUMBER 12
#define CALIBRATION_POSITION_NUMBER 12

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
* @brief Tableau contenant les positions experimentales 
*/
static ExperimentalPosition experimentalPositions[EXP_POSITION_NUMBER];

static CalibrationPosition calibrationPositions[CALIBRATION_POSITION_NUMBER];

static ExperimentalTraject experimentalTrajects[EXP_TRAJECT_NUMBER];

static ConnectionState connectionState;

static int calibrationCounter;

static Date currentDate;

/////////////////////////////////////////////////////////////////////////////
typedef enum  { 
    S_FORGET=0,
    S_DEATH,
    S_WATING_FOR_CONNECTION, 
    S_IDLE,
    S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION,
    S_WAITING_FOR_BE_PLACED,
    S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT,
    NB_STATE_
} State_GEOGRAPHER; // Déclencheur

const char * const State_Geographer_Name[] = {
    "S_FORGET", 
    "NB_STATE_"
};
static const char * State_Geographer_getName(int i)
{
    return State_Geographer_Name[i];
}

typedef enum  {
    E_STOP = 0, 
    E_CONNECTION_ESTABLISHED, 
    E_DATE_AND_SEND_DATA,
    E_ASK_CALIBRATION_POSITIONS,
    E_CONNECTION_DOWN,
    E_VALIDATE_POSITION,
    E_SIGNAL_END_UPDATE_ATTENUATION,
    E_SIGNAL_END_AVERAGE_CALCUL,
    NB_EVENT_GEOGRAPHER
} Event_GEOGRAPHER; // Déclencheur

const char * const Event_Geographer_Name[] = {
    "E_STOP", 
    "NB_EVENT_GEOGRAPHER"
};
static const char * Event_Geographer_getName(int i)
{
    return Event_Geographer_Name[i];
}

typedef enum {
    A_NOP=0,
    A_STOP,
    A_CONNECTION_ESTABLISHED,
    A_CONNECTION_DOWN,
    A_SET_ALL_DATA,
    A_SET_CALIBRATION_POSITIONS,
    A_ASK_4_UPDATE_ATTENUATION_COEFFICIENT,
    A_END_CALIBRATION_POSITION,
    A_END_CALIBRATION,
    A_ASK_4_AVERAGE_CALCUL

} Action_GEOGRAPHER;

const char * const Action_Geographer_Name[] = {
    "",
};

static const char * Action_Geographer_getName(int i)
{
    return Action_Geographer_Name[i];
}


//etat de destination et action associé
typedef struct{
    State_GEOGRAPHER destinationState;
    Action_GEOGRAPHER action;
} Transition_GEOGRAPHER;

typedef struct
{
    Event_GEOGRAPHER event; //evenement interne
} MqMsg; //type de message dans la BAL


static State_GEOGRAPHER myState;
static pthread_t myThread;



/**
 * The name of the queue
 */
static const char BAL[] = "/BALGeographer";

static mqd_t descripteur; //On déclare un descripteur de notre BAL qui permettra de l'ouvrir et de la fermer

static struct mq_attr attr; //On déclare un attribut pour la fonction mq_open qui est une structure spécifique à la file pour la configurer (cf l.64)



static Transition_GEOGRAPHER stateMachine [NB_STATE_-1][NB_EVENT_GEOGRAPHER] =
{
    
};


///////////////////////////////////////////////////////////////////////////////////
static BeaconData tempBeaconsData[];
static Position tempCurrentPosition;
static ProcessorAndMemoryLoad tempProcessorAndMemoryLoad;
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
 * @fn static void Geographer_mqInit ()
 * @brief initialise la boite aux lettres
 * 
 * @return 1 si erreur detectee, sinon retourne 0
*/
static int Geographer_mqInit ();

/**
 * @fn static int Geographer_mqDone ()
 * @brief ferme et detruit la boite aux lettres
 * 
 * @return 1 si erreur detectee, sinon retourne 0
*/
static int Geographer_mqDone ();

/**
 * @fn static int Geographer_mqReceive (MqMsg *this)
 * @brief ferme et detruit la boite aux lettres
 * 
 * @return 1 si erreur detectee, sinon retourne 0
 * @param this message a envoyer a la queue
*/
static int Geographer_mqReceive (MqMsg *this);


/**
 * @fn static void* run();
 * @brief methode appelee par un thread pour recevoir les action a faire grace a une boite au lettre et les effectuer
 * 
 * cette metode lira la boite au lettre et effectuera les actions 
 */
static void* run();

/**
 * @fn static void performAction(Action action);
 * @brief execution de l'action prise en parametre. Methode appelee par run
 * @param action action a faire
 */
static void performAction(Action_GEOGRAPHER action);

static Date getCurrentDate(){
    Date date=time(NULL);    //TODO determiner si c'est en secondes ou en millisecondes sur la discovery
    return date;
}

static void Geographer_mqInit (void)
{
	printf("On entre dans le Init\n");
	attr.mq_flags = 0; //Flags de la file
	attr.mq_maxmsg = MQ_MAX_MESSAGES; //Nombre maximum de messages dans la file
	attr.mq_msgsize = sizeof(MqMsg); //Taille maximale de chaque message
	attr.mq_curmsgs = 0; //Nombre de messages actuellement dans la file

	/* destruction de la BAL si toutefois préexistante */
	
	if(mq_unlink(BAL) == -1){
    	if(errno != ENOENT){
			perror("Erreur Unlink : ");
			exit(1);
		}
	}
	/* création et ouverture de la BAL */
	descripteur = mq_open(BAL, O_RDWR | O_CREAT, 0777, &attr); 
	/*On ouvre la BAL avec comme arguments : le nom de la BAL, un flag pour ouvrir la file en lecture et en ecriture (pour utiliser recEve et send) ou pour créer une MQ, le droit d'accès,l'attribut crée*/
	if (descripteur == -1)
	{
		perror("Erreur Open :\n");
	} else {
		printf("BAL ouverte\n");
	} 
	
}

static void Geographer_mqDone (void)
{
	printf("On entre dans le Done\n");

	/* fermeture de la BAL */
	mq_close(descripteur); //On ferme la BAL en mettant en paramètre le descripteur

	/* destruction de la BAL */
	mq_unlink(BAL); //On détruit la BAL en mettant en paramètre le nom de la BAL
}

static void Geographer_mqReceive (MqMsg *this)
{
	printf("On entre dans le mq_receive\n");
	mq_receive(descripteur, (char *)this, sizeof(*this), NULL);
}

static void* run(){
    printf("run\n");
    MqMsg msg; //message prenant l'event
    Action_GEOGRAPHER act;
    while (myState != S_DEATH)
    {
        Geographer_mqReceive(&msg); //Opération privée pour lire dans la BAL de AdminUI
        if (stateMachine[myState][msg.event].destinationState == S_FORGET) // aucun état de destination, on ne fait rien
        {
            //TRACE("MAE, perte evenement %s  \n", Event_getName(msg.event));
        }
        else /* on tire la transition */
        {
            //TRACE("MAE, traitement evenement %s \n",  Event_getName(msg.event));
            act = stateMachine[myState][msg.event].action;
            //TRACE("MAE, traitement action %s \n", Action_getName(act));
            performAction (act);
            myState = stateMachine[myState][msg.event].destinationState;
            //TRACE("MAE, va dans etat %s \n", State_getName(myState));
        }
    }
    return 0;
    
}

static void performAction(Action_ADMIN anAction){
    switch(anAction){
        case A_STOP:
            break;

        case A_NOP=0,:
            break;

        case A_STOP:
        
        case A_CONNECTION_ESTABLISHED :
            connectionState=CONNECTED;
            ProxyLoggerMOB_setExperimentalPositions(experimentalPositions);
            ProxyLoggerMOB_setExperimentalTrajects(experimentalTrajects);
            break;

        case A_CONNECTION_DOWN :
            connectionState=DISCONNECTED;
            break;

        case A_SET_ALL_DATA :
            currentDate=getCurrentDate();
            ProxyLoggerMOB_setAllBeaconsData(tempBeaconsData, currentDate);
            ProxyLoggerMOB_setCurrentPosition(tempCurrentPosition,currentDate);
            ProxyLoggerMOB_setProcessorAndMemoryLoad(tempProcessorAndMemoryLoad, currentDate);
            break;

        case A_SET_CALIBRATION_POSITIONS :
            calibrationCounter=0;
            ProxyGUI_setCalibrationPositions(calibrationPositions);
            break;

        case A_ASK_4_UPDATE_ATTENUATION_COEFFICIENT :
            Scanner_ask4UpdateAttenuationCoefficientFromPosition(calibrationPositions[calibrationCounter]);
            break;

        case A_END_CALIBRATION_POSITION :
            CalibrationData
            ProxyLoggerMOB_setCalibrationData(calibrationData);
            break;

        case A_END_CALIBRATION :
            calibrationCounter++;
            break;
        case A_ASK_4_AVERAGE_CALCUL:
            Scanner_ask4AverageCalcul();
            break;
        
        default:
            //TRACE("Action inconnue, pb ds la MAE de amminUi \n ");
        break;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int Geographer_new(){
    int returnError=EXIT_FAILURE;
    printf("Geographer_New\n");
    returnError=ProxyGUI_new();

    if(returnError==EXIT_SUCCESS){
        ProxyLoggerMOB_new();
    }
    if(returnError==EXIT_SUCCESS){
        Scanner_new();
    }
    Geographer_mqInit();

    return returnError;
}

extern int Geographer_free(){
    int returnError=EXIT_FAILURE;
    returnError=Geographer_mqDone();
    
    returnError=Scanner_free();

    if(returnError==EXIT_SUCCESS){
        ProxyLoggerMOB_free();
    }
    if(returnError==EXIT_SUCCESS){
        ProxyGUI_free();
    }
    Geographer_mqDone();
    return returnError;
}


extern int Geographer_ask4SignalStartGeographer(){
    int returnError=EXIT_FAILURE;
    returnError=ProxyGUI_start();

    if(returnError==EXIT_SUCCESS){
        ProxyLoggerMOB_start();
    }
    if(returnError==EXIT_SUCCESS){
        Scanner_ask4StartScanner();
    }
    myState=S_WATING_FOR_CONNECTION;
    connectionState=DISCONNECTED;
    
    pthread_create(&myThread, NULL, &run, NULL);

    return returnError;
}


extern int Geographer_ask4SignalStopGeographer(){
    int returnError=EXIT_FAILURE;
    returnError=Scanner_ask4SopScanner();

    if(returnError==EXIT_SUCCESS){
        ProxyLoggerMOB_stop();
    }
    if(returnError==EXIT_SUCCESS){
        ProxyGUI_stop();
    }
    pthread_join(myThread, NULL);

    return returnError;
}


extern int Geographer_ask4CalibrationPositions(){
    int returnError=EXIT_FAILURE;
    printf("Geographer_ask4CalibrationPositions\n");
    MqMsg msg = {.event = E_ASK_CALIBRATION_POSITIONS};
    if(mq_send(descripteur, (char *)&msg, sizeof(msg), 0)==0){
        int returnError=EXIT_SUCCESS;
    }    
    return returnError;
}


extern int Geographer_validatePosition(CalibrationPositionId calibrationPositionId){
    int returnError=EXIT_FAILURE;
    printf("Geographer_validatePosition\n");
    MqMsg msg = {.event = E_VALIDATE_POSITION};
    if(mq_send(descripteur, (char *)&msg, sizeof(msg), 0)==0){
        int returnError=EXIT_SUCCESS;
    }     
    return returnError;
}


extern int Geographer_signalEndUpdateAttenuation(){
    int returnError=EXIT_FAILURE;
    printf("Geographer_signalEndUpdateAttenuation\n");
    MqMsg msg = {.event = E_SIGNAL_END_UPDATE_ATTENUATION};
    if(mq_send(descripteur, (char *)&msg, sizeof(msg), 0)==0){
        int returnError=EXIT_SUCCESS;
    }  
    return returnError;
}


extern int Geographer_signalEndAverageCalcul(CalibrationData calibrationData[]){
    int returnError=EXIT_FAILURE;
    printf("Geographer_signalEndAverageCalcul\n");
    MqMsg msg = {.event = E_SIGNAL_END_AVERAGE_CALCUL};
    if(mq_send(descripteur, (char *)&msg, sizeof(msg), 0)==0){
        int returnError=EXIT_SUCCESS;
    }  
    return returnError;
}


extern int Geographer_signalConnectionEstablished(){
    int returnError=EXIT_FAILURE;
    printf("Geographer_signalConnectionEstablished\n");
    MqMsg msg = {.event = E_CONNECTION_ESTABLISHED};
    if(mq_send(descripteur, (char *)&msg, sizeof(msg), 0)==0){
        int returnError=EXIT_SUCCESS;
    } 
    return returnError;
}


extern int Geographer_signalConnectionDown(){
    int returnError=EXIT_FAILURE;
    printf("Geographer_signalConnectionDown\n");
    MqMsg msg = {.event = E_CONNECTION_DOWN};
    if(mq_send(descripteur, (char *)&msg, sizeof(msg), 0)==0){
        int returnError=EXIT_SUCCESS;
    } 
    return returnError;
}



extern int Geographer_dateAndSendData(BeaconData beaconsData[], Position currentPosition, ProcessorAndMemoryLoad currentProcessorAndMemoryLoad){
    int returnError=EXIT_FAILURE;
    printf("Geographer_dateAndSendData\n");
    MqMsg msg = {.event = E_DATE_AND_SEND_DATA};
    if(mq_send(descripteur, (char *)&msg, sizeof(msg), 0)==0){
        int returnError=EXIT_SUCCESS;
    } 
    return returnError;
}






