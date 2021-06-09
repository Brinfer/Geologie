
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include "cmocka.h"

/** Nombre de suites de tests a excuter. */
#define NB_SUITE_TESTS (2)

extern int translatorLOG_run_tests(void);
extern int mathematician_run_tests(void);

/** Liste des suites de tests a excuter. */
static int (*suite_tests[])(void) = {
    mathematician_run_tests,
    translatorLOG_run_tests
};

/**
 * Fonction principal du programme de test.
 */
int main(int argc, char* argv[]) {
    for (int i = 0; i < NB_SUITE_TESTS; i++) {
        suite_tests[i]();
    }

    return EXIT_SUCCESS;
}
