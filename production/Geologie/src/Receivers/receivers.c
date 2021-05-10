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

inquiry_info *ii = NULL;
int max_rsp, num_rsp;
int dev_id, sock, len, flags;
int i;
char addresse[19] = { 0 };
char name[248] = { 0 };
int8_t rssi;
uint16_t handle;
uint16_t ptype = HCI_DM1 | HCI_DM3 | HCI_DM5 | HCI_DH1 | HCI_DH3 | HCI_DH5;
int co;
int rs;
//hciStatus_t rs2;
int debug = 0;

int btconnect(char *dest)
{
    int rfsock = -1;
    struct sockaddr_rc addr = { 0 };
    int status;

    // allocate a socket
    rfsock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba( dest, &addr.rc_bdaddr );

    // connect to server
    if( debug ) printf("Connecting to %s ...\n", dest);
    status = connect(rfsock, (struct sockaddr *)&addr, sizeof(addr));


    if( status < 0 ) {
        perror("Error connecting");
        return -1;
    }
    else {
        if( debug ) printf("Connection established, rfsock = %d\n", rfsock);
        return rfsock;
    }
}

void btscan(){
	printf("On demarre le scan des peripheriques BLE.\n");
    dev_id = hci_get_route(NULL);
    sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }
    len  = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if( num_rsp < 0 ) perror("hci_inquiry");

    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii+i)->bdaddr, addresse);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0){
        	strcpy(name, "[unknown]");
        }
        //co = hci_create_connection(sock, &(ii+i)->bdaddr, ACL_PTYPE_MASK, 0, 0x01, &handle, 25000);
        //printf("connexion : %d\n", co);
        //rs = hci_read_rssi(sock, handle, &rssi, 1000);
        //rs2 = HCI_ReadRssiCmd(handle);
        //printf("rssi : %d\n", rs);
        btconnect(addresse);
        printf("%s  %s %d\n", addresse, name, rssi);
    }

    free( ii );
    close( sock );
}

int main(int argc, char **argv)
{
	btscan();
    return 0;
}
