/**
 * @file scanner_test.c
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

#include "Scanner/scanner.c"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EPSILON (0.0001)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions static
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Barrière de synchronisation en le thread de test (pour l'objet actif) et le thread de scénario.
static pthread_barrier_t barrier_scenario;


static int set_up(void** state) {
    //int pthread_barrier_init(pthread_barrier_t *__restrict__ __barrier, const pthread_barrierattr_t *__restrict__ __attr, unsigned int __count)
    pthread_barrier_init(&barrier_scenario, NULL, 2);
    return 0;
}

static int tear_down(void** state) {
    pthread_barrier_destroy(&barrier_scenario);
    return 0;
}

// Variables partagées entre le thread de test et le thread de l'objet actif
// pour la configuration des mocks et des asserts
int scanner_expected_p, scanner_mock_ret, scanner_expected_internal_value;

Action_SCANNER expectedAction;
Event_SCANNER expectedEvent;
State_SCANNER expectedState;
MqMsgScanner expectedMsg;
BeaconData expectedBeaconsData;
Position expectedPosition;
int8_t expectedReturn;
AttenuationCoefficient expectedAttenuationCoefficient;
ProcessorAndMemoryLoad expectedCurrentProcessorAndMemoryLoad;

int8_t returnValue;
void __wrap_ScannerTime_out() { // je fais ca pour avoir ma methode en externe et l'appeler dans mon test
    __real_ScannerTime_out();
}

// Outillage de la fonction de transition à objectif de test
// Rq: la fonction de transition réel est appelée via __real_Scanner_transitionFct (man ld, cf. /wrap)
void __wrap_Scanner_transitionFct(MqMsgScanner msg) {

    //int8_t returnValue;
    switch (msg.event) {
        case E_TIME_OUT:
            // Vérification appel à __wrap_mod1_call.
            expect_function_call(__wrap_Receiver_ask4BeaconsSignal);
            will_return(__wrap_Receiver_ask4BeaconsSignal, returnValue);

            __real_Scanner_transitionFct(msg);

            //assert_int_equal(expectedReturn, returnValue);
            assert_int_equal(expectedState, myState);

            pthread_barrier_wait(&barrier_scenario);
            break;
        case E_SET_BEACONS_SIGNAL:

            expect_function_call(__wrap_Mathematician_getCurrentPosition);
            expect_function_call(__wrap_Bookkeeper_ask4CurrentProcessorAndMemoryLoad);

            __real_Scanner_transitionFct(msg);

            //assert_int_equal(returnPosition.X, expectedPosition.X);
            //assert_int_equal(returnPosition.Y, expectedPosition.Y);
            assert_int_equal(expectedState, myState);


            pthread_barrier_wait(&barrier_scenario);
            break;
        case E_SET_PROCESSOR_AND_MEMORY:
            //test currentProcessorAndMemoryLoad
            expect_function_call(__wrap_Geographer_dateAndSendData);
            will_return(__wrap_Geographer_dateAndSendData, returnValue);

            expect_function_call(__wrap_Watchdog_start);

            __real_Scanner_transitionFct(msg);

            //assert_float_equal(expectedCurrentProcessorAndMemoryLoad.memoryLoad, currentProcessorAndMemoryLoad.memoryLoad, EPSILON);
            //assert_float_equal(expectedCurrentProcessorAndMemoryLoad.processorLoad, currentProcessorAndMemoryLoad.processorLoad, EPSILON);
            assert_int_equal(expectedState, myState);


            pthread_barrier_wait(&barrier_scenario);
            break;

        case E_STOP:
            expect_function_call(__wrap_Receiver_ask4StopReceiver);
            expect_function_call(__wrap_Bookkeeper_askStopBookkeeper);

            __real_Scanner_transitionFct(msg);
            pthread_barrier_wait(&barrier_scenario);

            break;
        case E_ASK_UPDATE_COEF_FROM_POSITION:
            //TODO tester valeur de retour et internes
            expect_function_call(__wrap_Mathematician_getAttenuationCoefficient);

            expect_function_call(__wrap_Geographer_signalEndUpdateAttenuation);

            __real_Scanner_transitionFct(msg);

            assert_int_equal(expectedState, myState);
            pthread_barrier_wait(&barrier_scenario);
            break;
        case E_ASK_AVERAGE_CALCUL:
            //TODO tester valeurs de retour et internes
            expect_function_call(__wrap_Mathematician_getAverageCalcul);
            expect_function_call(__wrap_Geographer_signalEndAverageCalcul);

            __real_Scanner_transitionFct(msg);

            assert_int_equal(expectedState, myState);
            pthread_barrier_wait(&barrier_scenario);
            break;
        default:
            break;
    }
}


/**
 * Fonctions et thread de scénario
 * pour le test de la machine à états de scanner
 *
 */

static pthread_t pthread_scenario;

static void* test_scanner_state_machine_scenario(void* arg) {
    //initialisation des variables internes a scanner
    currentProcessorAndMemoryLoad.memoryLoad = 0;
    currentProcessorAndMemoryLoad.processorLoad = 0;
    currentPosition.X = 34;
    currentPosition.Y = 48;
    nbBeaconsAvailable = 3;

    BeaconSignal beaconsSignalTest[] = {
        {.name = {'B','1','\00'},
        .uuid = {24,26},
        .rssi = -50,
        .position = {.X = 0,.Y = 100}
        },
        {.name = {'B','2','\00'},
        .uuid = {24,26},
        .rssi = -97,
        .position = {.X = 35,.Y = 22}
        },
        {.name = {'B','3','\00'},
        .uuid = {24,26},
        .rssi = -50,
        .position = {.X = 100,.Y = 0}
        }
    };

    returnValue = 0;

    myState = S_BEGINNING;
    expectedState = S_WAITING_DATA_BEACONS;
    TRACE("ScannerTime_out %s", "\n");

    ScannerTime_out();
    pthread_barrier_wait(&barrier_scenario);


    expectedState = S_COMPUTE_POSITION;
    TRACE("Scanner_setAllBeaconsSignal %s", "\n");

    Scanner_setAllBeaconsSignal(beaconsSignalTest, nbBeaconsAvailable);    //TODO, changer NULL par des parametres
    pthread_barrier_wait(&barrier_scenario);

    expectedState = S_COMPUTE_LOAD;
    returnValue = 0;

    ProcessorAndMemoryLoad testProcessorAndMemoryLoad = { .memoryLoad = 25.1,.processorLoad = 64.5 };
    TRACE("Scanner_setCurrentProcessorAndMemoryLoad %s", "\n");
    Scanner_setCurrentProcessorAndMemoryLoad(&testProcessorAndMemoryLoad);
    pthread_barrier_wait(&barrier_scenario);

    expectedState = S_WAITING_DATA_BEACONS;
    TRACE("ScannerTime_out %s", "\n");

    ScannerTime_out();
    pthread_barrier_wait(&barrier_scenario);


    // arret de l'objet actif
    Scanner_ask4StopScanner();
    pthread_barrier_wait(&barrier_scenario);

    return NULL;
}

static void test_scanner_state_machine(void** state) {
    TRACE("je suis dans le début du test %s", "\n");
    expect_function_call(__wrap_Watchdog_construct);
    expect_function_call(__wrap_Receiver_new);
    expect_function_call(__wrap_Bookkeeper_new);

    Scanner_new();

    // Création et exécution du thread de scenario
    if (pthread_create(&pthread_scenario, NULL, test_scanner_state_machine_scenario, NULL) != 0) {
        fail_msg("test scenario error %s\n", "pthread_create()");
    }
    pthread_detach(pthread_scenario);

    //Scanner_ask4StartScanner();

    //Scanner_ask4StopScanner(); //PAS SUR
    //Scanner_free();		//PAS SUR

    // Le thread de test exécute l'object actif.
    run(NULL);
}


/**
 * Suite de test pour le module.
 *
 * A compléter si besoin est.
 *
 */
static const struct CMUnitTest tests[] =
{
    cmocka_unit_test(test_scanner_state_machine)
};



/**
 * Lancement de la suite de tests pour le module.
 */
int scanner_run_tests() {
    return cmocka_run_group_tests_name("Test du module scanner", tests, set_up, tear_down);
}
