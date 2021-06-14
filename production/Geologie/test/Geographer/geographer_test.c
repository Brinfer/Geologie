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

#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <limits.h>
#include "../../src/tools.h"

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

/**
 * @brief Lance la suite de test du module geographer.
 *
 * @return int32_t 0 en cas de succes ou le numero du test qui a echoue.
 */

extern int32_t geographer_run_tests();

StateGeographer expectedFinalState;
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

/**
 * wrap de la fonction __wrap_Geographer_transitionFct(MqMsg msg)
 */


void __wrap_Geographer_transitionFct(MqMsg msg)
{
    switch (msg.event)
    {
    case E_STOP:

        expect_function_call(__wrap_Scanner_ask4StopScanner);
        expect_function_call(__wrap_ProxyLoggerMOB_stop);
		expect_function_call(__wrap_ProxyGUI_stop);

        __real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, currentState);
		assert_int_equal(expectedCalibrationCounter, calibrationCounter);
		assert_int_equal(expectedConnectionState, connectionState);
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_CONNECTION_ESTABLISHED:;

        expect_function_call(__wrap_ProxyLoggerMOB_setExperimentalPositions);
        expect_function_call(__wrap_ProxyLoggerMOB_setExperimentalTrajects);

		expect_value(__wrap_ProxyLoggerMOB_setExperimentalPositions, experimentalPositions, expectedExperimentalPositions);
		expect_value(__wrap_ProxyLoggerMOB_setExperimentalPositions, NB_EXPERIMENTAL_POSITION, expectedNbExperimentalPositions);

		expect_value(__wrap_ProxyLoggerMOB_setExperimentalTrajects, experimentalTrajects, expectedExperimentalTrajects);
		expect_value(__wrap_ProxyLoggerMOB_setExperimentalTrajects, NB_EXPERIMENTAL_TRAJECT, expectedNbExperimentalTrajects);

        __real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, currentState);
		assert_int_equal(expectedConnectionState, connectionState);
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_DATE_AND_SEND_DATA:

        __real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, currentState);
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

        assert_int_equal(expectedFinalState, currentState);
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_ASK_CALIBRATION_POSITIONS:

		expect_function_call(__wrap_ProxyGUI_setCalibrationPositions);

		expect_value(__wrap_ProxyGUI_setCalibrationPositions, calibrationPositions, expectedNbCalibrationPositions);
		expect_value(__wrap_ProxyGUI_setCalibrationPositions, CALIBRATION_POSITION_NUMBER, expectedNbCalibrationPositions);

		__real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, currentState);
		assert_int_equal(expectedCalibrationCounter, calibrationCounter);
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_CONNECTION_DOWN:

		__real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, currentState);
		assert_int_equal(expectedConnectionState, connectionState);
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_VALIDATE_POSITION:

		expect_function_call(__wrap_Scanner_ask4UpdateAttenuationCoefficientFromPosition);

		__real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, currentState);
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_SIGNAL_END_UPDATE_ATTENUATION_CALIBRATION:

		expect_function_call(__wrap_ProxyGUI_signalEndCalibrationPosition);

		__real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, currentState);
		assert_int_equal(expectedCalibrationCounter, calibrationCounter+1);
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_SIGNAL_END_UPDATE_ATTENUATION:

		expect_function_call(__wrap_ProxyLoggerMOB_setCalibrationData);
		expect_function_call(__wrap_ProxyGUI_signalEndCalibration);

		expect_value(__wrap_ProxyLoggerMOB_setCalibrationData, msg.calibrationData, expectedCalibrationData);
		expect_value(__wrap_ProxyLoggerMOB_setCalibrationData, msg.nbCalibrationData, expectedNbCalibration);

		__real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, currentState);
        pthread_barrier_wait(&barrier_scenario);

        break;

    case E_SIGNAL_END_AVERAGE_CALCUL:

		expect_function_call(__wrap_ProxyLoggerMOB_setCalibrationDataa);
		expect_function_call(__wrap_ProxyGUI_signalEndCalibration);

		expect_value(__wrap_ProxyLoggerMOB_setCalibrationData, msg.calibrationData, expectedCalibrationData);
		expect_value(__wrap_ProxyLoggerMOB_setCalibrationData, msg.nbCalibrationData, expectedNbCalibration);

		__real_Geographer_transitionFct(msg);

        assert_int_equal(expectedFinalState, currentState);
        pthread_barrier_wait(&barrier_scenario);

        break;

    default:
        break;
    }
}

static pthread_t thread_scenario;

/**
 * @brief StartCalibrationStop test : \n
 * Test du scénario qui consite à demarrer Geographer, effectuer le calibrage pour stopper l'objet
 *
 * Le premier test va verifier le comportement de l'objet lors du signal de la connexion etablie avec l'android\n
 * -> On demarre a l'etat S_WATING_FOR_CONNECTION \n
 * -> la fonction Geographer_signalConnectionEstablished() intervient et engendre l'evenement E_CONNECTION_ESTABLISHED \n
 * -> L'etat final est S_IDLE \n
 *
 *  * Le second test va verifier le comportement de l'objet lors du signal de la demande de calibrage\n
 * -> On demarre a l'etat S_IDLE \n
 * -> la fonction Geographer_askCalibrationPositions() intervient et engendre l'evenement E_ASK_CALIBRATION_POSITIONS \n
 * -> L'etat final est S_WAITING_FOR_BE_PLACED \n
 *
 *  * Le troisieme test va verifier le comportement de l'objet lorsque le robot sera palce sur une position de calibrage \n
 * -> On demarre a l'etat S_WAITING_FOR_BE_PLACED \n
 * -> la fonction Geographer_validatePosition(CalibrationPositionId) intervient et engendre l'evenement E_VALIDATE_POSITION \n
 * -> L'etat final est S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION \n
 *
 *  * Les quatieme et cinquieme tests vont verifier le comportement de l'objet lors des calculs des coefficent d'attenuation par position \n
 * -> L'un si il reste des positions à tester, l'autre si celles-ci ont déjà toutes été essayées
 * -> On demarre a l'etat S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION \n
 * -> la fonction Geographer_signalEndUpdateAttenuation() intervient et engendre l'evenement E_SIGNAL_END_UPDATE_ATTENUATION_CALIBRATION ou E_SIGNAL_END_UPDATE_ATTENUATION \n
 * -> L'etat final est alors S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION ou S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT\n
 *
 * * Le sixieme test verifie le comportement a la fin du processus de calibrage
 *
 * * Le septieme verifie la bonne extinction de l'objet Geographer
 */

static void *test_geographer_startCalibrationStop_scenario(void *arg)
{
    // 1er test
    expectedFinalState = S_IDLE;
    currentState = S_WATING_FOR_CONNECTION;
    Geographer_signalConnectionEstablished();
    pthread_barrier_wait(&barrier_scenario);

    //2nd test
    expectedFinalState = S_WAITING_FOR_BE_PLACED;
    currentState = S_IDLE;
    Geographer_askCalibrationPositions();
    pthread_barrier_wait(&barrier_scenario);

    //3eme test
    expectedFinalState = S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION;
    currentState = S_WAITING_FOR_BE_PLACED;
    CalibrationPositionId calibrationPosition = 1;
    Geographer_validatePosition(calibrationPosition);
    pthread_barrier_wait(&barrier_scenario);

    //4eme test
    expectedFinalState = S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION;
    currentState = S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION;
    calibrationCounter = CALIBRATION_POSITION_NUMBER - 1;
    Geographer_signalEndUpdateAttenuation();
    pthread_barrier_wait(&barrier_scenario);

    //5eme test
    expectedFinalState = S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT;
    currentState = S_WAITING_FOR_ATTENUATION_COEFFICIENT_FROM_POSITION;
    calibrationCounter = CALIBRATION_POSITION_NUMBER;
    Geographer_signalEndUpdateAttenuation();
    pthread_barrier_wait(&barrier_scenario);

    //6eme test
    expectedFinalState = S_IDLE;
    currentState = S_WAITING_FOR_CALCUL_AVERAGE_COEFFICIENT;
    CalibrationData * data;
    int8_t nbCalibration = 3;
    Geographer_signalEndAverageCalcul(data, nbCalibration);
    pthread_barrier_wait(&barrier_scenario);

    //7eme test
    expectedFinalState = S_DEATH;
    currentState = S_IDLE;
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

    Geographer_askCalibrationPositions();

    Geographer_askSignalStopGeographer();
    expect_function_call(__wrap_ProxyGUI_stop);
    expect_function_call(__wrap_ProxyLoggerMOB_stop);
    expect_function_call(__wrap_Scanner_ask4StopScanner);

    if (pthread_create(&thread_scenario, NULL, test_geographer_startCalibrationStop_scenario, NULL) != 0)
    {
        TRACE("test scenario error %s\n", "pthread_create()");
    }

    pthread_detach(thread_scenario);

    runGeographer(NULL);
}

/**
 * @brief connectionDown test : \n
 * Test du scénario de la perte de connexion
 */

static void *test_geographer_connectionDown_scenario(void *arg)
{
    // Premier test
    expectedFinalState = S_IDLE;
    currentState = S_WATING_FOR_CONNECTION;
    Geographer_signalConnectionEstablished();
    pthread_barrier_wait(&barrier_scenario);

    //Second test
    expectedFinalState = S_WATING_FOR_CONNECTION;
    currentState = S_IDLE;
    Geographer_signalConnectionDown();
    pthread_barrier_wait(&barrier_scenario);

    return NULL;
}

static void test_geographer_connectionDown(void **state)
{
    Geographer_new();
    expect_function_call(__wrap_ProxyGUI_new);
    expect_function_call(__wrap_ProxyLoggerMOB_new);
    expect_function_call(__wrap_Scanner_new);

    Geographer_askSignalStartGeographer();
    expect_function_call(__wrap_ProxyGUI_start);
    expect_function_call(__wrap_ProxyLoggerMOB_start);
    expect_function_call(__wrap_Scanner_ask4StartScanner);

    if (pthread_create(&thread_scenario, NULL, test_geographer_connectionDown_scenario, NULL) != 0)
    {
        TRACE("test scenario error %s\n", "pthread_create()");
    }

    pthread_detach(thread_scenario);

    runGeographer(NULL);
}

static const struct CMUnitTest tests[] =
{
    cmocka_unit_test(test_geographer_startCalibrationStop),
    cmocka_unit_test(test_geographer_connectionDown),

};

extern int geographer_run_tests(void) {
    return cmocka_run_group_tests_name("Test of the Geographer module", tests, setUp, tearDown);
}
