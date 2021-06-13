/**
 * @file scanner_mock.c
 *
 * @version 1.0
 * @date 03-06-21
 * @author Nathan BRIENT
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 *
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <limits.h>
#include "../../src/tools.h"


#include "cmocka.h"
#include "Scanner/scanner.h"
#include "Watchdog/watchdog.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// extern void Scanner_new(){

// }
////action case stop
extern void __wrap_Bookkeeper_new() {

	TRACE("__wrap_Bookkeeper_new %s", "\n");
	function_called();
}
extern void __wrap_Bookkeeper_free() {
	TRACE("__wrap_Bookkeeper_free %s", "\n");
	function_called();
}
extern void __wrap_Bookkeeper_askStartBookkeeper() {
	TRACE("__wrap_Bookkeeper_askStartBookkeeper %s", "\n");
	function_called();
}
extern void __wrap_Receiver_free() {
	TRACE("__wrap_Receiver_free %s", "\n");
	function_called();
}
extern void __wrap_Receiver_ask4StartReceiver() {
	TRACE("__wrap_Receiver_ask4StartReceiver %s", "\n");
	function_called();
}
extern void __wrap_Receiver_new() {
	TRACE("__wrap_Receiver_new %s", "\n");
	function_called();
}
extern int8_t __wrap_Receiver_ask4StopReceiver() {
	TRACE("__wrap_Receiver_ask4StopReceiver %s", "\n");
	function_called();
	return (int8_t) mock();
}
extern int8_t __wrap_Bookkeeper_askStopBookkeeper() {
	TRACE("__wrap_Bookkeeper_askStopBookkeeper %s", "\n");
	function_called();
	return (int8_t) mock();
}
/// action set current position
extern void __wrap_Mathematician_getCurrentPosition(const BeaconData* beaconsData, uint8_t nbBeacon, Position* currentPosition) {
	// Vérification de l'appel de function.
	TRACE("__wrap_Mathematician_getCurrentPosition %s", "\n");
	function_called();

		// Vérification du paramètre.
	//check_expected(beaconsData);

	//check_expected(nbBeacon);

}

extern void __wrap_Bookkeeper_ask4CurrentProcessorAndMemoryLoad() {
	// Vérification de l'appel de function.
	TRACE("__wrap_Bookkeeper_ask4CurrentProcessorAndMemoryLoad %s", "\n");
	function_called();
}

//action set current processor and memory load


extern int8_t __wrap_Geographer_dateAndSendData(BeaconData* beaconsData, int8_t beaconsDataSize, Position* currentPosition, ProcessorAndMemoryLoad* currentProcessorAndMemoryLoad) {
	// Vérification de l'appel de function.
	TRACE("__wrap_Geographer_dateAndSendData %s", "\n");
	function_called();

		// Vérification du paramètre.
	// check_expected(beaconsData);
	// check_expected(beaconsDataSize);
	// check_expected(currentPosition);
	// check_expected(currentProcessorAndMemoryLoad);

	// Retour d'une valeur bouchonnée.
	return (int8_t) mock();

}

//action askCalibrationFromPosition

extern AttenuationCoefficient __wrap_Mathematician_getAttenuationCoefficient(const Power* power, const Position* beaconPosition, const CalibrationPosition* calibrationPosition) {
	// Vérification de l'appel de function.
	TRACE("__wrap_Mathematician_getAttenuationCoefficient %s", "\n");
	function_called();

		// Vérification du paramètre.
		// check_expected(power);
		// check_expected(beaconPosition);
		// check_expected(calibrationPosition);

		// Retour d'une valeur bouchonnée.
	return (AttenuationCoefficient) mock();
}

extern int8_t __wrap_Geographer_signalEndUpdateAttenuation() {
	// Vérification de l'appel de function.
	TRACE("__wrap_Geographer_signalEndUpdateAttenuation %s", "\n");
	function_called();

		// Retour d'une valeur bouchonnée.
	return (int8_t) mock();
}

//Action askCalibrationAverage
extern AttenuationCoefficient __wrap_Mathematician_getAverageCalcul(const BeaconCoefficients* beaconCoefficients, uint8_t nbCoefficient) {
	// Vérification de l'appel de function.
	TRACE("__wrap_Mathematician_getAverageCalcul %s", "\n");
	function_called();

		// Vérification du paramètre.
	// check_expected(beaconCoefficients);
	// check_expected(nbCoefficient);


	// Retour d'une valeur bouchonnée.
	return (AttenuationCoefficient) mock();
}
//Action askBeaconsSignal

extern int8_t __wrap_Receiver_ask4BeaconsSignal() {
	// Vérification de l'appel de function.
	TRACE("__wrap_Receiver_ask4BeaconsSignal %s", "\n");
	function_called();


		// Retour d'une valeur bouchonnée.
	return (int8_t) mock();
}
extern void __wrap_Watchdog_start(Watchdog* this) {
	TRACE("__wrap_Watchdog_start %s", "\n");
	function_called();
}
Watchdog* __wrap_Watchdog_construct(uint32_t thisDelay, WatchdogCallback callback) {
	TRACE("__wrap_Watchdog_construct %s","\n");
	function_called();
}
extern void __wrap_Watchdog_destroy(Watchdog* this) {
	TRACE("__wrap_Watchdog_destroy %s", "\n");
	function_called();
}

extern int8_t __wrap_Geographer_signalEndAverageCalcul(CalibrationData* calibrationData, int8_t nbCalibration) {
	// Vérification de l'appel de function.
	TRACE("__wrap_Geographer_signalEndAverageCalcul %s", "\n");
	function_called();


		// Retour d'une valeur bouchonnée.
	return (int8_t) mock();
}

// static void __wrap_scanner_performAction(Action_SCANNER action, MqMsg* msg){
// 	TRACE("__wrap_Bookkeeper_new %s","\n");
//function_called();
// 	check_expected(action);
// 	check_expected(msg);

// }




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



