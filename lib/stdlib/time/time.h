#ifndef NUC_STDLIB_TIME_H
#define NUC_STDLIB_TIME_H

#ifdef NUC_NTVDEF_TIME

    // C Standard Library
    #include <stdint.h>
    #include <sys/time.h>
    #include <time.h>

    // Nucleus Headers
    #include "../helpers.h"

    /**********************
     *  TIME DEFINITIONS  *
     **********************/

    // to help with conversions
    #define MILLION 1000000L

/******************
 *  TIME NATIVES  *
 ******************/

/** Returns the current time from program inception as a Nucleus Numeric. */
NUC_NATIVE_WRAPPER(
    time,                                                        // parent
    clock,                                                       // native name
    return NUC_NUM(MILLION * (double)clock() / CLOCKS_PER_SEC))  // method

    /*************
     *  EXPORTS  *
     *************/

    // exports all the TIME methods
    #define NUC_STDLIB__TIME_NATIVES \
        { "std.time.clock", nuc_time__clock }

    /***************
     *  UNDEFINES  *
     ***************/

    #undef MILLION

#endif

#endif