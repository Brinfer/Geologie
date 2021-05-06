/**
 * @file commun.h
 *
 * @brief fichier commun au client et serveur
 * @version 1.0.1
 * @date 5 mai 2021
 * @author BRIENT Nathan
 * @copyright BSD 2-clauses
 *
 */
#ifndef COMMUN_H_
#define COMMUN_H_

/* \struct Data
 * \brief Structure qui correspond à un paquet envoyé au serveur comportant une demande et une direction
 */
typedef struct {
	char msg[16];
}Data;

#endif 
