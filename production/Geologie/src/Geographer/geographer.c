/**
 * @file Geographer.c
 *
 * @brief Est en quelque sorte le chef d'orchestre de Geologie
 *
 * @version 2.0
 * @date 15-06-2021
 * @author BRIENT Nathan
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

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
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Le nombre de position experimentale.
 */
#define NB_EXPERIMENTAL_POSITION (25)

/**
 * @brief Le nombre de trajet experimentaux.
 */
#define NB_EXPERIMENTAL_TRAJECT (3)

/**
 * @brief Le nombre maximale de message dans la boite aux lettre de PostmanLOG.
 */
#define MQ_MAX_MESSAGES (10)

/**
 * @brief Le nom de la boite au lettre de Geographer.
 */
#define MQ_LABEL "/MQ_GEOGRAPHER"

/**
 * @brief Les indicateurs de la boite au lettre de Geographer.
 *
 */
#define MQ_FLAGS (O_CREAT | O_RDWR)

/**
 * @brief Les modes de la boite aux lettre de Geographer.
 */
#define MQ_MODE (S_IRUSR | S_IWUSR)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Enumeration des etat que peut prendre Geographer.
 */
typedef enum {
    S_NONE = 0,                                             /**< Etat par defaut, celui ne devrait jamais arriver */

    S_DEATH,                                                /**< Etat indiquant l'arret de la machine a etat */
    S_WATING_FOR_CONNECTION,                                /**< Etat d'attente de la connexion avec un client */
    S_IDLE,                                                 /**< Etat pricipale de la machine a etat */

    // Calibrating
    S_WAITING_FOR_BE_PLACED,                                /**< Etat d'attente durant la calibration ou GEOLOGIE attent d'etre place sur une position de calibration */
    S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION,    /**< Etat d'attente durant la calibration ou GEOLOGIE attent que Scanner est fini de calculer les coefficient d'attenuation a la position de calibration actuelle */
    S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT,               /**< Etat d'attente durant la calibration ou GEOLOGIE attent que Scanner est fini de calculer la moyenne des coefficient d'attenuation */
    S_TEST_IF_FINISH_ALL_POSITION,                          /**< Etat permettant d'estimer si tout les coefficient d'attenuation ont ete calcule */

    S_NB_STATE                                              /**< Le nombre d'etat de la machine a etat de Geographer */
} StateGeographer;

/**
 * @brief Enumeration des evenement que Geographer peut recevoir.
 */
typedef enum {
    E_NONE = 0,                             /**< Evenement par defaut, celui ne devrait jamais arriver */

    E_STOP,                                 /**< Evenement demandant l'arret de Geographer */
    E_CONNECTION_ESTABLISHED,               /**< Evenement indiquant que la connection a ete etablie */
    E_CONNECTION_DOWN,                      /**< Evenement indiquant que la connection a ete perdu */

    E_DATE_AND_SEND_DATA,                   /**< Evenement indiquant qu'il faut envoyer l'ensemble des donnees */
    E_ASK_CALIBRATION_POSITIONS,            /**< Evenement demandant a Geographer d'envoyer les position de calibration */
    E_VALIDATE_POSITION,                    /**< Evenement indiquant a Geographer de calibrer a la position de calibration actuelle */
    E_SIGNAL_END_UPDATE_ATTENUATION,        /**< Evenement indiquant a Geographer que Scanner a fini de calculer les coefficient d'attenuation a la position actuelle */
    E_FINISH_CALIBRATE_ALL_POSITION,        /**< Evenement indiquant a Geographer que l'ensemble des coefficients d'attenuations ont ete calculer */
    E_NOT_FINISH_CALIBRATE_ALL_POSITION,    /**< Evenement indiquant a Geographer que l'ensemble des coefficient d'attenuation n'ont pas tous ete calculer */
    E_SIGNAL_END_AVERAGE_CALCUL,            /**< Evenement indiquant a Geographer que le calcul de la moyenne des coefficient d'attenuation a ete fait */

    E_NB_EVENT                              /**< Le nombre d'evenement */
} EventGeographer;

/**
 * @brief Enumeration des actions que Geographer peut effectuer.
 */
typedef enum {
    A_NONE,                                 /**< Action par defaut, celle-ci ne permet de rien faire. */

    A_STOP,                                 /**< Arret de Geographer */
    A_SEND_EXPERIMENTAL_DATA,               /**< Envoie des donnees experimentales */
    A_SEND_ALL_DATA,                        /**< Envoie de toutes les donnees courantes */
    A_SET_CALIBRATION_DATA,                 /**< Envoie a GEOMOBILE les donnees de calibration */
    A_INCREASE_CALIBRATION_COUNTER,         /**< Incrementation du compteur de position calibrer */
    A_SIGNAL_END_CALIBRATION_POSITION,      /**< Signale la fin de la calibration a la position de calibration */
    A_ASK_AVERAGE_CALCUL,                   /**< Demande le calcul de la moyenne des coefficient d'attenuation */
    A_ASK_COMPUTE_ATTENUATION_COEFFICIENT,  /**< Demande de calculer les coefficient d'attenuation */
    A_SET_CALIBRATION_POSITION,             /**< Envoie a GEOMOBILE les position de calibration */

    A_NB_ACTION,                            /**< Le nombre d'action */
} ActionGeographer;

/**
 * @brief Struture contenant les donnees courantes.
 */
typedef struct {
    Position* position;                                 /**< La position courante a envoyer a GEOMOBILE */
    ProcessorAndMemoryLoad* processorAndMemoryLoad;     /**< La charge processeur et memoire a envoyer a GEOMOBILE */
    BeaconData* beaconsData;                            /**< Les donnees balises courante a envoyer a GEOMOBILE */
    int8_t nbBeaconData;                                /**< Le nombre de balises composant les donnees balises */
} DataCurrent;

/**
 * @brief Les donnees de calibration.
 *
 */
typedef struct {
    CalibrationData* calibrationData;   /**< Les donnees de calibration a envoyer a GEOMOBILE */
    int8_t nbCalibrationData;           /**< Le nombre de donnees de calibration */
} DataCalibration;

typedef union {
    DataCurrent current;                            /**< Les donnees courantes a envoyer a GEOMOBILE */
    DataCalibration calibration;                    /**< Les donnees de calibration a envoyer a GEOMOBILE. */
    CalibrationPositionId calibrationPositionId;    /**< L'identifiant de calibration ou se calibrer */
} DataToShare;

/**
 * @brief La structure correspondant aux message passant par la boite aux letres de Geographer.
 */
typedef struct {
    EventGeographer event;  /**< L'evement associer au message */
    DataToShare data;       /**< L'union des donnees a passer a Geographer et qu'il devra traiter. */
} MqMsgGeographer;

/**
 * @brief Contient les informations necessaire a une transition entre les etats.
 */
typedef struct {
    StateGeographer destinationState;   /**< L'etat suivant */
    ActionGeographer action;            /**< L'action a effectuer lors de la transition */
} TransitionGeographer;

/**
 * @brief L'etat courant de la machine a etat de Geographer.
 */
static StateGeographer currentState;

/**
 * @brief Le thread de Geographer.
 */
static pthread_t geographerThread;

/**
 * @brief La queue de Geographer
 */
static mqd_t geographerMq;

/**
 * @brief Compteur de calibration.
 */
static uint8_t calibrationCounter;

/**
* @brief Tableau contenant les positions experimentales
*/
static const ExperimentalPosition EXPERIMENTAL_POSITIONS[NB_EXPERIMENTAL_POSITION] = {
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
*/
static const  CalibrationPosition CALIBRATION_POSITION[NB_CALIBRATION_POSITION] = {
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
*/
static Position TRAJECT_1[] = {
    {.X = 100, .Y = 1300 },
    {.X = 1000, .Y = 1300 },
    {.X = 1000, .Y = 200 },
    {.X = 100, .Y = 200 },
    {.X = 100, .Y = 1300}
};

/**
* @brief Tableau contenant le deuxieme trajet experimental
*/
static Position TRAJECT_2[] = {
    {.X = 50, .Y = 1300 },
    {.X = 1050, .Y = 1000 }
};

/**
* @brief Tableau contenant le troisieme trajet experimental
*/
static Position TRAJECT_3[] = {
    {.X = 950, .Y = 1000 },
    {.X = 350, .Y = 1200 },
    {.X = 50, .Y = 1000 }
};

/**
* @brief Tableau contenant les trajets experimentaux
*/
static const ExperimentalTraject EXPERIMENTAL_TRAJECTS[NB_EXPERIMENTAL_TRAJECT] = {
    {.id = 1, .traject = TRAJECT_1, .nbPosition = 5},
    {.id = 2, .traject = TRAJECT_2, .nbPosition = 2},
    {.id = 3, .traject = TRAJECT_3, .nbPosition = 3}
};

/**
 * @brief L'ensemble des #TransitionGeographer a effectuer pour la machine a etat en fonction de l'etat actuel et de l'evenement.
 *
 */
static const TransitionGeographer stateMachine[S_NB_STATE][E_NB_EVENT] = {
    [S_WATING_FOR_CONNECTION] [E_CONNECTION_ESTABLISHED] = {S_IDLE, A_SEND_EXPERIMENTAL_DATA},
    [S_WATING_FOR_CONNECTION][E_DATE_AND_SEND_DATA] = {S_WATING_FOR_CONNECTION, A_NONE},
    [S_WATING_FOR_CONNECTION][E_STOP] = {S_DEATH, A_STOP},

    [S_IDLE][E_ASK_CALIBRATION_POSITIONS] = {S_WAITING_FOR_BE_PLACED, A_SET_CALIBRATION_POSITION},
    [S_IDLE][E_DATE_AND_SEND_DATA] = {S_IDLE, A_SEND_ALL_DATA},
    [S_IDLE][E_CONNECTION_DOWN] = {S_WATING_FOR_CONNECTION, A_NONE},
    [S_IDLE][E_STOP] = {S_DEATH, A_STOP},

    [S_WAITING_FOR_BE_PLACED][E_VALIDATE_POSITION] = {S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION, A_ASK_COMPUTE_ATTENUATION_COEFFICIENT},
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
 * @return int8_t -1 en cas d'erreur, 0 sinon.
*/
static Date getCurrentDate();

/**
 * @brief initialise la boite aux lettres
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
*/
static int8_t setUpMq();

/**
 * @brief Ferme et detruit la boite aux lettres
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
*/
static int8_t tearDoneMq();

/**
 * @brief Ferme et detruit la boite aux lettres
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
*/
static int8_t readMsgMq(MqMsgGeographer* dest);

/**
 * @brief methode appelee par un thread pour recevoir les action a faire grace a une boite au lettre et les effectuer.
 *
 * Cette methode lira la boite au lettre et effectuera les actions.
 */
static void* runGeographer();

/**
 * @brief Envoye des messages a la queue
 *
 * @param sizeOfMsg La taille du message a envoyer
 * @param msg Les donnees a utiliser pour effectuer l'action
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t sendMsgMq(MqMsgGeographer* msg);

/**
 * @brief Execute la fonction associer a l'#ActionGeographer et passe les donnees contenue dans #MqMsgGeographer.
 *
 * @param action L'action a effectuer.
 * @param msg Le message contenant les donnees a passer aux fonction.
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t performAction(ActionGeographer action, const MqMsgGeographer* msg);

/**
 * @brief Signale en interne que toutes les positions de calibration ont ete calculer.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t signalFinishCalibrateAllPosition(void);

/**
 * @brief Signale en interne que toutes les positions de calibration n'ont pas ete calculer.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t signalNotFinishCalibrateAllPosition(void);

/**
 * @brief N'effectue rien
 *
 * @retval int8_t 0
 */
static int8_t actionNone(void);

/**
 * @brief Execute le processus d'arret de Geographer.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t actionStop(void);

/**
 * @brief Envoie a GEOMOBILE les donnees experimentales.
 *
 * @param experimentalPositions Les positions experimentales
 * @param nbExperimentalPosition Le nombre de position experimentales
 * @param experimentalTrajects Les trajects experimentaux
 * @param nbExperimentalTraject Le nombre de trajet experimentaux
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t actionSendExperimentalData(const ExperimentalPosition* experimentalPositions, uint8_t nbExperimentalPosition, const ExperimentalTraject* experimentalTrajects, uint8_t nbExperimentalTraject);

/**
 * @brief Envoie les donnees d'experimentation a GEOMOBILE
 *
 * @param beaconData Les donnees balises
 * @param nbBeaconData Le nombre de balise / Le nombre de donnees balise
 * @param position La position de GEOLOGIE
 * @param processorAndMemoryLoad La charge processeur et memoire de GEOLOGIE
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t actionSendAllData(BeaconData* beaconData, uint8_t nbBeaconData, Position* position, ProcessorAndMemoryLoad* processorAndMemoryLoad);

/**
 * @brief Envoie les position de calibration a GEOMOBILE.
 *
 * @param calibrationPosition Les positions de calibration.
 * @param nbCalibrationPosition Le nombre de position de calibration
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t actionSetCalibrationPosition(const CalibrationPosition* calibrationPosition, uint8_t nbCalibrationPosition);

/**
 * @brief Incremente la valeur de #calibrationCounter
 *
 * @retval int8_t 0
 */
static int8_t actionIncreaseCalibrationCounter(void);

/**
 * @brief Signal a GEOMOBILE la fin de la calibration a la position de calibration actuelle.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t actionSignalEndCalibrationPosition(void);

/**
 * @brief Demande a Scanner de calculer la moyenne des coefficient de calibration
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t actionAskAverageCalcul(void);

/**
 * @brief Demande a Scanner de se calibrer a la position de calibration dont l'#CalibrationPositionId est donnee.
 *
 * @param calibrationPositionId L'identifiant de la position de calibration ou se calibrer
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
static int8_t actionAskComputeAttenuationCoefficient(CalibrationPositionId calibrationPositionId);

/**
 * @brief Envoie a GEOMOBILE les donnees de calibration.
 *
 * @param calibrationData Les donnees de calibration
 * @param nbCalibrationData Le nombre de donnees de calibration
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
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

    MqMsgGeographer msg = { .event = E_STOP };
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

    MqMsgGeographer msg = { .event = E_CONNECTION_ESTABLISHED };
    returnError = sendMsgMq(&msg);

    ERROR(returnError < 0, "[Geographer] Fail to send the message signal connection established ... Abandonnement");

    return returnError;
}

extern int8_t Geographer_signalConnectionDown() {
    int8_t returnError;

    MqMsgGeographer msg = { .event = E_CONNECTION_DOWN };
    returnError = sendMsgMq(&msg);

    ERROR(returnError < 0, "[Geographer] Fail to send the message signal connection down ... Abandonnement");

    return returnError;
}

extern int8_t Geographer_askCalibrationPositions() {
    int8_t returnError;

    MqMsgGeographer msg = { .event = E_ASK_CALIBRATION_POSITIONS };
    returnError = sendMsgMq(&msg);

    ERROR(returnError < 0, "[Geographer] Fail to send the message stop ... retry");

    return returnError;
}

extern int8_t Geographer_validatePosition(CalibrationPositionId calibrationPositionId) {
    int8_t returnError;

    MqMsgGeographer msg = {
        .event = E_VALIDATE_POSITION,
        .data.calibrationPositionId = calibrationPositionId,
    };

    returnError = sendMsgMq(&msg);

    ERROR(returnError < 0, "[Geographer] Fail to send the message validate position ... Abandonnement");

    return returnError;
}

extern int8_t Geographer_signalEndUpdateAttenuation() {
    int8_t returnError;

    MqMsgGeographer msg = { .event = E_SIGNAL_END_UPDATE_ATTENUATION };
    returnError = sendMsgMq(&msg);

    ERROR(returnError < 0, "[Geographer] Fail to send the message singal end update attenuation ... Abandonnement");

    return returnError;
}

extern int8_t Geographer_signalEndAverageCalcul(CalibrationData* calibrationData, int8_t nbCalibration) { //comment
    int8_t returnError;

    MqMsgGeographer msg = { .event = E_SIGNAL_END_AVERAGE_CALCUL };

    returnError = sendMsgMq(&msg);
    ERROR(returnError < 0, "[Geographer] Fail to send the message signal end average calcul ... Abandonnement");

    return returnError;
}

extern int8_t Geographer_dateAndSendData(BeaconData* beaconsData, int8_t beaconsDataSize, Position* currentPosition, ProcessorAndMemoryLoad* currentProcessorAndMemoryLoad) {
    int8_t returnError = EXIT_FAILURE;

    MqMsgGeographer msg = {
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
    attr.mq_msgsize = sizeof(MqMsgGeographer);
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

static int8_t readMsgMq(MqMsgGeographer* dest) {
    int8_t returnError = EXIT_SUCCESS;

    returnError = mq_receive(geographerMq, (char*) dest, sizeof(MqMsgGeographer), NULL); // put char to avoid a warning
    ERROR(returnError < 0, "[Geographer] Error when reading the message in the queue");

    return returnError;
}

static int8_t sendMsgMq(MqMsgGeographer* msg) {
    int8_t returnError;
    errno = 0;

    returnError = mq_send(geographerMq, (char*) msg, sizeof(MqMsgGeographer), 0); // put char to avoid a warning
    ERROR(returnError < 0, "[Geographer] Error when sending the message in the queue");

    return returnError;
}

static void* runGeographer() {
    while (currentState != S_DEATH) {
        MqMsgGeographer msg;

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

static int8_t performAction(ActionGeographer action, const MqMsgGeographer* msg) {
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
            returnError = actionSendExperimentalData(EXPERIMENTAL_POSITIONS, NB_EXPERIMENTAL_POSITION, EXPERIMENTAL_TRAJECTS, NB_EXPERIMENTAL_TRAJECT);
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
            returnError = actionSetCalibrationPosition(CALIBRATION_POSITION, NB_CALIBRATION_POSITION);
            break;

        case A_ASK_COMPUTE_ATTENUATION_COEFFICIENT:
            returnError = actionAskComputeAttenuationCoefficient(msg->data.calibrationPositionId);
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

static int8_t actionSendExperimentalData(const ExperimentalPosition* experimentalPositions, uint8_t nbExperimentalPosition, const ExperimentalTraject* experimentalTrajects, uint8_t nbExperimentalTraject) {
    int8_t returnErrorPosition;
    int8_t returnErrorTraject;

    returnErrorPosition = ProxyLoggerMOB_setExperimentalPositions(experimentalPositions, nbExperimentalPosition);
    if (returnErrorPosition < 0) {
        ERROR(true, "[Geographer] Fail to send the experimental positions ... Retry");

        returnErrorPosition = ProxyLoggerMOB_setExperimentalPositions(experimentalPositions, nbExperimentalPosition);
        ERROR(returnErrorPosition < 0, "[Geographer] Fail to send the experimental positions ... Abandonment");
    }


    returnErrorTraject = ProxyLoggerMOB_setExperimentalTrajects(experimentalTrajects, NB_EXPERIMENTAL_TRAJECT);
    if (returnErrorTraject < 0) {
        ERROR(true, "[Geographer] Fail to send the experimental trajects ... Retry");

        returnErrorTraject = ProxyLoggerMOB_setExperimentalTrajects(experimentalTrajects, NB_EXPERIMENTAL_TRAJECT);
        ERROR(returnErrorTraject < 0, "[Geographer] Fail to send the experimental trajects ... Abandonment");
    }


    ERROR((returnErrorTraject + returnErrorPosition) < 0, "[Geographer] Fail to send the experimental data ... Abandonment");

    return (returnErrorTraject + returnErrorPosition) < 0 ? -1 : 0;
}

static int8_t actionSendAllData(BeaconData* beaconData, uint8_t nbBeaconData, Position* position, ProcessorAndMemoryLoad* processorAndMemoryLoad) {
    Date currentDate = getCurrentDate();
    int8_t returnErrorBeaconData = 0;
    int8_t returnErrorCurrentPosition = 0;
    int8_t returnErrorLoad = 0;

    returnErrorBeaconData = ProxyLoggerMOB_setAllBeaconsData(beaconData, nbBeaconData, currentDate);
    if (returnErrorBeaconData < 0) {
        ERROR(true, "[Geographer] Fail to send the beacons data ... Retry");
        returnErrorBeaconData = ProxyLoggerMOB_setAllBeaconsData(beaconData, nbBeaconData, currentDate);
        ERROR(returnErrorBeaconData < 0, "[Geographer] Fail to send the beacons data ... Abandonment");
    }

    returnErrorCurrentPosition = ProxyLoggerMOB_setCurrentPosition(position, currentDate);
    if (returnErrorCurrentPosition < 0) {
        ERROR(true, "[Geographer] Fail to send the current position ... Retry");
        returnErrorCurrentPosition = ProxyLoggerMOB_setCurrentPosition(position, currentDate);
        ERROR(returnErrorCurrentPosition < 0, "[Geographer] Fail to send the current position ... Abandonment");
    }

    returnErrorLoad = ProxyLoggerMOB_setProcessorAndMemoryLoad(processorAndMemoryLoad, currentDate);
    if (returnErrorLoad < 0) {
        ERROR(true, "[Geographer] Fail to send the current processor and the memory load ... Retry");
        returnErrorLoad = ProxyLoggerMOB_setProcessorAndMemoryLoad(processorAndMemoryLoad, currentDate);
        ERROR(returnErrorLoad < 0, "[Geographer] Fail to send the beacons data ... Abandonment");
    }

    ERROR((returnErrorBeaconData + returnErrorCurrentPosition + returnErrorLoad) < 0, "[Geographer] Fail to send a curent data ... Abandonment");

    free(beaconData);
    free(processorAndMemoryLoad);
    free(position);

    return (returnErrorBeaconData + returnErrorCurrentPosition + returnErrorLoad) < 0 ? -1 : 0;
}

static int8_t actionSetCalibrationPosition(const CalibrationPosition* calibrationPosition, uint8_t nbCalibrationPosition) {
    int8_t returnErrorSetData;

    calibrationCounter = 0;

    returnErrorSetData = ProxyGUI_setCalibrationPositions(calibrationPosition, nbCalibrationPosition);
    if (returnErrorSetData < 0) {
        ERROR(true, "[Geographer] Fail to send the calibration data ... Retry");

        returnErrorSetData = ProxyGUI_setCalibrationPositions(calibrationPosition, nbCalibrationPosition);
        ERROR(returnErrorSetData < 0, "[Geographer] Fail to send the calibration data ... Abandonment");
    }

    return returnErrorSetData;
}

static int8_t actionIncreaseCalibrationCounter(void) {
    calibrationCounter++;

    TRACE("[Geographer] calibrate %d / %d%s", calibrationCounter, NB_CALIBRATION_POSITION, "\n");
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

    if (calibrationCounter >= NB_CALIBRATION_POSITION - 1) {    // The increasing of the counter is made after
        returnErrorMq = signalFinishCalibrateAllPosition();
    } else {
        returnErrorMq = signalNotFinishCalibrateAllPosition();
    }

    if (returnErrorMq < 0) {
        ERROR(true, "[Geographer] Fail to emit the internal signal for the the next state ... Retry");

        if (calibrationCounter >= NB_CALIBRATION_POSITION - 1) {    // The increasing of the counter is made after
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
    //Scanner_ask4AverageCalcul();
    return (returnErrorMq + returnErrorSignal) < 0 ? -1 : 0;
}

static int8_t actionAskAverageCalcul(void) {
    Scanner_ask4AverageCalcul();

    return 0;
}

static int8_t actionAskComputeAttenuationCoefficient(CalibrationPositionId calibrationPositionId) {
    int8_t returnError = -1;
    CalibrationPosition calibrationPosition;

    for (uint8_t i = 0; i < NB_CALIBRATION_POSITION; i++) {
        if (CALIBRATION_POSITION[i].id == calibrationPositionId) {
            calibrationPosition = CALIBRATION_POSITION[i];
            returnError = 0;
            break;
        }
    }

    if (returnError < 0) {
        ERROR(true, "[Geographer] Can't find a position associated to the id");
        // TODO make a better exit
        calibrationCounter--;                       // Prevent the increasing in the next step
        Geographer_signalEndUpdateAttenuation();    // To not be blocked
    } else {
        Scanner_ask4UpdateAttenuationCoefficientFromPosition(&calibrationPosition);
    }


    return returnError;
}

static int8_t actionSetCalibrationData(const CalibrationData* calibrationData, uint8_t nbCalibrationData) {
    TRACE("[Geographer] action Set Calibration Data%s", "\n");

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
    MqMsgGeographer msg = { .event = E_FINISH_CALIBRATE_ALL_POSITION };

    int8_t returnError = sendMsgMq(&msg);
    ERROR(returnError < 0, "[Geographer] Fail to signal in intern the message finnish calibrate all position ... Abandonnement");

    return returnError;
}

static int8_t signalNotFinishCalibrateAllPosition(void) {
    MqMsgGeographer msg = { .event = E_NOT_FINISH_CALIBRATE_ALL_POSITION };

    int8_t returnError = sendMsgMq(&msg);
    ERROR(returnError < 0, "[Geographer] Fail to signal in intern the message not finnish calibrate all position ... Abandonnement");

    return returnError;
}
