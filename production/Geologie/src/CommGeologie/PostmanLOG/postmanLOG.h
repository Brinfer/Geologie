/**
 * @file postmanLOG.h
 *
 * @brief Gere le socket de communication.
 *
 * Joue le role de serveur dans cette communication et ecoute le socket pour etablir une
 * connexion ou recevoir une communication.
 * PostmanLOG ne sait pas decrypter les messages qu’il envoie ou qu’il reçoit.
 * Il s’occupe uniquement de donner les messages qu’il reçoit a DispatcherLOG ainsi que
 * d’envoyer les messages fournis par ProxyGUI et ProxyLoggerMOB.
 *
 * @version 2.0
 * @date 03-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#ifndef POSTMAN_LOG_
#define POSTMAN_LOG_

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "../com_common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialise le sockect.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t PostmanLOG_new(void);

/**
 * @brief Ouvre le socket.
 *
 * Reste en attente de connexion si aucune n'est etablie, et permet
 * de lire par la suite des messages.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t PostmanLOG_start(void);

/**
 * @brief Envoie un message.
 *
 * @param trame La trame a envoyer.
 * @param size La taille de la trame a envoyer.
 * @return int8_t 0 en cas de succes, une autre valeur sinon.
 */
extern int8_t PostmanLOG_sendMsg(Trame trame, uint8_t size);

/**
 * @brief Lie sur le socket le nombre d'octet indique.
 *
 * @param destTrame La trame ou ecrire le message lue.
 * @param nbToRead Le nombre d'octet a lire.
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 *
 * @warning Fonction bloquante.
 */
extern int8_t PostmanLOG_readMsg(Trame destTrame, uint8_t nbToRead);

/**
 * @brief Fermeture du socket.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t PostmanLOG_stop(void);

/**
 * @brief Ferme le socket.
 *
 * @return int8_t -1 en cas d'erreur, 0 sinon.
 */
extern int8_t PostmanLOG_free(void);

#endif // POSTMAN_LOG_
