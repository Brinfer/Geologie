/**
 * @file client.h
 *
 * @brief 
 * @version 1.0.1
 * @date 5 mai 2021
 * @author BRIENT Nathan
 * @copyright BSD 2-clauses
 *
 */

#ifndef SRC_TELCO_CLIENT_H_
#define SRC_TELCO_CLIENT_H_

/* \struct Client_t
 * \brief  Structure de client définie dans client.c
 */
typedef struct Client_t Client;

extern Client * Client_new();
extern void Client_free(Client * client);
extern void Client_start(Client * client);
extern void Client_stop(Client * client);

extern void Client_sendMsg(Client * client,Data payload);
extern void Client_readMsg(Client * client, Data * data);


#endif /* SRC_TELCO_CLIENT_H_ */
