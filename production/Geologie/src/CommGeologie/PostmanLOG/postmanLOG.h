/**
 * @file postmanLOG.h
 *
 * @brief Gére le socket de communication.
 *
 * Joue le rôle de serveur dans cette communication et ecoute  le socket pour etablir une
 * connexion ou recevoir une communication.
 * #PostmanLOG ne sait pas décrypter les messages qu’il envoie ou qu’il reçoit.
 * Il s’occupe uniquement de donner les messages qu’il reçoit a DispatcherLOG ainsi que
 * d’envoyer les messages fournis par ProxyGUI et ProxyLoggerMOB
 *
 * @version 2.0
 * @date 03-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#ifndef POSTMAN_LOG_
#define POSTMAN_LOG_

/**
 * @brief Initialise le sockect
 *
 * @return int 0 en cas de succes, une autre valeur sinon
 */
extern int PostmanLOG_new(void);

/**
 * @brief Ouvre le socket
 *
 * Reste en attente de connexion si aucune n'est etablie, et permet
 * de lire par la suite des messages.
 *
 * @return int 0 en cas de succes, une autre valeur sinon
 */
extern int PostmanLOG_start(void);

/**
 * @brief Envoie un message
 *
 * @param trame La trame a envoyer
 * @param size La taille de la trame a envoyer
 * @return int 0 en cas de succes, une autre valeur sinon
 */
extern int PostmanLOG_sendMsg(const unsigned char* trame, short size);

/**
 * @brief Lie les message sur le socket
 *
 * @param destTrame La trame ou ecrire le message lue
 * @return int 0 en cas de succes, une autre valeur sinon
 */
extern int PostmanLOG_readMsg(unsigned char* destTrame);

/**
 * @brief Fermeture du socket
 *
 * @return int 0 en cas de succes, une autre valeur sinon
 */
extern int PostmanLOG_stop(void);

/**
 * @brief Ferme le socket
 *
 * @return int 0 en cas de succes, une autre valeur sinon
 */
extern int PostmanLOG_free(void);

#endif // POSTMAN_LOG_
