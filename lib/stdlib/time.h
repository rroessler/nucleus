#ifndef NUC_STDLIB_TIME_H
#define NUC_STDLIB_TIME_H

// C Standard Library
#include <stdint.h>
#include <sys/time.h>
#include <time.h>

// Nucleus Headers
#include "../particle/value.h"

// TIME DEFINES
#define MILLION 1000000L

/** Returns the current time from program inception as a Nucleus Numeric. */
static Particle nuc_std__time_clock(int argCount, Particle* args) {
    return NUC_NUMBER(MILLION * (double)clock() / CLOCKS_PER_SEC);
}

/** Returns the current time since epoch as a Nucleus Numeric. */
static Particle nuc_std__time_now(int argCount, Particle* args) {
    // initialise some values
    struct timeval tv;
    gettimeofday(&tv, NULL);

    /** Gets the time since epoch in (...) seconds */
    uint64_t microSinceEpoch = (uint64_t)(tv.tv_sec) * MILLION +
                               (uint64_t)(tv.tv_usec);

    // and return as a nucleus number
    return NUC_NUMBER((double)microSinceEpoch);
}

#endif