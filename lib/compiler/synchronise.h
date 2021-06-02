#ifndef NUC_SYNCHRONISE_H
#define NUC_SYNCHRONISE_H

// Nucleus Headers
#include "parser.h"

/** Synchronises the parser out of panicing. */
static void synchronise() {
    parser.panicMode = false;  // set the panic mode to false

    // iterate until either at ease of end of file
    while (parser.current.type != T_EOF) {
        if (parser.previous.type == T_SEMICOLON) return;  // at ease again

        // depending on the given token
        switch (parser.current.type) {
            case T_LET:
            case T_MODEL:
            case T_REACTION:
            case T_FOR:
            case T_DO:
            case T_REPEAT_IF:
            case T_IF:
            case T_RETURN:  // all these items will return the parser to ease
                return;
            default:;
        }

        // and advance to the next token
        advance();
    }
}

#endif