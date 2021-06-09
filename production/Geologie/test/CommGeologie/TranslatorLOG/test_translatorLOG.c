
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

/** Nombre de suites de tests a excuter. */
#define NB_SUITE_TESTS_TRANSLATOR_LOG (1)

extern int test_TranslatorLOG_run_translateTrameToHeader(void);

/** Liste des suites de tests a excuter. */
static int (*suite_tests[])(void) = {
   test_TranslatorLOG_run_translateTrameToHeader
};

/**
 * Fonction principal du programme de test.
 */
extern int translatorLOG_run_tests() {
    for (int i = 0; i < NB_SUITE_TESTS_TRANSLATOR_LOG; i++) {
        suite_tests[i]();
    }

    return EXIT_SUCCESS;
}
