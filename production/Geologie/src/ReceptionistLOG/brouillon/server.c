/**
 * @file server.c
 *
 * @brief Permet d'envoyer des informations à travers les sockets de manière continue.
 *
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
#include "server.h"
//#include "pilot.h"

#define MAX_PENDING_CONNECTIONS (2)
#define PORT_DU_SERVEUR 12345

static void Server_run(Server * this);
void communication_avec_client(Server * this,int socket);

/* \struct Server_t
 * \brief  Structure de server comportant une socket_ecoute de type int, une socket_donnees de type int, une sockaddr_in de type struct et un pointeur sur un pilot
 */
struct Server_t{
	int socket_ecoute; //~salle attente
	int socket_donnees; //donnes
	//int age_capitaine_courant = 25;
	struct sockaddr_in mon_adresse;
};

/* \methode Server_new
 * \brief  Création d'un pointeur sur un server, allocation de l'espace mémoire et initialisation de socket_ecoute, des addresse et du pilot.
 * \param void
 * \return Server *
 */

int main(int argc, char* argv[]) {
	printf("Bienvenue dans le serveur\n");
	Server * server = Server_new();
	Server_start(server);
	Server_stop(server);
	Server_free(server);
	printf("Au revoir\n");
	return 0;
}

extern Server * Server_new(){
	Server * this;
	printf("création socket");
	this=calloc(1,sizeof(Server));
	/* Creation du socket : AF_INET = IP, SOCK_STREAM = TCP */
	this->socket_ecoute = socket (AF_INET, SOCK_STREAM, 0);
	this->mon_adresse.sin_family = AF_INET; /* Type d'adresse = IP */
	this->mon_adresse.sin_port = htons (PORT_DU_SERVEUR); /* Port TCP ou le service est accessible */
	this->mon_adresse.sin_addr.s_addr = htonl (INADDR_ANY); /* On s'attache a toutes les interfaces */
	return this;
}

/* \methode Server_free
 * \brief  Liberation mémoire du server dont le pointeur est placé en paramètre
 * \param Server *
 * \return void
 */
extern void Server_free(Server * this){
	printf("on free la socket");
	free(this);
}

/* \methode Server_start
 * \brief  Démarrage du server les méthodes bind et listen, démarrage du pilot et appel de la fonction Server_run
 * \param Server *
 * \return void
 */
extern void Server_start(Server * this){
	printf("server start");

	/* On attache le socket a l'adresse indiquee */
	bind (this->socket_ecoute, (struct sockaddr *)&(this->mon_adresse), sizeof (this->mon_adresse));
	/* Mise en ecoute du socket */
	listen (this->socket_ecoute, MAX_PENDING_CONNECTIONS);
	Server_run(this);
}

/* \methode Server_stop
 * \brief  Arret du server avec la fermetture du port d'ecoute
 * \param Server *
 * \return void
 */
extern void Server_stop(Server * this){
	/* On ferme le port sur lequel on ecoutait */
	printf("on ferme la socket");
	close (this->socket_ecoute);
}

/* \methode Server_sendMsg
 * \brief  Non utilisée
 * \return void
 */
static void Server_sendMsg(){

}

/* \methode Server_readMsg
 * \brief  Non utilisée
 * \return void
 */
static void Server_readMsg(){

}

/* \methode Server_run
 * \brief  Gestion de la communication avec la telecommande
 * \param Server *
 * \return void
 */
static void Server_run(Server * this){
	printf("server run");

	while(1){
		this->socket_donnees = accept (this->socket_ecoute, NULL, 0); //qd une connexion est acceptée on créer un socket pr communiquer
		printf("Connexion acceptée");
		if (fork () == 0){
			communication_avec_client(this,this->socket_donnees);
			printf("on est apres la comm avec client");
		}
		else
		{

		}
	}
}

/* \methode communication_avec_client
 * \brief  Communication avec la telecommande et création du data
 * \param Server *
 * \param int
 * \return void
 */
void communication_avec_client(Server * this,int socket)
{//socket = socket_donnees
	printf("communication_avec_client");

	Data data;
	Data myData;

	char quit = 0;
	char log = 0;
	while(quit==0){
		read (socket, &data, sizeof (data));
		
		char stopMsg[]="stop";
		if(strcmp(stopMsg,data.msg) == 0){
			printf("je suis iciiiiii");
			sleep(1);
			quit=1;
		}
		printf("Le client à envoyé le message %s \n",data.msg);

		//write(socket,&pilotState,sizeof(pilotState));
	}
	printf("Le socket se ferme\n");
	/*close (socket);

	exit (0);*/
}

