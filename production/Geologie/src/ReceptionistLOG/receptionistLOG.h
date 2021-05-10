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

/**
 * @fn int ReceptionistLOG_new()
 * @brief fonction qui créer le socket
 *  
 */
extern int ReceptionistLOG_new(void);


/**
 * @fn extern int ReceptionistLOG_start(int argc, char* argv[])
 * @brief fonction principale de receptionistLOG.c qui sera appelee de l'exterieur
 *
 */
extern int ReceptionistLOG_start(void);

