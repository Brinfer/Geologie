/**


 * @file scanner.c


 *


 * @version 2.0


 * @date 7/06/2021


 * @author Gabriel LECENNE


 * @copyright Geo-Boot


 * @license BSD 2-clauses


 */

#include <stdlib.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Variable et structure privee

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static BeaconsData beaconsData[];
static Position currentPosition;
static ProcessorAndMemoryLoad currentProcessorAndMemoryLoad;
static BeaconCoefficients beaconsCoefficient[];
static BeaconSignal beaconSignal[];

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions static

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//

//                                              Fonctions extern

//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**


 * @fn static void Scanner_ask4StartScanner()


 * @brief TODO


 */

extern void Scanner_ask4StopScanner(){

}

/**


 * @fn static void Scanner_ask4StartScanner()


 * @brief TODO


 */

extern void Scanner_ask4UpdateAttenuationCoefficientFromPosition(Position calibrationPosition){

}

/**


 * @fn static void Scanner_ask4StartScanner()


 * @brief TODO


 */

extern void Scanner_ask4AverageCalcul(){

}

/**


 * @fn static void Scanner_ask4StartScanner()


 * @brief TODO


 */

extern void Scanner_setAllBeaconsSignal(BeaconsSignal : beaconsSignal[]){

}

/**


 * @fn static void Scanner_ask4StartScanner()


 * @brief TODO


 */

extern void Scanner_setCurrentProcessorAndMemoryLoad(ProcessorAndMemoryLoad currentProcessorAndMemoryLoad){

}