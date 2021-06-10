/**
 * @file test_translatorLOG_translateTrameToHeader.c
 *
 * @brief Ensemble de test pour tester TranslatorLOG_translateForSendCurrentPosition.
 *
 * @version 2.0
 * @date 09-06-2021
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
 * @brief La taille d'une #Position en octet.
 */
#define SIZE_POSITION (8)

/**
 * @brief La taille d'une #Date en octet.
 */
#define SIZE_TIMESTAMP (4)

/**
 * @brief Structure passee aux fonctions tests.
 */
typedef struct {
    Trame trameExpected[SIZE_HEADER + SIZE_POSITION + SIZE_TIMESTAMP];  /**< La #Trame attendue en resultat de TranslatorLOG_translateForSendCurrentPosition */
    Position positionInput;                                             /**< La #Position passee a TranslatorLOG_translateForSendCurrentPosition */
    Date dateInput;                                                     /**< La #Date passee a TranslatorLOG_translateForSendCurrentPosition */
} ParameterTestCurrentPosition;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Ensemble des donnees de tests.
 */
static ParameterTestCurrentPosition parameterTest[] = {
    {
        .positionInput = { .X = 0, .Y = 0},
        .dateInput = 0,
        .trameExpected = {
            // Header
            SEND_CURRENT_POSITION,          // CMD
            0x00, 0x0C,                     // Size - 12

            // Data
            0x00, 0x00, 0x00, 0x00,         // TimeStamp
            0x00, 0x00, 0x00, 0x00,         // Position X
            0x00, 0x00, 0x00, 0x00,         // Position Y
        }
    },
    {
        .positionInput = { .X = 4294967295, .Y = 4294967295},
        .dateInput = 4294967295,
        .trameExpected = {
            // Header
            SEND_CURRENT_POSITION,          // CMD
            0x00, 0x0C,                     // Size - 12

            // Data
            0xFF, 0xFF, 0xFF, 0xFF,         // TimeStamp
            0xFF, 0xFF, 0xFF, 0xFF,         // Position X
            0xFF, 0xFF, 0xFF, 0xFF,         // Position Y
        }
    },
    {
        .positionInput = { .X = 2863311530, .Y = 1431655765},
        .dateInput = 2779096485,
        .trameExpected = {
            // Header
            SEND_CURRENT_POSITION,          // CMD
            0x00, 0x0C,                     // Size - 12

            // Data
            0xA5, 0xA5, 0xA5, 0xA5,         // TimeStamp
            0xAA, 0xAA, 0xAA, 0xAA,         // Position X
            0x55, 0x55, 0x55, 0x55,         // Position Y
        }
    },
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Execute les tests de TranslatorLOG_translateForSendCurrentPosition.
 *
 * @return int 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int test_TranslatorLOG_run_translateForSendCurrentPosition(void);

/**
 * @brief La fonction test permettant de verifier le bon fonctionnement de TranslatorLOG_translateForSendCurrentPosition.
 *
 * @param state Les donnees de test #ParameterTestCurrentPosition.
 */
static void test_TranslatorLOG_translateForSendCurrentPosition(void** state);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Ensemble des tests a executer.
 */
static const struct CMUnitTest testsCurrentPosition[] = {
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendCurrentPosition, &(parameterTest[0])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendCurrentPosition, &(parameterTest[1])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendCurrentPosition, &(parameterTest[2])),
};


extern int test_TranslatorLOG_run_translateForSendCurrentPosition(void) {
    return cmocka_run_group_tests_name("Test of the module translatorLOG for function TranslatorLOG_translateForSendCurrentPosition", testsCurrentPosition, NULL, NULL);
}

static void test_TranslatorLOG_translateForSendCurrentPosition(void** state) {
    ParameterTestCurrentPosition* parameter = (ParameterTestCurrentPosition*) *state;

    /* Test trame sizeResult */
    uint16_t sizeResult = TranslatorLOG_getTrameSize(SEND_CURRENT_POSITION, 0);
    assert_int_equal(SIZE_HEADER + SIZE_POSITION + SIZE_TIMESTAMP, sizeResult);

    Trame currentResult[sizeResult];
    TranslatorLOG_translateForSendCurrentPosition(&(parameter->positionInput), parameter->dateInput, currentResult);

    /* Test trame */
    assert_memory_equal(parameter->trameExpected, currentResult, sizeResult);
}
