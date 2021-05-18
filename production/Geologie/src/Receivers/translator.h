/**

 * @file translator.c

 *

 * @brief Contient differrente fonction permettant de convertir les structures de donnees en

 * un tableau d'octet.

 *

 * @version 1.0

 * @date 5 mai 2021

 * @author GAUTIER Pierre-Louis

 * @copyright BSD 2-clauses

 *

 */

#ifndef TRANSLATOR_

#define TRANSLATOR_

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Include

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../tools.h"
#include "receivers.h"
//#include "../config.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Variable et structure extern

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions publiques

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**

 * @brief Convertie une structure BeaconData en un tableau d'octet.

 *

 * La conversion est ecrite dans le tableau @p dest.

 *

 * @param beaconData Un pointeur vers la stucture a convertir.

 * @param dest Le pointeur vers tableau dans lequel ecrire la conversion.

 *

 * @warning Le tableau @p dest doit etre un tableau dont la taille fait au minimum 32

 * Aucune verification de la taille de celui-ci ne sera faite.

 */

//extern void Translator_convertBeaconDataToByte(const BeaconData* beaconData, unsigned char* dest);

/**

 * @brief Convertie une structure Position en un tableau d'octet.

 *

 * @param position Un pointeur vers la stucture a convertir.

 * @param dest Le pointeur vers tableau dans lequel ecrire la conversion.

 *

 * @warning Le tableau @p dest doit etre un tableau dont la taille fait au minimum 8

 * Aucune verification de la taille de celui-ci ne sera faite.

 */

//extern void Translator_convertPositionToByte(const Position* position, unsigned char* dest);

/**

 * @brief Convertie un tableau d'octet en un structure Position.

 *

 * @param src Le pointeur vers le tableau a convertir.

 * @param dest La structure dans laquelle ecrire la conversion.

 */

//extern void Translator_convertByteToPosition(const unsigned char* src, Position* dest);

/**

 * @brief Convertie un tableau d'octet en un structure BeaconData.

 *

 * @param src Le pointeur vers le tableau a convertir.

 * @param dest La structure dans laquelle ecrire la conversion.

 */

extern void Translator_convertByteToRawData(const unsigned char* src, RawData* dest);

#endif /* TRANSLATOR_ */
