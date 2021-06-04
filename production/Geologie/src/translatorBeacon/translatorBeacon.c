/**

 * @file translatorBeacon.c

 *

 * @version 1.0

 * @date 4 juin 2021

 * @author LECENNE Gabriel

 * @copyright Geo-Boot

 * @license BSD 2-clauses

 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Include

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include <stdlib.h>

#include <string.h>

#include <arpa/inet.h>

#include "translatorBeacon.h"

#include "../Receivers/receivers.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Define

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**

 * @brief Convertie un entier en un tableau d'octet.

 *

 * Copie dans le tableau @p dest la valeur des octects de l'entier.

 *

 * @param i L'entier a convertir.

 * @param dest Le pointeur vers le tableau ou ecrire les octects.

 *

 * @warning Le tableau @p dest doit etre un tableau dont la taille fait au minimum 4

 * Aucune verification de la taille de celui-ci ne sera faite.

 */

static void convertIntToBytes(int i, unsigned char* dest);

/**

 * @brief Convertie un floattant en un tableau d'octet.

 *

 * Copie dans le tableau @p dest la valeur des octects du floattant.

 *

 * @param f Le floattant a convertire.

 * @param dest Le pointeur vers le tableau ou ecrire les octects.

 *

 * @warning Le tableau @p dest doit etre un tableau dont la taille fait au minimum 4

 * Aucune verification de la taille de celui-ci ne sera faite.

 */

static void convertFloatToBytes(float f, unsigned char* dest);

/**

 * @brief Convertie un tableau d'octet en un entier.

 *

 * @param src Le pointeur vers le tableau d'octet.

 * @return int Le nombre entier en base 10 des octets.

 */

static int convertBytesToInt(const unsigned char* src);

/**

 * @brief Convertie un tableau d'octet en un entier.

 *

 * @param src Le pointeur vers le tableau d'octet.

 * @return float Le nombre reel en base 10 des octets.

 */

static float convertBytesToFloat(const unsigned char* src);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions publiques

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern BeaconsSignal TranslatorBeacon_translateChannelToBeaconsSignal(le_advertising_info * info){

    BeaconsSignal bs;

    char posX[5];
	char posY[5];

	memcpy(bs.name, info->data + 5, 2); //Mettre des define pour les index

	printf("Name : %c%c\n", bs.name[0], bs.name[1]);

	memcpy(bs.uuid, info->data + 21, 2);

	printf("\nUUID : %d\n", bs.uuid[0]);

	memcpy(posX, info->data + 8, 5); //parfois des Pbs au niveau du X

	sscanf(posX, "%d", &(bs.position.X));

	printf("\nX : %d\n", bs.position.X);

	memcpy(posY, info->data + 14, 5);	

	sscanf(posY, "%d", &(bs.position.Y));

	printf("\nY : %d\n", bs.position.Y);

	bs.rssi = (int8_t) info->data[info->length];

	printf("\nRSSI : %d\n", bs.rssi);

    return bs;


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions static

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void convertIntToBytes(int i, unsigned char* dest) {

    const int reversed = htonl(i);

    memcpy(dest, (unsigned char*) &reversed, sizeof(int));

}

static void convertFloatToBytes(float f, unsigned char* dest) {

    const int reversed = htonl(*(int*) &f);

    memcpy(dest, (unsigned char*) &reversed, sizeof(float));

}

static int convertBytesToInt(const unsigned char* src) {

    int returnVal;

    unsigned char tempArray[sizeof(int)];

    memcpy(tempArray, src, sizeof(int));

    returnVal = *(int*) tempArray;

    returnVal = ntohl(returnVal);

    return returnVal;

}

static float convertBytesToFloat(const unsigned char* src) {

    float returnVal;

    int temp;   // ntohl accept uint32_t not the float

    unsigned char tempArray[sizeof(float)];

    memcpy(tempArray, src, sizeof(float));

    temp = *(int*) tempArray;

    temp = ntohl(temp);

    returnVal = *(float*) &temp;

    return returnVal;

}

int main(){
    const unsigned char * data;
    data = "0201060F09623178333030303079343030303003031A1803190003";
    printf("%s\n", data);
    TranslatorBeacon_translateChannelToBeaconsSignal(data);
}