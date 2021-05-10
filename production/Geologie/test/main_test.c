
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

/** Nombre de suites de tests à exécuter. */
#define NB_SUITE_TESTS 1

extern int translator_run_tests(void);

/** Liste des suites de tests à exécuter. */
int (*suite_tests[])(void) = {
    translator_run_tests
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
