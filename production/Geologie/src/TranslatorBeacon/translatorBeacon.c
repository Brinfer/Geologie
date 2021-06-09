/**

 * @file translatorBeacon.c

 *

 * @version 1.0
 * @date 4 juin 2021
 * @author LECENNE Gabriel
 * @copyright Geo-Boot
 * @license BSD 2-clauses

 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Include

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "translatorBeacon.h"
#include "../Receiver/receiver.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Define

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define POSITION_LENGTH 5
#define DEVICE_NAME_FIRST_BYTE 5
#define DEVICE_NAME_LENGTH 2
#define DEVICE_POSITION_X_FIRST_BYTE 8
#define DEVICE_POSITION_Y_FIRST_BYTE 14
#define DEVICE_UUID_FIRST_BYTE 21
#define DEVICE_UUID_LENGTH 2


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions publiques

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int TranslatorBeacon_new() {
    return 0;
}

extern int TranslatorBeacon_free() {
    return 0;
}


extern BeaconSignal TranslatorBeacon_translateChannelToBeaconsSignal(BeaconsChannel * info) {

    BeaconSignal bs;

    char posX[POSITION_LENGTH];
	char posY[POSITION_LENGTH];

	memcpy(bs.name, info->data + DEVICE_NAME_FIRST_BYTE, DEVICE_NAME_LENGTH);
	bs.name[2] =  '\0';

	bs.uuid[0] = (int32_t) info->data[DEVICE_UUID_FIRST_BYTE];
	bs.uuid[1] = (int32_t) info->data[DEVICE_UUID_FIRST_BYTE + 1];

	memcpy(posX, info->data + DEVICE_POSITION_X_FIRST_BYTE, POSITION_LENGTH);

	sscanf(posX, "%d", &(bs.position.X));

	memcpy(posY, info->data + DEVICE_POSITION_Y_FIRST_BYTE, POSITION_LENGTH);

	sscanf(posY, "%d", &(bs.position.Y));

	bs.rssi = (int8_t) info->data[info->length];

    return bs;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions static

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*int main(){

	BeaconSignal bs;

	uint8_t * bc;
	
	uint8_t inputData[27] = {
            0x02, 0x01, 0x06, 0x0F, 0x09,              // Not used
            0x62, 0x31, 0x78, 0x33, 0x30, 0x30, 0x30, 0x30, 0x79, 0x34, 0x30, 0x30, 0x30, 0x30,
			0x03, 0x03,					// Not used
            0x18, 0x1A,                  // UUID, default value of the UUID
			0x03, 0x19, 0x00, 0x03
        };
    uint8_t powerInput = 1;

	bc = inputData;

	bs = TranslatorBeacon_translateChannelToBeaconsSignal(bc);

	//uint8_t name[2] = {'b', '1'};
	uint8_t name[3] = {98, 49, '\0'};

	int32_t uuid[2] = { 24, 26 };

	if(bs.name[0] == name[0]){
		printf("Name OK !\n");
	}
	else{
		printf("Name FALSE !\n");
		printf("%d%d\n", bs.name[0], bs.name[1]);
	}

	if(bs.uuid[0] == uuid[0]){
		printf("UUID OK !\n");
	}
	else{
		printf("UUID FALSE !\n");
		printf("%d%d", bs.uuid[0], bs.uuid[1]);
	}

	if(bs.position.X == 30000){
		printf("X OK !\n");
	}

	if(bs.position.Y == 40000){
		printf("Y OK !\n");
	}
	else{
		printf("Y FALSE !\n");
		printf("%d", bs.position.Y);
	}

}*/