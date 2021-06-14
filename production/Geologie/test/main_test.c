
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "../src/tools.h"

#include "cmocka.h"

/**
 * @brief Nombre de suites de tests a excuter.
 */
#define NB_SUITE_TESTS (4)

/**
 * @brief Fonction lançant la suite des tests pour TranslatorLOG.
 *
 * @return int32_t 0 en cas de succee ou le numero du test qui a echoue.
 */
extern int32_t translatorLOG_run_tests(void);

/**
 * @brief Lance la suite de test du module TranslatorBeacon.
 *
 * @return int32_t 0 en cas de succee ou le numero du test qui a echoue.
 */
extern int32_t translatorBeacon_run_tests(void);

/**
 * @brief Lance la suite de test du module Scanner.
 *
 * @return int32_t 0 en cas de succee ou le numero du test qui a echoue.
 */
extern int scanner_run_tests(void);

extern int geographer_run_tests(void);

/**
 * @brief Liste des suites de tests a excuter.
 */
static int32_t (*suite_tests[])(void) = {
    scanner_run_tests,
    geographer_run_tests
};

/**
 * @brief Fonction principal du programme de test.
 *
 * @retval int32_t 0.
 */
int32_t main(int argc, char* argv[]) {
    for (int i = 0; i < NB_SUITE_TESTS; i++) {
        suite_tests[i]();
    }

    return EXIT_SUCCESS;
}
