/**
 * @file client.c
 *
 * @brief 
 * @version 1.0.1
 * @date 5 mai 2021
 * @author BRIENT Nathan
 * @copyright BSD 2-clauses
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "commun.h"
#include "client.h"

#define PORT_DU_SERVEUR (12345)

/* \struct Client_t
 * \brief Structure de client comportant un socket de type int et une sockaddr_in de type struct
 */
struct Client_t{
	int socket;
	struct sockaddr_in adresse_du_serveur;
};

static int Client_run(Client * client) {
    Data data;
	printf("je suis dans le run");
	for(int i=0;i<10;i++){
		/*char message[]="salut cest nath";
		for(int i=0;i<=16;i++){
			data.msg[i]=message[i];
		}
		*/
		data.msg[i]=i;
		data.msg[i+1]='\0';
		printf("%i",i);
		//sprintf (data.msg[i]‚ "%d"‚i);
		Client_sendMsg(client,data);
		sleep(1);
	}
	Data dataStop;
	char stopChar[]="stop";
	for(int i=0;i<=16;i++){
		dataStop.msg[i]=stopChar[i];
	}
	Client_sendMsg(client,dataStop);

    return 0;
}

int main(int argc, char* argv[]) {
	printf("Bienvenue dans le client\n");
	Client * client = Client_new();
	Client_start(client);
	Client_stop(client);
	Client_free(client);
	printf("Au revoir\n");
	return 0;
}

/* \methode Client_new
 * \brief  Création d'un pointeur sur un client et allocation mémoire
 * \param void
 * \return Client *
 */
extern Client * Client_new(){
	printf("Je suis dans client new\n");

	Client * this;

	this = calloc(1,sizeof(Client));

	return this;
}

/* \methode Client_start
 * \brief  Démarrage du client dont le pointeur est placé en paramètre
 * \param Client *
 * \return void
 */
extern void Client_start(Client * client){
	printf("je suis dans client start\n");

	/* on choisit un socket TCP (SOCK_STREAM) sur IP (AF_INET) */
	client->socket = socket (AF_INET, SOCK_STREAM, 0);
	/* AF_INET = famille TCP/IP */
	client->adresse_du_serveur.sin_family = AF_INET;
	/* port du serveur auquel se connecter */
	client->adresse_du_serveur.sin_port = htons (PORT_DU_SERVEUR);
	/* adresse IP (ou nom de domaine) du serveur auquel se connecter */
	client->adresse_du_serveur.sin_addr = *((struct in_addr *)gethostbyname ("localhost")->h_addr_list[0]); //ladresse du serveur
	/* On demande la connexion auprès du serveur */
	connect(client->socket, (struct sockaddr *)&(client->adresse_du_serveur), sizeof (client->adresse_du_serveur));
	/*
	data data;
	data.demande=MVT;
	data.direction=LEFT;
	Client_sendMsg(client, data);

	printf("J'ai fini\n");
	*/
	Client_run(client);
}

/* \methode Client_stop
 * \brief  Arret du client dont le pointeur est placé en paramètre
 * \param Client *
 * \return void
 */
extern void Client_stop(Client * client){
	printf("je suis dans client stop\n");

	close (client->socket);
}

/* \methode Client_free
 * \brief  Libération mémoire du client dont le pointeur est en paramètre
 * \param Client *
 * \return void
 */
extern void Client_free(Client * client){
	printf("je suis dans client free\n");
	free(client);
}

/* \methode Client_sendMsg
 * \brief Envoi de données
 * \param Client *
 * \param Data
 * \return void
 */
extern void Client_sendMsg(Client * client, Data data){
	write(client->socket, &data, sizeof (data));
}

/* \methode Client_read_Msg
 * \brief  Reception de données
 * \param Client *
 * \param PilotState *
 * \return void
 */
extern void Client_readMsg(Client * client, Data * data){
	read(client->socket, data, sizeof (Data));
}


