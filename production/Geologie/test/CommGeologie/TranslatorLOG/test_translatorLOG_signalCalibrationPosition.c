/**
 * @file test_translatorLOG_signalCalibrationPosition.c
 *
 * @brief Ensemble de test pour tester TranslatorLOG_signalCalibrationPosition.
 *
 * @version 2.0
 * @date 11-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Include
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "cmocka.h"

#include "CommGeologie/TranslatorLOG/translatorLOG.h"
#include "CommGeologie/com_common.h"
#include "common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Structure passee aux fonctions tests.
 */
typedef struct {
    Trame trameInput[4];                /**< La #Trame passee en entree du test */
    CalibrationPositionId idExpected;   /**< Le #CalibrationPositionId attendue en sortie du test */
} ParameterTestCalibrationPosition;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Ensemble des donnees de tests.
 */
static ParameterTestCalibrationPosition parameterTest[] = {
    {.idExpected = 0, .trameInput = { SIGNAL_CALIBRATION_POSITION, 0x00, 0x01, 0x00}},
    {.idExpected = 255, .trameInput = { SIGNAL_CALIBRATION_POSITION, 0x00, 0x01, 0xFF}},
    {.idExpected = 170, .trameInput = { SIGNAL_CALIBRATION_POSITION, 0x00, 0x01, 0xAA}},
    {.idExpected = 85, .trameInput = { SIGNAL_CALIBRATION_POSITION, 0x00, 0x01, 0x55}}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Execute les tests de TranslatorLOG_signalCalibrationPosition.
 *
 * @return int 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int test_TranslatorLOG_run_translateSignalCalibrationPosition(void);

/**
 * @brief La fonction test permettant de verifier le bon fonctionnement de TranslatorLOG_signalCalibrationPosition.
 *
 * @param state Les donnees de test #ParameterTestCalibrationPosition.
 */
static void test_TranslatorLOG_signalCalibrationPosition(void** state);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Ensemble des tests a executer.
 */
static const struct CMUnitTest tests[] = {
    cmocka_unit_test_prestate(test_TranslatorLOG_signalCalibrationPosition, &(parameterTest[0])),
    cmocka_unit_test_prestate(test_TranslatorLOG_signalCalibrationPosition, &(parameterTest[1])),
    cmocka_unit_test_prestate(test_TranslatorLOG_signalCalibrationPosition, &(parameterTest[2])),
    cmocka_unit_test_prestate(test_TranslatorLOG_signalCalibrationPosition, &(parameterTest[3]))
};


extern int test_TranslatorLOG_run_translateSignalCalibrationPosition(void) {
    return cmocka_run_group_tests_name("Test of the module translatorLOG for function TranslatorLOG_signalCalibrationPosition", tests, NULL, NULL);
}

static void test_TranslatorLOG_signalCalibrationPosition(void** state) {
    ParameterTestCalibrationPosition* parameter = (ParameterTestCalibrationPosition*) *state;

    CalibrationPositionId result = TranslatorLOG_translateForSignalCalibrationPosition(parameter->trameInput);

    assert_int_equal(parameter->idExpected, result);
}
