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

	bs.uuid[0] = (uint32_t) info->data[DEVICE_UUID_FIRST_BYTE];
	bs.uuid[1] = (uint32_t) info->data[DEVICE_UUID_FIRST_BYTE + 1];

	memcpy(posX, info->data + DEVICE_POSITION_X_FIRST_BYTE, POSITION_LENGTH);

	sscanf(posX, "%d", (int32_t*) &(bs.position.X));

	memcpy(posY, info->data + DEVICE_POSITION_Y_FIRST_BYTE, POSITION_LENGTH);

	sscanf(posY, "%d", (int32_t*) &(bs.position.Y));

	bs.rssi = (int8_t) info->data[info->length - 1];

    return bs;

}
