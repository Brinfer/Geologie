/**
 * @file server.h
 *
 * @brief 
 * @version 1.0.1
 * @date 5 mai 2021
 * @author BRIENT Nathan
 * @copyright BSD 2-clauses
 *
 */

#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

/* \struct Server_t
 * \brief  Structure de server définie dans server.c
 */
typedef struct Server_t Server;


extern Server * Server_new();
extern void Server_free(Server * this);
extern void Server_start(Server * this);
extern void Server_stop(Server * this);


#endif /* SRC_SERVER_H_ */
