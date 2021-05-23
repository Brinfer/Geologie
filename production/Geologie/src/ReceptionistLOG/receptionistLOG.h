#ifndef MATHEMATICIAN_H
#define MATHEMATICIAN_H
/**
 * @file receptionistLOG.h
 *
 * @brief va permettre de se connecter au client (disponible sur android) et recevoir/lui envoyer de l'information
 *
 * @version 1.0.1
 * @date 5 mai 2021
 * @author BRIENT Nathan
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @typedef type Data
 * @brief type representant la chaine qui sera envoyee
 */
typedef char Data;                         //


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn int ReceptionistLOG_new()
 * @brief fonction qui creer le socket
 *
 */
extern int ReceptionistLOG_new(void);


/**
 * @fn extern int ReceptionistLOG_start()
 * @brief fonction principale de receptionistLOG.c qui sera appelee de l'exterieur, elle créera notamment le thread de lecture
 *
 */
extern int ReceptionistLOG_start(void);

/**
 * @fn extern int ReceptionistLOG_readMsg()
 * @brief fonction extern qui modifiera dataToModify avec l'information recue sur le socket
 *
 * @param dataToModify prend l'adresse du tableau à modifier
 */
extern int ReceptionistLOG_readMsg(Data* dataToModify);

/**
 * @fn extern int ReceptionistLOG_sendMsg()
 * @brief fonction extern qui enverra la donnee dataToSend au client
 *
 * @param dataToSend donnee a envoyer (tableau)
 * @param taille taille du tableau a envoyer
 */
extern int ReceptionistLOG_sendMsg(Data dataToSend[], int taille);
#endif
