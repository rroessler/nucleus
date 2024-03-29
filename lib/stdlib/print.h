#ifndef NUC_STDLIB_PRINT_H
#define NUC_STDLIB_PRINT_H

// Nucleus Headers
#include "../particle/print.h"

// FORWARD DECLARATION
void particle_print(nuc_Particle value, bool prettify);

/** Prints all given Nucleus Values. */
static inline nuc_Particle nuc_std__print(int argCount, nuc_Particle* args) {
    if (argCount > 1) {
        for (int i = 0; i < argCount; i++) {
            particle_print(args[i], false);  // do not prettify
        }
    } else if (argCount == 1) {
        particle_print(args[0], !IS_STRING(args[0]));  // want to pretty print the value
    }

    // and cap off with newline character
    putchar('\n');

    // does not return anything
    return NUC_NULL;
}

#endif