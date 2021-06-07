/**
 * @file geographer.c
 *
 * @version 1.0
 * @date 03/06/21
 * @author Nathan BRIENT
 * @copyright BSD 2-clauses
 * 
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
#include "geographer.h"
#include <stdlib.h>
#include <unistd.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** 
* @brief Tableau d'entier 
* creation d'un tableau constant d'entier 
*/

/** 
 * 
 * @brief On documente ce qui est privé 
 * (et uniquement ce qui
 * est privé) dans le fichier .c puisque le reste (public)
 * est documenté dans le fichier .h.
 *
 * Remarque : les variables globales sont à éviter. Elles sont
 * tolérées dans les cas connus suivants :
 * - partage de données entre tâches ou entre tâches et
 *   interruptions (attention à la synchronisation)
 * - traduction d'une conception objet, dans le cas d'un
 *   singleton
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Ceci est une fonction privée.
 *
 * On documente les fonctions privées (et seulement celles-ci) dans le .c.
 *
 * Pour éviter une déclaration préalable (prototype), on les place
 * avant la définition des fonctions publiques.
 *
 * @param[in,out] instance instance à initialiser
 * @param[in] val valeur d'initialisation
 */

/**
 * @fn static uneFonctionPourLExemple(void)
 * @brief une fonction exemple
 * fonction exemple qui va montrer plusieurs cas à faire ou ne pas faire
 */ 

  /**
  * @brief Exemple de commentaire
  * Un commentaire peut servir à réaliser un titre de "paragraphe",
  * un paragraphe constituant (comme dans du texte) une étape/idée
  * cohérente dans l'algorithme
  */



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions publiques
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int Geographer_new(){
    int returnError=EXIT_FAILURE;

    return returnError;
}

extern int Geographer_free(){
    int returnError=EXIT_FAILURE;

    return returnError;
}


extern int Geographer_ask4SignalStartGeographer(){
    int returnError=EXIT_FAILURE;

    return returnError;
}


extern int Geographer_ask4SignalStopGeographer(){
    int returnError=EXIT_FAILURE;

    return returnError;
}


extern int Geographer_ask4CalibrationPositions(){
    int returnError=EXIT_FAILURE;

    return returnError;
}


extern int Geographer_validatePosition(CalibrationPositionId calibrationPositionId){
    int returnError=EXIT_FAILURE;

    return returnError;
}


extern int Geographer_signalEndUpdateAttenuation(){
    int returnError=EXIT_FAILURE;

    return returnError;
}


extern int Geographer_signalEndAverageCalcul(CalibrationData calibrationData[]){
    int returnError=EXIT_FAILURE;

    return returnError;
}


extern int Geographer_signalConnectionEstablished(){
    int returnError=EXIT_FAILURE;

    return returnError;
}


extern int Geographer_signalConnectionDown(){
    int returnError=EXIT_FAILURE;

    return returnError;
}



extern int Geographer_dateAndSendData(BeaconData beaconsData[], Position currentPosition, ProcessorAndMemoryLoad currentProcessorAndMemoryLoad){
    int returnError=EXIT_FAILURE;

    return returnError;
}





