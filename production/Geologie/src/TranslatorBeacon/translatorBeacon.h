/**

 * @file translatorBeacon.h

 * 

 * @brief TODO

 *

 * Vous pouvez ici mettre un descriptif un peu plus long de ce que propose

 * le module (fichiers .h + .c). C'est là que vous expliquez les modalités

 * générales d'utilisation (préconditions, contexte d'utilisation, avertissements,

 * ...). TODO

 * 

 * @version 2.0

 * @date 03-06-2021

 * @author Gabriel LECENNE

 * @copyright Geo-Boot

 * @license BSD 2-clauses

 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Define 

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** 

 * @def TRANSLATORBEACON_H

 */

#ifndef TRANSLATORBEACON_H

#define TRANSLATORBEACON_H 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Include 

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Receiver/receiver.h"
//#include "../common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Variable et structure extern

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions publiques

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**

 * @fn extern int TranslatorBeacon_new()

 * @brief Instancie et initialise l'objet translatorBeacon

 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode

*/

extern int TranslatorBeacon_new();

/**

 * @fn extern int TranslatorBeacon_free()

 * @brief Libere et supprime l'objet translatorBeacon

 * @return retourne 1 s'il y a une erreur dans l'execution de la méthode

*/

extern int TranslatorBeacon_free();

/**

 * @fn extern BeaconsSignal TranslatorBeacon_translateChannelToBeaconsSignal(BleChannel)

 * @brief Demande la traduction de la trame passee en parametre

 * @return retourne une structure de BeaconsSignal

*/

extern BeaconSignal TranslatorBeacon_translateChannelToBeaconsSignal(uint8_t * beaconsChannel);


#endif /* TRANSLATORBEACON_H */
