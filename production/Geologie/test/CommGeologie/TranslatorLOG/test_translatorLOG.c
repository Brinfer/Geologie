
#include <stdlib.h>

/** Nombre de suites de tests a excuter. */
#define NB_SUITE_TESTS_TRANSLATOR_LOG (2)

extern int test_TranslatorLOG_run_translateTrameToHeader(void);
extern int test_TranslatorLOG_run_translateForSendCurrentPosition(void);

/** Liste des suites de tests a excuter. */
static int (*suite_tests[])(void) = {
   test_TranslatorLOG_run_translateTrameToHeader,
   test_TranslatorLOG_run_translateForSendCurrentPosition
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
