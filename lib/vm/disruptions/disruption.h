#ifndef NUC_DISRUPTION_H
#define NUC_DISRUPTION_H

// C Standard Library
#include <stdarg.h>
#include <stdio.h>

// Nucleus Headers
#include "../../common.h"
#include "../../utils/macros.h"
#include "../core/core.h"
#include "../core/flags.h"
#include "../global.h"
#include "immediate.h"

/*******************
 *  HELPER MACROS  *
 *******************/

// init variable args
#define NUC_INIT_VA_ARGS \
    va_list args;        \
    va_start(args, format)

// prints a variable format string
#define NUC_VA_FPRINT               \
    vfprintf(stderr, format, args); \
    fputc('\n', stderr)

// set a buffer length
#define NUC_ERROR_BUFFER_LEN 512

// define a suitable frame trace length
#define NUC_ERROR_LOOKBACK_FRAMES 8

// allows calling an item as unimplemented
#define NUC_UNIMP(msg, ...) atomizer_runtimeError(NUC_EXIT_UNIMP, msg NUC_VA_ARGS(__VA_ARGS__))

// allows calling an item as unimplemented and returning false
#define NUC_UNIMP_RETURN(msg, ...)                                       \
    atomizer_runtimeError(NUC_EXIT_UNIMP, msg NUC_VA_ARGS(__VA_ARGS__)); \
    return false

/************************
 *  DISRUPTION METHODS  *
 ************************/

/**
 * Patches requests between `atomizer_catchableError` and `atomizer_runtimeError`.
 * @param code                      Error code.
 * @param format                    Format string for error.
 * @param args                      Variadic format string args.
 */
static inline void __atomizer_errorDisplay(uint8_t code, const char* format, va_list args) {
    nuc_printErrorCode(code);  // display the error code
    NUC_VA_FPRINT;             // and print the args
}

// helper macro to allow `runtime` and `catchable` to both print variable args
#define atomizer_errorDisplay(code, format, ...)        \
    fputc('\n', stderr);                                \
    __atomizer_errorDisplay(code, format, __VA_ARGS__); \
    va_end(args)

/**
 * Chucks a runtime error as suitable.
 * @param code                      Error code.
 * @param format                    Error format string.
 */
static void atomizer_runtimeError(uint8_t code, const char* format, ...) {
    if (format[0] != '\0') {  // only display if not empty
        NUC_INIT_VA_ARGS;
        atomizer_errorDisplay(code, format, args);
    }

    // determine a suitable frame trace
    int endFrame = atomizer.frameCount > NUC_ERROR_LOOKBACK_FRAMES
                       ? (atomizer.frameCount - NUC_ERROR_LOOKBACK_FRAMES)
                       : 0;

    // and iterate over the final frames for a call trace
    for (int i = atomizer.frameCount - 1; i >= endFrame; i--) {
        nuc_CallFrame* frame = &atomizer.frames[i];
        nuc_ObjReaction* reaction = frame->closure->reaction;
        size_t inst = frame->ip - reaction->chunk.code - 1;

        // retrieve some items for displaying the called line
        size_t line = reaction->chunk.lines[inst];
        const char* source = lexer_getLine(line);

        fprintf(stderr, "[\x1b[2mline\x1b[0m \x1b[33m%lu\x1b[0m] ", line);
        if (reaction->name == NULL) {
            fprintf(stderr, "in \x1b[32mscript\x1b[0m.");
        } else {
            fprintf(stderr, "at \x1b[3;31m\"%s()\"\x1b[0m.", reaction->name->chars);
        }

        // and now printing a TRIMMED source
        fprintf(stderr, "\n[\x1b[2;36msource\x1b[0m] \x1b[36m`");
        char* ptr = NULL;
        while (isspace((unsigned)*source)) source++;     // chomp at start of source
        ptr = (char*)source + strlen(source) - 1;        // jump to the last char of source
        while (isspace((unsigned)*ptr)) ptr--;           // trim the end of the string
        while (source <= ptr) fputc(*source++, stderr);  // and
        fprintf(stderr, "`\x1b[0m\n");
    }
    fputc('\n', stderr);

    // clean the atomizer stack
    atomizer_resetStack();
    NUC_SET_AFLAG(NUC_AFLAG_DISRUPTED);  // note as disrupted
    atomizer.exitCode = code;            // and save the exit code
}

/**
 * If the atomizer is in a catchable state, then errors can be caught and notified.
 * @param code                      Error code.
 * @param format                    Error format string.
 */
static void atomizer_catchableError(uint8_t code, const char* format, ...) {
    NUC_INIT_VA_ARGS;

    // if the atomizer is not in a catchable state, then disrupt
    if (!NUC_CHECK_AFLAG(NUC_AFLAG_DISRUPTION_CATCHABLE)) {
        atomizer_errorDisplay(code, format, args);
        atomizer_runtimeError(code, "");
        return;
    }

    // read the format string to a buffer
    char buffer[NUC_ERROR_BUFFER_LEN];
    vsnprintf(buffer, NUC_ERROR_BUFFER_LEN, format, args);
    va_end(args);

    // and reallocate the buffer to an appropriate size
    // size_t length = strlen(buffer);
    // const char* errStr = (char*)realloc(&buffer, length);

    // now can play with the error as needed
    nuc_immediateExit(NUC_EXIT_UNIMP, "Catchable disruptions have not been implemented yet.");
}

#endif