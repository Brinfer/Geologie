/**
 * @file receivers.c
 *
 * @brief Permet d'obtenir le RSSI des balises.
 *
 * @version 1.0.1
 * @date 5 mai 2021
 * @author LECENNE Gabriel
 * @copyright BSD 2-clauses
 *
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <pthread.h>

#include "../tools.h"   // TODO
#include "receivers.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inquiry_info* ii = NULL;
static int max_rsp, num_rsp;
static int dev_id, sock, len, flags;
static int i;
static char addresse[19] = { 0 };
static char name[248] = { 0 };
static int8_t rssi;
static uint16_t handle;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* btscan(void *arg);
static int btconnect(char* dest);
static int btrssi(char* dest);

static pthread_t pthreadScan;

/**
 * @fn static int btrssi(char *dest) {
 * @brief methode qui permet d'obtenir le RSSI
 *
 * @param dest correspond à l'adresse du peripherique dont on veut le RSSI
 */

static int btrssi(char* dest) {
    int dev_id = -1;
    struct hci_conn_info_req* cr;
    bdaddr_t bdaddr;
    int8_t rssi;
    int dd;

    str2ba(dest, &bdaddr);

    if (dev_id < 0) {
        dev_id = hci_for_each_dev(HCI_UP, (void*) btscan, (long) &bdaddr);
        if (dev_id < 0) {
            PRINT("Not connected.\n");
            exit(1);
        }
    }

    dd = hci_open_dev(dev_id);
    if (dd < 0) {
        perror("HCI device open failed");
        exit(1);
    }

    cr = malloc(sizeof(*cr) + sizeof(struct hci_conn_info));
    if (!cr) {
        perror("Can't allocate memory");
        exit(1);
    }

    bacpy(&cr->bdaddr, &bdaddr);
    cr->type = ACL_LINK;
    if (ioctl(dd, HCIGETCONNINFO, (unsigned long) cr) < 0) {
        perror("Get connection info failed");
        exit(1);
    }

    if (hci_read_rssi(dd, htobs(cr->conn_info->handle), &rssi, 1000) < 0) {
        perror("Read RSSI failed");
        exit(1);
    }

    PRINT("RSSI return value: %d\n", rssi);

    free(cr);

    hci_close_dev(dd);

    return 0;
}

/**
 * @fn static void btconnect(char *dest) {
 * @brief methode qui permet de se connecter à un peripherique
 *
 * @param dest correspond à l'adresse du peripherique dont on veut le RSSI
 */

static int btconnect(char* dest) {
    int rfsock = -1;
    struct sockaddr_rc addr = { 0 };
    int status;

    // alloue une socket
    rfsock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    PRINT("socket %d\n", rfsock);

    // defini les paramrtres de connexion
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba(dest, &addr.rc_bdaddr);
    PRINT("adresse %s\n", dest);

    // connexion au seveur
    PRINT("Connecting to %s ...\n", dest);
    status = connect(rfsock, (struct sockaddr*) &addr, sizeof(addr));
    PRINT("connexion %d\n", status);


    if (status < 0) {
        perror("Error connecting");
        return -1;
    } else {
        PRINT("Connection established, rfsock = %d\n", rfsock);
        return rfsock;
    }
}

/**
 * @fn static void btscan() {
 * @brief methode qui permet de scanner les peripheriques disponibles
 *
 */

static void* btscan(void *arg) {
    PRINT("On demarre le scan des peripheriques BLE.\n");
    dev_id = hci_get_route(NULL);
    sock = hci_open_dev(dev_id);
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }
    len = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*) malloc(max_rsp * sizeof(inquiry_info));

    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if (num_rsp < 0) perror("hci_inquiry");

    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii + i)->bdaddr, addresse);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii + i)->bdaddr, sizeof(name), name, 0) < 0) {
            strcpy(name, "[unknown]");
        }
        PRINT("%s  %s %d\n", addresse, name, rssi);
        btconnect(addresse);
        btrssi(addresse);
    }

    free(ii);
    close(sock);

    return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


extern void Receivers_start(Receivers_t* this) {
    pthread_create(&pthreadScan, NULL, btscan, NULL);
}

extern void Receivers_stop(Receivers_t* this) {
    pthread_join(pthreadScan, NULL);
}
