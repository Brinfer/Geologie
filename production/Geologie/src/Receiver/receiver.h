/**
 * @file receiver.h
 *
 * @brief TODO
 *
 * @version 1.0.1
 * @date 5 mai 2021
 * @author LECENNE Gabriel
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include "../common.h"

#ifndef RECEIVERS_H_
#define RECEIVERS_H_

typedef le_advertising_info BeaconsChannel;


/**
 * @fn extern void Receiver_new()
 * @brief Instancie et initialise l'objet Receiver
*/

extern void Receiver_new();

/**
 * @fn extern void Receiver_free()
 * @brief Libere et supprime l'objet Receiver
*/

extern void Receiver_free();

/**
 * @fn extern int8_t Receiver_ask4StartScanner()
 * @brief Demande le démarrage de Receiver
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
*/

extern int8_t Receiver_ask4StartReceiver();

/**
 * @fn extern int8_t Receiver_ask4StopScanner()
 * @brief Demande l'arret de Receiver
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
*/

extern int8_t Receiver_ask4StopReceiver();

/**
 * @fn extern int8_t Receiver_ask4BeaconsSignal()
 * @brief Demande le démarrage du scan
 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode
*/

extern int8_t Receiver_ask4BeaconsSignal();


#endif /* RECEIVERS_H_ */
