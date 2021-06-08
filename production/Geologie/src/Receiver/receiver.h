/**
 * @file receivers.h
 *
 * @brief TODO
 *
 * @version 1.0.1
 * @date 5 mai 2021
 * @author LECENNE Gabriel
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

//#include <stdlib.h>
//#include <errno.h>
//#include <curses.h>
//#include <unistd.h>
//#include <sys/ioctl.h>
//#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
//#include <bluetooth/hci_lib.h>
//#include <inttypes.h>
#include "../common.h"

#ifndef RECEIVERS_H_
#define RECEIVERS_H_

#define RAW_DATA_FLAGS_LENGTH_INDEX
#define RAW_DATA_NAME_LENGTH_INDEX
#define RAW_DATA_UUID_LENGTH_INDEX
#define RAW_DATA_FLAGS_VALUE_INDEX
#define RAW_DATA_NAME_VALUE_INDEX
#define RAW_DATA_UUID_VALUE_INDEX

typedef le_advertising_info BeaconsChannel;

extern void Receiver_new();

extern void Receiver_ask4StartReceiver();

extern void Receiver_ask4BeaconsSignal();

extern void Receiver_ask4StopReceiver();

extern void Receiver_free();


#endif /* RECEIVERS_H_ */
