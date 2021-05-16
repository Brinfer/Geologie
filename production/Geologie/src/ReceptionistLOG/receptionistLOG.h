/**
 * @file receptionistLOG.h
 *
 * @brief .h de serveurWifi.c
 *
 * @version 1.0.1
 * @date 5 mai 2021
 * @author BRIENT Nathan
 * @copyright BSD 2-clauses
 *
 */

typedef char Data;                         //TODO changer nom structure


/**
 * @fn int ReceptionistLOG_new()
 * @brief fonction qui crer le socket
 *
 */
extern int ReceptionistLOG_new(void);


/**
 * @fn extern int ReceptionistLOG_start(int argc, char* argv[])
 * @brief fonction principale de receptionistLOG.c qui sera appelee de l'exterieur
 *
 */
extern int ReceptionistLOG_start(void);

/**
 * @fn extern int ReceptionistLOG_readMsg()
 * @brief fonction qui sera appelee de l'exterieur et modifiera dataToModify
 *
 * @param dataToModify prend l'adresse du tableau à modifier
 */
extern int ReceptionistLOG_readMsg();

/**
 * @fn extern int ReceptionistLOG_sendMsg()
 * @brief fonctionqui sera appelee de l'exterieur et qui enverra la donnee dataToSend
 *
 * @param dataToSend donnee a envoyer (tableau)
 * @param taille taille du tableau a envoyer
 */
extern int ReceptionistLOG_sendMsg(const Data* dataToSend, int taille);
