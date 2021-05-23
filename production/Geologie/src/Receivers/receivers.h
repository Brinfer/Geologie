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

#ifndef RECEIVERS_H_
#define RECEIVERS_H_

#define RAW_DATA_FLAGS_LENGTH_INDEX
#define RAW_DATA_NAME_LENGTH_INDEX
#define RAW_DATA_UUID_LENGTH_INDEX
#define RAW_DATA_FLAGS_VALUE_INDEX
#define RAW_DATA_NAME_VALUE_INDEX
#define RAW_DATA_UUID_VALUE_INDEX

/* \struct Receivers_t
 * \brief  Structure de receivers définie dans receivers.c
 */
typedef struct {

} Receivers_t;

typedef struct {
    char name[8];
    int8_t rssi;
} BeaconsData;

typedef struct {
    unsigned char* flags_length;
    unsigned char* name_length;
    unsigned char* uuid_length;
    unsigned char* flags_value;
    unsigned char* name_value;
    unsigned char* uuid_value;
} RawData;

extern void Receivers_getAllBeaconsData();

extern Receivers_t * Receivers_new();
extern void Receivers_free(Receivers_t * this);
extern void Receivers_start(Receivers_t * this);
extern void Receivers_stop(Receivers_t * this);


#endif /* RECEIVERS_H_ */
