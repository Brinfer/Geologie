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
 * @brief On documente ce qui est prive
 * (et uniquement ce qui
 * est prive) dans le fichier .c puisque le reste (public)
 * est documente dans le fichier .h.
 *
 * Remarque : les variables globales sont a eviter. Elles sont
 * tolerees dans les cas connus suivants :
 * - partage de donnees entre tâches ou entre tâches et
 *   interruptions (attention a la synchronisation)
 * - traduction d'une conception objet, dans le cas d'un
 *   singleton
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure privee
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Ceci est une fonction privee.
 *
 * On documente les fonctions privees (et seulement celles-ci) dans le .c.
 *
 * Pour eviter une declaration prealable (prototype), on les place
 * avant la definition des fonctions publiques.
 *
 * @param[in,out] instance instance a initialiser
 * @param[in] val valeur d'initialisation
 */

/**
 * @fn static uneFonctionPourLExemple(void)
 * @brief une fonction exemple
 * fonction exemple qui va montrer plusieurs cas a faire ou ne pas faire
 */

  /**
  * @brief Exemple de commentaire
  * Un commentaire peut servir a realiser un titre de "paragraphe",
  * un paragraphe constituant (comme dans du texte) une etape/idee
  * coherente dans l'algorithme
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


extern int Geographer_askSignalStartGeographer(){
    int returnError=EXIT_FAILURE;

    return returnError;
}


extern int Geographer_askSignalStopGeographer(){
    int returnError=EXIT_FAILURE;

    return returnError;
}


extern int Geographer_askCalibrationPositions(){
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
