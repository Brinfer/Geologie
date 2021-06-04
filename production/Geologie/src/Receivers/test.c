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

#include "receivers.h"
#include "translator.h"
#include "../translatorBeacon/translatorBeacon.h"

#define UUID_FIRST_BYTE 21
#define UUID_LAST_BYTE 22

#define LOCAL_NAME_INDEX_LENGTH 4
#define LOCAL_NAME_LENGTH 14
#define LOCAL_NAME_FIRST_BYTE 5
#define LOCAL_NAME_LAST_BYTE 18

#define BEACONS_UUID 0x1A18

BeaconsSignal beaconsData;

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

void getNameAndRSSI(le_advertising_info * info){
	char name[100];
	char name_length;
	name_length = info->data[LOCAL_NAME_INDEX_LENGTH];
	int verif[2];
	int i;
	int j;
	printf("Device_Name : ");
	for(i = 5; i < 7; i++){
		if(i == 6){
			printf("%c\n", (char) info->data[i]);
		}
		else{
			printf("%c", (char) info->data[i]);
		}			
	}
	printf("Pos_X : ");
	for(i = 8; i < 13; i++){
		if(i == 12){
			printf("%c\n", (char) info->data[i]);
		}
		else{
			printf("%c", (char) info->data[i]);
		}			
	}
	printf("Pos_Y : ");
	for(i = 14; i < 19; i++){
		if(i == 18){
			printf("%c\n", (char) info->data[i]);
		}
		else{
			printf("%c", (char) info->data[i]);
		}			
	}
	beaconsData.rssi = (int8_t)info->data[info->length];
	printf("RSSI : %d\n\n", beaconsData.rssi);
}

static int convertBytesToInt(const unsigned char* src) {

    int returnVal;

    unsigned char tempArray[8];

    memcpy(tempArray, src, 8);

    returnVal = *(int*) tempArray;

    returnVal = ntohl(returnVal);

    return returnVal;

}

void checkUUID(le_advertising_info * info){
	int uuid[2] = {0};
	int i;
	for(i = UUID_FIRST_BYTE; i < UUID_LAST_BYTE+1; i++){
		if(i == UUID_LAST_BYTE){
			//printf("%d\n", info->data[i]);
			uuid[1] = (char)info->data[i];
		}
		else{
			//printf("%d", info->data[i]);
			uuid[0] = (char)info->data[i];
		}		
	}
	if(uuid[0] == 26 && uuid[1]==24){
		translate(info);
		//getNameAndRSSI(info);
	}
}

int main()
{
	int ret, status;

	// Get HCI device.

	const int device = hci_open_dev(hci_get_route(NULL));
	if ( device < 0 ) { 
		perror("Failed to open HCI device.");
		return 0; 
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
		return 0;
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
		return 0;
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
		return 0;
	}

	// Get Results.

	struct hci_filter nf;
	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);
	if ( setsockopt(device, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0 ) {
		hci_close_dev(device);
		perror("Could not set socket options\n");
		return 0;
	}

	printf("Scanning....\n");

	uint8_t buf[HCI_MAX_EVENT_SIZE];
	evt_le_meta_event * meta_event;
	le_advertising_info * info;
	int len;

	while ( 1 ) {
		len = read(device, buf, sizeof(buf));
		if ( len >= HCI_EVENT_HDR_SIZE ) {
			meta_event = (evt_le_meta_event*)(buf+HCI_EVENT_HDR_SIZE+1);
			if ( meta_event->subevent == EVT_LE_ADVERTISING_REPORT ) {
				uint8_t reports_count = meta_event->data[0];
				void * offset = meta_event->data + 1;
				while ( reports_count-- ) {
					info = (le_advertising_info *)offset;
					char addr[18];
					if ( ret < 0 ) {
						hci_close_dev(device);
						perror("Failed to enable scan.");
						return 0;
					}
					ba2str(&(info->bdaddr), addr);
					checkUUID(info);			
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
		return 0;
	}

	hci_close_dev(device);
	
	return 0;
}