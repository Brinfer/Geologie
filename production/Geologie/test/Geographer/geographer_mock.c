#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"

#include "Geographer/geographer.h"


void __wrap_Scanner_ask4StopScanner() {

	// Vérification de l'appel de function.
	function_called();
}

void __wrap_ProxyLoggerMOB_stop() {

	// Vérification de l'appel de function.
	function_called();
}

void __wrap_ProxyGUI_stop() {

	// Vérification de l'appel de function.
	function_called();
}

void __wrap_ProxyLoggerMOB_setExperimentalPositions(const ExperimentalPosition * experimentalPosition, unsigned short NbExperimentalPositions){
	// Vérification de l'appel de function.
	function_called();

    // Vérification du paramètre.
	check_expected(experimentalPosition);
    check_expected(NbExperimentalPositions);
}

void __wrap_ProxyLoggerMOB_setExperimentalTrajects(const ExperimentalTraject * experimentalTrajects, unsigned short NbExperimentalTrajects){
	// Vérification de l'appel de function.
	function_called();

    // Vérification du paramètre.
	check_expected(experimentalTrajects);
    check_expected(NbExperimentalTrajects);
}

void __wrap_ProxyLoggerMOB_setAllBeaconsData(const BeaconData * beaconData, unsigned short nbBeacons, Date currentDate){
	// Vérification de l'appel de function.
	function_called();

    // Vérification du paramètre.
	check_expected(beaconData);
    check_expected(nbBeacons);
    check_expected(currentDate);
}

void __wrap_ProxyLoggerMOB_setCurrentPosition(const Position * currentPosition, Date currentDate){
	// Vérification de l'appel de function.
	function_called();

    // Vérification du paramètre.
	check_expected(currentPosition);
    check_expected(currentDate);
}

void __wrap_ProxyLoggerMOB_setProcessorAndMemoryLoad(const ProcessorAndMemoryLoad * processorAndMemoryLoad, Date currentDate){
	// Vérification de l'appel de function.
	function_called();

    // Vérification du paramètre.
	check_expected(processorAndMemoryLoad);
    check_expected(currentDate);
}

void __wrap_ProxyGUI_setCalibrationPositions(CalibrationPosition * calibrationPositions, uint16_t size){
	// Vérification de l'appel de function.
	function_called();

    // Vérification du paramètre.
	check_expected(calibrationPositions);
    check_expected(size);
}

void __wrap_Scanner_ask4UpdateAttenuationCoefficientFromPosition(CalibrationPosition calibrationPosition){
	// Vérification de l'appel de function.
	function_called();

    // Vérification du paramètre.
	//check_expected(calibrationPosition);
}

void __wrap_ProxyGUI_signalEndCalibrationPosition(){
	// Vérification de l'appel de function.
	function_called();
}

void __wrap_ProxyLoggerMOB_setCalibrationData(const CalibrationData * calibrationData, uint8_t nbCalibration){
	// Vérification de l'appel de function.
	function_called();

    // Vérification du paramètre.
	check_expected(calibrationData);
    check_expected(nbCalibration);

}

void __wrap_ProxyGUI_signalEndCalibration(){
	// Vérification de l'appel de function.
	function_called();
}

