#ifndef NUC_IMMEDIATE_H
#define NUC_IMMEDIATE_H

// C Standard Library
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// Nucleus Headers
#include "../../common.h"
#include "codes.h"

/*******************
 *  HELPER MACROS  *
 *******************/

/**
 * Immediately exits a Nucleus program instance with a given error code.
 * @param code              Error code.
 * @param format            `fprintf` format string.
 */
inline static void nuc_immediateExit(uint8_t code, const char* format, ...) {
    fputc('\n', stderr);       // pad display
    nuc_printErrorCode(code);  // display the exit code

    // coordinate the variable format arguments
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n\n", stderr);

    // and exit
    exit(code);
}

#endif