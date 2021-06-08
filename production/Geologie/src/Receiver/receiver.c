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

#include "receiver.h"
#include "../translatorBeacon/translatorBeacon.h"
#include "../common.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Variable et structure privee

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NB_BEACONS_AVAILABLE 3

#define BEACONS_UUID 0x181A

static BeaconsSignal beaconsSignal[NB_BEACONS_AVAILABLE];

static BeaconsChannel * beaconsChannel[NB_BEACONS_AVAILABLE];

static int index_channel = 0;

static int channel_INDEX;

typedef enum{
    S_SCANNING = 0,
    S_TRANSLATING,
    S_DEATH,
    NB_STATE
}State_RECEIVER;

typedef enum {
    E_STOP = 0,
    E_ASK_BEACONS_SIGNAL,
    E_MAJ_BEACONS_CHANNEL,
    NB_EVENT_RECEIVER
} Event_RECEIVER;

typedef enum {
    A_NOP = 0,
    A_STOP,
    A_SEND_BEACONS_SIGNAL,
    A_MAJ_BEACONS_CHANNELS,
    NB_ACTION_RECEIVER
} Action_RECEIVER;

typedef struct {
    State_RECEIVER destinationState;
    Action_RECEIVER action;
} Transition_RECEIVER;

static Transition_RECEIVER stateMachine[NB_STATE - 1][NB_EVENT_RECEIVER] =
{
    [S_SCANNING] [E_STOP] = {S_DEATH, A_STOP},
    [S_SCANNING] [E_ASK_BEACONS_SIGNAL] = {S_SCANNING, A_SEND_BEACONS_SIGNAL},
    [S_SCANNING] [E_MAJ_BEACONS_CHANNEL] = {S_SCANNING, A_MAJ_BEACONS_CHANNELS},
    [S_TRANSLATING] [E_STOP] = {S_DEATH, A_STOP},
    [S_TRANSLATING] [E_MAJ_BEACONS_CHANNEL] = {S_SCANNING, A_MAJ_BEACONS_CHANNELS}
};

struct hci_request ble_hci_request(uint16_t ocf, int clen, void * status, void * cparam)
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions static

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/**

 * @fn static void Receiver_translateChannelToBeaconsSignal(BeaconsChannel channel[])()

 * @brief methode privee permettant traduire les trames memorisees dans beaconsChannel

 * @param Tableau de BeaconsChannel a traduire

 */

static void Receiver_translateChannelToBeaconsSignal(){
    int i;
	int index_signal = 0;
	int index_channel = 0;
	BeaconsSignal signal;
	printf("TRANSLATING\n");

	signal = TranslatorBeacon_translateChannelToBeaconsSignal(beaconsChannel[0]);
	printf("Index n° : %d\n", 0);
	printf("Name : %s\n", signal.name);
	printf("X : %d\n", signal.position.X);
	printf("Y : %d\n", signal.position.Y);
	printf("UUID : %d\n", signal.uuid[0]);
	printf("RSSI : %d\n", signal.rssi);
	



	/*for (i = 0; i < sizeof(BeaconsChannel); i++)
	{
		signal = TranslatorBeacon_translateChannelToBeaconsSignal(beaconsChannel[index_channel]);
		printf("Index n° : %d\n", i);
		printf("Name : %s\n", signal.name);
		printf("X : %d\n", signal.position.X);
		printf("Y : %d\n", signal.position.Y);
		printf("UUID : %d\n", signal.uuid[0]);
		printf("RSSI : %d\n", signal.rssi);
		/*if(signal.uuid[0] == BEACONS_UUID){
			beaconsSignal[index_signal] = signal;
			index_signal ++;
		}
		index_channel ++;*/
	//}*/
	/*int k;
	for(k = 0; k< sizeof(beaconsSignal); k ++){
		printf("Index n° : %d\n", k);
		printf("Name : %s\n", beaconsSignal[k].name);
		printf("Name : %d\n", beaconsSignal[k].position.X);
		printf("Name : %d\n", beaconsSignal[k].position.Y);
		printf("Name : %d\n", beaconsSignal[k].uuid[0]);
		printf("Name : %d\n", beaconsSignal[k].rssi);
	}*/
	
}

/**

 * @fn static void Receiver_getAllBeaconsChannel()

 * @brief methode privee permettant de memoriser dans beaconsChannel l'ensemble des trames des balises

 */

static void Receiver_getAllBeaconsChannel(){
    int ret, status;

	// Get HCI device.

	const int device = hci_open_dev(hci_get_route(NULL));
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

	printf("Scanning...\n");

	uint8_t buf[HCI_MAX_EVENT_SIZE];
	evt_le_meta_event * meta_event;
	BeaconsChannel * info;
	BeaconsSignal bs;
	int uuid[2];
	int len;

	while(1){ //Thread pour la suite
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

					memcpy(uuid, info->data + 21, 2); //cond pour le Name

					if(uuid[0] == BEACONS_UUID){
						beaconsChannel[0] = info;
						index_channel ++;
					}

					beaconsChannel[index_channel] = info;
					
					Receiver_translateChannelToBeaconsSignal();

					//beaconsChannel[0] = info;
					
					/*if(index_channel == 100){
						Receiver_translateChannelToBeaconsSignal();
					}*/

					//Receiver_translateChannelToBeaconsSignal();

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

/**

 * @fn static void Receiver_performAction()

 * @brief methode privee permettant de definir les actions a effectuer

 * @param TODO

 */

static void Receiver_performAction(Action_RECEIVER action){
    switch (action) {
        case A_STOP:
            Receiver_free();
            break;

        case A_SEND_BEACONS_SIGNAL:
            //Scanner_setAllBeaconsSignal();
            //Thread getChannel()         
            break;

        case A_MAJ_BEACONS_CHANNELS:
            Receiver_getAllBeaconsChannel();
            //Timer TMAJ
            //Timer TScan
            break;

        default:
            break;
    }

}

/**

 * @fn static void Receiver_run() TODO

 */

static void Receiver_run(Event_RECEIVER event){

    Action_RECEIVER action = stateMachine[myState][event].action;
	myState =  stateMachine[myState][event].destinationState;
    Receiver_performAction(action);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions extern

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


extern void Receiver_new(){
    myState = S_DEATH;
}

extern void Receiver_ask4StartReceiver(){
    myState = S_SCANNING;
    Event_RECEIVER myEvent = E_ASK_BEACONS_SIGNAL;
    Receiver_run(myEvent);
}

extern void Receiver_ask4StopReceiver(){
    Event_RECEIVER myEvent = E_STOP;
    Receiver_run(myEvent);
}

extern void Receiver_ask4BeaconsSignal(){
    Event_RECEIVER myEvent = E_ASK_BEACONS_SIGNAL;
    Receiver_run(myEvent);
}

extern void Receiver_free(){
    myState = S_DEATH;
}

/*int main(){
    Receiver_getAllBeaconsChannel();
    //sleep(10);
    Receiver_translateChannelToBeaconsSignal(beaconsChannel);
    //Receiver_ask4BeaconsSignal();
    return 0;
}*/