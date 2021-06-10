#ifndef NUC_EXIT_CODES_H
#define NUC_EXIT_CODES_H

// C Standard Library
#include <stdio.h>

// Nucleus Headers
#include "../../common.h"

// Nucleus Exit Codes
#define NUC_EXIT_SUCCESS 0  // generic success
#define NUC_EXIT_FAILURE 1  // generic failure

// External Errors
#define NUC_EXIT_CMD 64     // command line errors
#define NUC_EXIT_SYNTAX 65  // ".nuc" file errors
#define NUC_EXIT_TYPE 66    // type errors
#define NUC_EXIT_ARG 67     // invalid argument error
#define NUC_EXIT_RANGE 68   // range error (indexing / other)
#define NUC_EXIT_REF 69     // reference error, missing or invalid reference
#define NUC_EXIT_IO 70      // input / output error

// Internal Errors
#define NUC_EXIT_MEM 80       // memory error
#define NUC_EXIT_OVERFLOW 81  // frame/ overflow error
#define NUC_EXIT_INTERNAL 82  // internally bad errors
#define NUC_EXIT_UNIMP 90     // unimplemented error

// Reserved Exit Codes
#define NUC_EXIT_PERM 126        // permission error
#define NUC_EXIT_SIG(n) 128 + n  // SIG exit
#define NUC_EXIT_TERM 130        // control-c termination

/*******************
 *  HELPER MACROS  *
 *******************/

/**
 * Emits a switch case statement for error codes.
 * @param code              Error Code.
 * @param str               Associated Error Message
 */
#define CASE_ERROR_CODE(code, str) \
    case code:                     \
        fputs(str ": ", stderr);   \
        break

/************************
 *  ERROR CODE METHODS  *
 ************************/

/**
 * Displays a printable error definition based on the given exit code.
 * @param code              Error code.
 */
inline static void nuc_printErrorCode(uint8_t code) {
    fputs("\x1b[1;31m", stderr);  // bold + red
    switch (code) {
        CASE_ERROR_CODE(NUC_EXIT_FAILURE, "Error");
        CASE_ERROR_CODE(NUC_EXIT_CMD, "Command Error");
        CASE_ERROR_CODE(NUC_EXIT_SYNTAX, "Syntax Error");
        CASE_ERROR_CODE(NUC_EXIT_TYPE, "Type Error");
        CASE_ERROR_CODE(NUC_EXIT_ARG, "Argument Error");
        CASE_ERROR_CODE(NUC_EXIT_RANGE, "Range Error");
        CASE_ERROR_CODE(NUC_EXIT_REF, "Reference Error");
        CASE_ERROR_CODE(NUC_EXIT_IO, "IO Error");
        CASE_ERROR_CODE(NUC_EXIT_MEM, "Memory Error");
        CASE_ERROR_CODE(NUC_EXIT_UNIMP, "Unimplemented Error");
        CASE_ERROR_CODE(NUC_EXIT_PERM, "Permission Error");

        default:  // in the default case, ignore displaying
            break;
    }
    fputs("\x1b[0m", stderr);  // and cap off chalk
}

// undefining macros to restrict use
#undef CASE_ERROR_CODE

#endif