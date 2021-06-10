/**
 * @file test_translatorLOG_translateForSendMemoryAndProcessorLoad.c
 *
 * @brief Ensemble de test pour tester TranslatorLOG_translateForSendMemoryAndProcessorLoad.
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief La taille de la partie Data du message
 */
#define SIZE_MEMORY_AND_PROCESSOR_LOAD (12)

/**
 * @brief Structure passee aux fonctions tests.
 */
typedef struct {
    ProcessorAndMemoryLoad processorAndMemoryLoadInput; /**< La charge processeur et memoire passee en entree. */
    Date dateInput;                                     /**< La date passee en entree. */
    Trame trameExpected[15];                            /**< La trame attendue en resultat */
} ParameterTestProcessorAndMemoryLoad;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Variable et structure extern
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Ensemble des donnees de tests.
 */
static ParameterTestProcessorAndMemoryLoad parameterTest[] = {
    {
        .processorAndMemoryLoadInput = {.memoryLoad = 0, .processorLoad = 0},
        .dateInput = 0,
        .trameExpected = {
            SEND_MEMORY_PROCESSOR_LOAD, // CMD
            0x00, 0x0C,                 // SIZE

            0x00, 0x00, 0x00, 0x00,     // Timestamp
            0x00, 0x00, 0x00, 0x00,     // Memory
            0x00, 0x00, 0x00, 0x00,     // Processor
        }
    },
    {
        .processorAndMemoryLoadInput = {.memoryLoad = 55.55, .processorLoad = 25.25},
        .dateInput = 2295105740,
        .trameExpected = {
            SEND_MEMORY_PROCESSOR_LOAD, // CMD
            0x00, 0x0C,                 // SIZE

            0x88, 0xCC, 0x88, 0xCC,     // Timestamp
            0x42, 0x5E, 0x33, 0x33,     // Memory
            0x41, 0xCA, 0x00, 0x00,     // Processor
        }
    },
    {
        .processorAndMemoryLoadInput = {.memoryLoad = 100, .processorLoad = 75.75},
        .dateInput = 2857740885,
        .trameExpected = {
            SEND_MEMORY_PROCESSOR_LOAD, // CMD
            0x00, 0x0C,                 // SIZE

            0xAA, 0x55, 0xAA, 0x55,     // Timestamp
            0x42, 0xC8, 0x00, 0x00,     // Memory
            0x42, 0x97, 0x80, 0x00,     // Processor
        }
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Prototypes de fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Execute les tests de TranslatorLOG_translateForSendMemoryAndProcessorLoad.
 *
 * @return int 0 en cas de succes, le numero du test qui a echoue sinon.
 */
extern int test_TranslatorLOG_run_translateForSendMemoryAndProcessorLoad(void);

/**
 * @brief La fonction test permettant de verifier le bon fonctionnement de TranslatorLOG_translateForSendMemoryAndProcessorLoad.
 *
 * @param state Les donnees de test #ParameterTestCurrentPosition.
 */
static void test_TranslatorLOG_translateForSendMemoryAndProcessorLoad(void** state);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Fonctions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const struct CMUnitTest testsMemoryAndProcessorLoad[] = {
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendMemoryAndProcessorLoad, &(parameterTest[0])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendMemoryAndProcessorLoad, &(parameterTest[1])),
    cmocka_unit_test_prestate(test_TranslatorLOG_translateForSendMemoryAndProcessorLoad, &(parameterTest[2])),
};

extern int test_TranslatorLOG_run_translateForSendMemoryAndProcessorLoad(void) {
    return cmocka_run_group_tests_name("Test of the module translatorLOG for function TranslatorLOG_translateForSendMemoryAndProcessorLoad", testsMemoryAndProcessorLoad, NULL, NULL);
}

static void test_TranslatorLOG_translateForSendMemoryAndProcessorLoad(void** state) {
    ParameterTestProcessorAndMemoryLoad* parameter = (ParameterTestProcessorAndMemoryLoad*) *state;

    uint16_t sizeResult = TranslatorLOG_getTrameSize(SEND_MEMORY_PROCESSOR_LOAD, 0);
    assert_int_equal(SIZE_HEADER + SIZE_MEMORY_AND_PROCESSOR_LOAD, sizeResult);

    Trame trameResult[sizeResult];
    TranslatorLOG_translateForSendMemoryAndProcessorLoad(&(parameter->processorAndMemoryLoadInput), parameter->dateInput, trameResult);

    assert_memory_equal(parameter->trameExpected, trameResult, sizeResult);
}
