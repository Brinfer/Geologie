/**
 * @file geographer_test.c
 *
 * @brief Ensemble de test pour Geographer
 *
 * @version 1.0
 * @date 11-06-2021
 * @author LECENNE Gabriel
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <limits.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include "cmocka.h"

#include "Geographer/geographer.c"

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

// Barrière de synchronisation en le thread de test (pour l'objet actif) et le thread de scénariob.
static pthread_barrier_t barrier_scenario;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Lance la suite de test du module geographer.
 *
 * @return int32_t 0 en cas de succes ou le numero du test qui a echoue.
 */
static int32_t geographer_run_tests();

static int32_t setUp(void** state);

static int32_t tearDown(void** state);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int32_t setUp(void** state){
	pthread_barrier_init(&barrier_scenario, NULL, 2);
	return 0; 
}

static int32_t tearDown(void **state) {
	pthread_barrier_destroy(&barrier_scenario);
	return 0;
}

State_GEOGRAPHER expectedFinalState;
int8_t expectedCalibrationCounter;
ConnectionState expectedConnectionState;
ExperimentalPosition * expectedExperimentalPositions;
unsigned short expectedNbExperimentalPositions;
ExperimentalTraject * expectedExperimentalTrajects;
unsigned short expectedNbExperimentalTrajects;
Date expectedCurrentDate;
BeaconData * expectedBeaconData;
int8_t expectedBeaconDataSize;
Position * expectedCurrentPosition;
ProcessorAndMemoryLoad * expectedProcessorAndMemoryLoad;
CalibrationPosition * expectedCalibrationPositions;
uint16_t expectedNbCalibrationPositions;
CalibrationPosition expectedCalibrationPosition;
CalibrationData * expectedCalibrationData;
uint8_t expectedNbCalibration;

void __wrap_Geographer_transitionFct(MqMsg msg)
{
    switch (msg.event)
    {
    case E_STOP:

        expect_function_call(__wrap_Scanner_ask4StopScanner);
        expect_function_call(__wrap_ProxyLoggerMOB_stop);
		expect_function_call(__wrap_ProxyGUI_stop);

        //Is running the real function
        __real_Geographer_transitionFct(msg);

        //Test side effect
        assert_int_equal(expectedFinalState, myState);
		assert_int_equal(expectedCalibrationCounter, calibrationCounter);
		assert_int_equal(expectedConnectionState, connectionState);
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_CONNECTION_ESTABLISHED:;

        expect_function_call(__wrap_ProxyLoggerMOB_setExperimentalPositions);
        expect_function_call(__wrap_ProxyLoggerMOB_setExperimentalTrajects);

		// Vérification paramètre de l'appel à __wrap_mod1_call.
		expect_value(__wrap_ProxyLoggerMOB_setExperimentalPositions, experimentalPositions, expectedExperimentalPositions);
		expect_value(__wrap_ProxyLoggerMOB_setExperimentalPositions, EXP_POSITION_NUMBER, expectedNbExperimentalPositions);

		expect_value(__wrap_ProxyLoggerMOB_setExperimentalTrajects, experimentalTrajects, expectedExperimentalTrajects);
		expect_value(__wrap_ProxyLoggerMOB_setExperimentalTrajects, EXP_TRAJECT_NUMBER, expectedNbExperimentalTrajects);

        __real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, myState);
		assert_int_equal(expectedConnectionState, connectionState);
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_DATE_AND_SEND_DATA_ELSE:

        __real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, myState);
		pthread_barrier_wait(&barrier_scenario);
        break;

    case E_DATE_AND_SEND_DATA_CONNECTED:

        expect_function_call(__wrap_ProxyLoggerMOB_setAllBeaconsData);
        expect_function_call(__wrap_ProxyLoggerMOB_setCurrentPosition);
		expect_function_call(__wrap_ProxyLoggerMOB_setProcessorAndMemoryLoad);

		expect_value(__wrap_ProxyLoggerMOB_setAllBeaconsData, msg.beaconsData, expectedBeaconData);
		expect_value(__wrap_ProxyLoggerMOB_setAllBeaconsData, msg.beaconsDataSize, expectedBeaconDataSize);
		expect_value(__wrap_ProxyLoggerMOB_setAllBeaconsData, currentDate, expectedCurrentDate);

		expect_value(__wrap_ProxyLoggerMOB_setCurrentPosition, expectedCurrentPosition, expectedCurrentPosition);
		expect_value(__wrap_ProxyLoggerMOB_setCurrentPosition, currentDate, expectedCurrentDate);

		expect_value(__wrap_ProxyLoggerMOB_setProcessorAndMemoryLoad, expectedProcessorAndMemoryLoad, expectedProcessorAndMemoryLoad);
		expect_value(__wrap_ProxyLoggerMOB_setProcessorAndMemoryLoad, currentDate, expectedCurrentDate);

        __real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, myState);
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_ASK_CALIBRATION_POSITIONS:

		expect_function_call(__wrap_ProxyGUI_setCalibrationPositions);

		expect_value(__wrap_ProxyGUI_setCalibrationPositions, calibrationPositions, expectedNbCalibrationPositions);
		expect_value(__wrap_ProxyGUI_setCalibrationPositions, CALIBRATION_POSITION_NUMBER, expectedNbCalibrationPositions);

		__real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, myState);
		assert_int_equal(expectedCalibrationCounter, calibrationCounter);
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_CONNECTION_DOWN:

		__real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, myState);
		assert_int_equal(expectedConnectionState, connectionState);		
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_VALIDATE_POSITION:

		expect_function_call(__wrap_Scanner_ask4UpdateAttenuationCoefficientFromPosition);

		//expect_value(__wrap_Scanner_ask4UpdateAttenuationCoefficientFromPosition, calibrationPositions[calibrationCounter], expectedCalibrationPosition);

		__real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, myState);
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_SIGNAL_END_UPDATE_ATTENUATION_CALIBRATION:

		expect_function_call(__wrap_ProxyGUI_signalEndCalibrationPosition);

		__real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, myState);
		assert_int_equal(expectedCalibrationCounter, calibrationCounter+1);
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_SIGNAL_END_UPDATE_ATTENUATION_ELSE:

		expect_function_call(__wrap_ProxyLoggerMOB_setCalibrationData);
		expect_function_call(__wrap_ProxyGUI_signalEndCalibration);

		expect_value(__wrap_ProxyLoggerMOB_setCalibrationData, msg.calibrationData, expectedCalibrationData);
		expect_value(__wrap_ProxyLoggerMOB_setCalibrationData, msg.nbCalibration, expectedNbCalibration);

		__real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, myState);
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_SIGNAL_END_AVERAGE_CALCUL:

		expect_function_call(__wrap_ProxyLoggerMOB_setCalibrationDataa);
		expect_function_call(__wrap_ProxyGUI_signalEndCalibration);

		expect_value(__wrap_ProxyLoggerMOB_setCalibrationData, msg.calibrationData, expectedCalibrationData);
		expect_value(__wrap_ProxyLoggerMOB_setCalibrationData, msg.nbCalibration, expectedNbCalibration);

		__real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, myState);
        pthread_barrier_wait(&barrier_scenario);

        break;

    default:
        break;
    }
}

static pthread_t thread_scenario;

static void *test_geographer_startCalibrationStop_scenario(void *arg)
{
    // First test
    expectedFinalState = S_IDLE;
    myState = S_WATING_FOR_CONNECTION;
    Geographer_signalConnectionEstablished();
    pthread_barrier_wait(&barrier_scenario);

    //Second test
    expectedFinalState = S_WAITING_FOR_BE_PLACED;
    myState = S_IDLE;
    Geographer_askCalibrationPositions();
    pthread_barrier_wait(&barrier_scenario);

    //Third test
    expectedFinalState = S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION;
    myState = S_WAITING_FOR_BE_PLACED;
    CalibrationPositionId calibrationPosition = 1;
    Geographer_validatePosition(calibrationPosition);
    pthread_barrier_wait(&barrier_scenario);

    //Fourth test
    expectedFinalState = S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION;
    myState = S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION;
    calibrationCounter = CALIBRATION_POSITION_NUMBER - 1;
    Geographer_signalEndUpdateAttenuation();
    pthread_barrier_wait(&barrier_scenario);

    //Fifth test
    expectedFinalState = S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT;
    myState = S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION;
    calibrationCounter = CALIBRATION_POSITION_NUMBER;
    Geographer_signalEndUpdateAttenuation();
    pthread_barrier_wait(&barrier_scenario);

    //Sixth test
    expectedFinalState = S_IDLE;
    myState = S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT;
    CalibrationData * data;
    int8_t nbCalibration = 3;
    Geographer_signalEndAverageCalcul(data, nbCalibration);
    pthread_barrier_wait(&barrier_scenario);

    //Seventh test
    expectedFinalState = S_DEATH;
    myState = S_IDLE;
    Geographer_askSignalStopGeographer();
    pthread_barrier_wait(&barrier_scenario);

    return NULL;
}

static void test_geographer_startCalibrationStop(void **state)
{
    Geographer_new();
    expect_function_call(__wrap_ProxyGUI_new);
    expect_function_call(__wrap_ProxyLoggerMOB_new);
    expect_function_call(__wrap_Scanner_new);

    Geographer_askSignalStartGeographer();
    expect_function_call(__wrap_ProxyGUI_start);
    expect_function_call(__wrap_ProxyLoggerMOB_start);
    expect_function_call(__wrap_Scanner_ask4StartScanner);

    // Creation & execution of the thread scenario

    if (pthread_create(&thread_scenario, NULL, test_geographer_startCalibrationStop_scenario, NULL) != 0)
    {
        TRACE("test scenario error %s\n", "pthread_create()");
    }

    pthread_detach(thread_scenario);

    // Test thread run the active object
    run(NULL);
}

static const struct CMUnitTest tests[] =
{
    cmocka_unit_test(test_geographer_startCalibrationStop)
};

extern int geographer_run_tests(void) {
    return cmocka_run_group_tests_name("Test of the Geographer module", tests, setUp, tearDown);
}


