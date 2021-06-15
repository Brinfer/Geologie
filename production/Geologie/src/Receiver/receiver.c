/**
 * @file receiver.c
 *
 * @version 2.0
 * @date 5/06/2021
 * @author Gabriel LECENNE
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#include <stdlib.h>
#include <errno.h>
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <inttypes.h>
#include <mqueue.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>

#include "receiver.h"
#include "../TranslatorBeacon/translatorBeacon.h"
#include "../Watchdog/watchdog.h"
#include "../Scanner/scanner.h"
#include "../common.h"
#include "../tools.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Variable et structure privee

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NB_BEACONS_AVAILABLE (5)
#define NB_MAX_ADVERTISING_CHANNEL (100)
#define NB_MAX_BEACONS_AVAILABLE (10)

#define BEACONS_UUID_1 (0x18)
#define BEACONS_UUID_2 (0x1A)

#define MQ_MAX_MESSAGES (5)

static BeaconSignal beaconsSignal[NB_MAX_BEACONS_AVAILABLE];
static BeaconsChannel * beaconsChannel;
static uint32_t NbBeaconsChannel;
static uint32_t NbBeaconsSignal;

typedef enum{
	S_FORGET=0,
	S_BEGINNING,
    S_SCANNING,
    S_TRANSLATING,
    S_DEATH,
    NB_STATE
}State_RECEIVER;

typedef enum {
    E_STOP = 0,
    E_ASK_BEACONS_SIGNAL,
    E_MAJ_BEACONS_CHANNEL,
	E_TIME_OUT,
	E_TRANSLATING_DONE,
    NB_EVENT_RECEIVER
} Event_RECEIVER;

typedef enum {
    A_NOP = 0,
    A_STOP,
    A_SEND_BEACONS_SIGNAL,
    A_MAJ_BEACONS_CHANNELS,
	A_TRANSLATE,
    NB_ACTION_RECEIVER
} Action_RECEIVER;

typedef struct {
    State_RECEIVER destinationState;
    Action_RECEIVER action;
} Transition_RECEIVER;

static Transition_RECEIVER stateMachine[NB_STATE - 1][NB_EVENT_RECEIVER] =
{
    [S_BEGINNING] [E_MAJ_BEACONS_CHANNEL] = {S_SCANNING, A_MAJ_BEACONS_CHANNELS},
    [S_SCANNING] [E_ASK_BEACONS_SIGNAL] = {S_SCANNING, A_SEND_BEACONS_SIGNAL},
	[S_SCANNING] [E_TIME_OUT] = {S_TRANSLATING, A_TRANSLATE},
	[S_SCANNING] [E_STOP] = {S_DEATH, A_STOP},
    [S_TRANSLATING] [E_TRANSLATING_DONE] = {S_SCANNING, A_MAJ_BEACONS_CHANNELS},
	[S_TRANSLATING] [E_STOP] = {S_DEATH, A_STOP},

};

struct hci_request ble_hci_request(uint16_t ocf, uint8_t clen, void * status, void * cparam)
{
	struct hci_request rq;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = ocf;
	rq.cparam = cparam;
	rq.clen = clen;
	rq.rparam = status;
	rq.rlen = 1;
	return rq;
}

State_RECEIVER myState;
static pthread_t myThreadMq;

static const char BAL[] = "/BALReceiver";
static mqd_t descripteur;
static struct mq_attr attr;

typedef struct {
    Event_RECEIVER event;
} MqMsgReceiver;

static Watchdog * wtd_TScan;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions static

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
 * @fn static void sendMsg(MqMsgReceiver* this)
 * @brief Envoie des messages a la BAL
 *
 * @param this structure du message envoye a la BAL
 * @return renvoie 1 si une erreur est detectee, sinon 0
*/
static int8_t sendMsg(MqMsgReceiver* this);

/**
 * @fn static void mqReceive(MqMsgReceiver* this)
 * @brief Va chercher les messages dans la BAL
 *
 * @param msg structure du message a recuperer
 * @return renvoie 1 si une erreur est detectee, sinon 0
*/
static void mqReceive(MqMsgReceiver* this);

/**
 * @fn static void Receiver_translateChannelToBeaconsSignal()
 * @brief traduit le tableau en variable globale de BeaconChannel en BeaconSignal
 */

static void Receiver_translateChannelToBeaconsSignal();

/**
 * @fn reset_beaconsChannelAndSignal()
 * @brief methode privee permettant de supprimer l'ensemble des trames et des BeaconsSignal contenus dans les variable globales.
 */

static void reset_beaconsChannelAndSignal();

/**
 * @fn static void Receiver_getAllBeaconsChannel()
 * @brief methode privee permettant de memoriser dans beaconsChannel l'ensemble des trames des balises
 */

static void Receiver_getAllBeaconsChannel();

/**
 * @fn static void performAction(Action_SCANNER action, MqMsgReceiver * msg)
 * @brief execute les fonctions a realiser en fonction du parametre action
 *
 * @param action action a executer
 * @param msg message qui contient les donnees necessaire a l'execution de la fonction
*/
static void performAction(Action_RECEIVER action, MqMsgReceiver * msg);

/**
 * @fn static void * run()
 * @brief thread qui lit la BAL et met a jour l'action a realiser
*/
static void * run();

/**
 * @fn static void time_out()
 * @brief fonction de callback du watchdog wtd_TScan
*/
static void time_out();


static void mqInit() {
    TRACE("[Scanner] : mqInit %s","\n");

    attr.mq_flags = 0; //Flags de la file
    attr.mq_maxmsg = MQ_MAX_MESSAGES; //Nombre maximum de messages dans la file
    attr.mq_msgsize = sizeof(MqMsgReceiver); //Taille maximale de chaque message
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
    }
}

static int8_t sendMsg(MqMsgReceiver* msg) {
    int8_t returnError = EXIT_FAILURE;
    if (mq_send(descripteur, (char*) msg, sizeof(msg), 0) == 0) {
        returnError = EXIT_SUCCESS;
    }
    return returnError;
}

static void mqReceive(MqMsgReceiver* msg) {

    mq_receive(descripteur, (char*) msg, sizeof(msg), NULL);

}

static void Receiver_translateChannelToBeaconsSignal(){
	TRACE("[Receiver] ON VA TRADUIRE LES CHANNELS%s", "\n"); 
	uint8_t index_signal;
	uint8_t index_channel;

	for (index_channel = 0; index_channel < NbBeaconsChannel; index_channel++)
	{
		BeaconSignal signal;
		bool find = FALSE;
		signal = TranslatorBeacon_translateChannelToBeaconsSignal(&(beaconsChannel[index_channel]));
		for(index_signal = 0; index_signal < NbBeaconsSignal; index_signal++)
		{
			if(signal.name == beaconsSignal[index_signal].name){
				beaconsSignal[index_signal] = signal;
				find = TRUE;
			}
		}
		if(find == FALSE){
			beaconsSignal[index_signal] = signal;
			index_signal++;
			NbBeaconsSignal++;
		}
	}

	MqMsgReceiver msg = {
                .event = E_TRANSLATING_DONE
                };
    sendMsg(&msg);

}

static void reset_beaconsChannelAndSignal(){
	memset(beaconsChannel, 0, NbBeaconsChannel * sizeof(BeaconsChannel));
	memset(beaconsSignal, 0, NbBeaconsSignal * sizeof(BeaconSignal));
}

static void Receiver_getAllBeaconsChannel(){
	TRACE("[Receiver] ON VA CHERCHER LES BALISES%s", "\n"); 
    int32_t ret, status;

	// Get HCI device.

	const int32_t device = hci_open_dev(hci_get_route(NULL));
	if ( device < 0 ) {
		perror("Failed to open HCI device.");
	}

	// Set BLE scan parameters.

	le_set_scan_parameters_cp scan_params_cp;
	memset(&scan_params_cp, 0, sizeof(scan_params_cp));
	scan_params_cp.type 			= 0x00;
	scan_params_cp.interval 		= htobs(0x0010);
	scan_params_cp.window 			= htobs(0x0010);
	scan_params_cp.own_bdaddr_type 	= 0x00; // Public Device Address (default).
	scan_params_cp.filter 			= 0x00; // Accept all.


	struct hci_request scan_params_rq = ble_hci_request(OCF_LE_SET_SCAN_PARAMETERS, LE_SET_SCAN_PARAMETERS_CP_SIZE, &status, &scan_params_cp);

	ret = hci_send_req(device, &scan_params_rq, 1000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		perror("Failed to set scan parameters data.");
	}

	// Set BLE events report mask.

	le_set_event_mask_cp event_mask_cp;
	memset(&event_mask_cp, 0, sizeof(le_set_event_mask_cp));
	int i = 0;
	for ( i = 0 ; i < 8 ; i++ ) event_mask_cp.mask[i] = 0xFF;

	struct hci_request set_mask_rq = ble_hci_request(OCF_LE_SET_EVENT_MASK, LE_SET_EVENT_MASK_CP_SIZE, &status, &event_mask_cp);
	ret = hci_send_req(device, &set_mask_rq, 1000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		perror("Failed to set event mask.");
	}

	// Enable scanning.

	le_set_scan_enable_cp scan_cp;
	memset(&scan_cp, 0, sizeof(scan_cp));
	scan_cp.enable 		= 0x01;	// Enable flag.
	scan_cp.filter_dup 	= 0x00; // Filtering disabled.

	struct hci_request enable_adv_rq = ble_hci_request(OCF_LE_SET_SCAN_ENABLE, LE_SET_SCAN_ENABLE_CP_SIZE, &status, &scan_cp);
	ret = hci_send_req(device, &enable_adv_rq, 1000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		perror("Failed to enable scan.");
	}

	// Get Results.

	struct hci_filter nf;
	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);
	if (setsockopt(device, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0){
		hci_close_dev(device);
		perror("Could not set socket options\n");
	}

	uint8_t buf[HCI_MAX_EVENT_SIZE];
	evt_le_meta_event * meta_event;
	BeaconsChannel * info;
	static uint32_t index_channel = 0;
	uint32_t uuid[2];
	uint32_t len;

	while(1){
		len = read(device, buf, sizeof(buf));
		if ( len >= HCI_EVENT_HDR_SIZE ){
			meta_event = (evt_le_meta_event*)(buf+HCI_EVENT_HDR_SIZE+1);
			if ( meta_event->subevent == EVT_LE_ADVERTISING_REPORT){
				uint8_t reports_count = meta_event->data[0];
				void * offset = meta_event->data + 1;
				while ( reports_count-- ) {
					info = (BeaconsChannel *)offset;
					if ( ret < 0 ) {
						hci_close_dev(device);
						perror("Failed to enable scan.");
					}

					memcpy(uuid, info->data + 21, 2);

					if(uuid[0] == BEACONS_UUID_1 && uuid[1]== BEACONS_UUID_2){
						beaconsChannel[index_channel] = *(info);
						index_channel ++;
						NbBeaconsChannel++;
					}

					offset = info->data + info->length + 2;
				}
			}
		}
	}

	// Disable scanning.

	memset(&scan_cp, 0, sizeof(scan_cp));
	scan_cp.enable = 0x00;	// Disable flag.

	struct hci_request disable_adv_rq = ble_hci_request(OCF_LE_SET_SCAN_ENABLE, LE_SET_SCAN_ENABLE_CP_SIZE, &status, &scan_cp);
	ret = hci_send_req(device, &disable_adv_rq, 1000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		perror("Failed to disable scan.");
	}

	hci_close_dev(device);
}

static void performAction(Action_RECEIVER action, MqMsgReceiver * msg){
    switch (action) {

        case A_SEND_BEACONS_SIGNAL:
			TRACE("[Receiver] SEND BEACONS SIGNAL%s", "\n"); 
            Scanner_setAllBeaconsSignal(beaconsSignal, NbBeaconsSignal);
            break;

        case A_MAJ_BEACONS_CHANNELS:
			TRACE("[Receiver] MAJ BEACONS CHANNEL%s", "\n"); 
			Watchdog_start(wtd_TScan);
			reset_beaconsChannelAndSignal();
            Receiver_getAllBeaconsChannel();
            break;

		case A_TRANSLATE:
			Receiver_translateChannelToBeaconsSignal(beaconsChannel);

		case A_STOP:
			break;
        default:
            break;
    }

}

static void * run(){

    MqMsgReceiver msg;

    Action_RECEIVER action;

    while (myState != S_DEATH) {

        mqReceive(&msg);
        action = stateMachine[myState][msg.event].action;
		if (stateMachine[myState][msg.event].destinationState != S_FORGET) {
			performAction(action, &msg);
			myState =  stateMachine[myState][msg.event].destinationState;
		}else {
        	TRACE("MAE, perte evenement %i  \n", stateMachine[myState][msg.event].destinationState);
    	}
        
    }

   return 0;
}

static void time_out(){
    MqMsgReceiver msg = {
                .event = E_TIME_OUT
                };
    sendMsg(&msg);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions extern

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


extern void Receiver_new(){
    mqInit();
	wtd_TScan = Watchdog_construct(1000000, &(time_out));
}

extern int8_t Receiver_ask4StartReceiver(){
    TRACE("[Receiver] Receiver_ask4StartReceiver%s", "\n");

	int8_t returnError = EXIT_FAILURE;
    myState = S_BEGINNING;
    // MqMsgReceiver msg = {
    //             .event = E_MAJ_BEACONS_CHANNEL
    //             };
    // sendMsg(&msg);
	Watchdog_start(wtd_TScan);
	// reset_beaconsChannelAndSignal();
    Receiver_getAllBeaconsChannel();
    returnError = pthread_create(&myThreadMq, NULL, &run, NULL);
	if(returnError != -1){
		TRACE("[Receiver] Reveiver RUN lancé%s", "\n"); 

	}
	assert(returnError >= 0);
	return returnError;
}

extern int8_t Receiver_ask4StopReceiver(){
    TRACE("[Receiver] Receiver_ask4StopReceiver%s", "\n");
    MqMsgReceiver msg = {
                .event = E_STOP
                };
    sendMsg(&msg);
    int8_t returnError = EXIT_FAILURE;
	returnError = pthread_join(myThreadMq, NULL);
	return returnError;
}

extern void Receiver_free(){
	myState = S_DEATH;
    Watchdog_destroy(wtd_TScan);
}

extern int8_t Receiver_ask4BeaconsSignal(){
    TRACE("[Receiver] Receiver_ask4BeaconsSignal%s", "\n");

	int8_t returnError = EXIT_FAILURE;
    MqMsgReceiver msg = {
                .event = E_ASK_BEACONS_SIGNAL
                };
    returnError = sendMsg(&msg);
	return returnError;
}
