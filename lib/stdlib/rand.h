#ifndef NUC_STDLIB_RAND_H
#define NUC_STDLIB_RAND_H

// C Standard Library
#include <stdlib.h>
#include <time.h>

// Nucleus Headers
#include "../particle/value.h"

// forward declaration
static void atomizer_runtimeError(const char* format, ...);

/** Pre-Initialise SRAND */
static inline void nuc_initRandom() {
    srand((unsigned)time(NULL));
    rand();  // wasting some rand()
    rand();  // results as a cheap hack
    rand();  // to force initial variety
}

/****************
 *  RANDOM API  *
 ****************/

// sets a new seed for randomness
static inline Particle nuc_rand__seed(int argCount, Particle* args) {
    if (argCount < 1) {
        atomizer_runtimeError("\"seed\" expected 1 argument.");
        return NUC_NULL;
    }

    if (!IS_NUMBER(args[0])) {
        atomizer_runtimeError("\"seed\" expected a numeric argument.");
        return NUC_NULL;
    }

    srand(AS_NUMBER(args[0]));
    return NUC_NULL;
}

// naively uses the internal random value and floatifies
static inline Particle nuc_rand__float(int argCount, Particle* args) {
    double scale = rand() / (double)RAND_MAX;
    if (argCount == 0) return NUC_NUMBER(scale);

    if (!IS_NUMBER(args[0])) {
        atomizer_runtimeError("\"seed\" expected a numeric argument.");
        return NUC_NULL;
    }

    if (argCount == 1) return NUC_NUMBER(scale * AS_NUMBER(args[0]));

    if (!IS_NUMBER(args[1])) {
        atomizer_runtimeError("\"seed\" expected a numeric argument.");
        return NUC_NULL;
    }
    double min = AS_NUMBER(args[1]);
    return NUC_NUMBER(min + scale * (AS_NUMBER(args[0]) - min));
}

// generates a naive integer random number
static inline Particle nuc_rand(int argCount, Particle* args) {
    return NUC_NUMBER(rand());
}

#endif
