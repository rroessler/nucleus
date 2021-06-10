#ifndef NUC_RETURN_STATEMENT_H
#define NUC_RETURN_STATEMENT_H

// Nucleus Headers
#include "../../emit.h"
#include "../../global.h"

/** Compiles a return statement. */
static void fuser_returnStatement() {
    if (current->type == RT_SCRIPT) PARSER_ERROR_AT("Cannot return from top-level code.");

    if (MATCH(T_SEMICOLON)) {
        EMIT_RET;
    } else {
        // ensure valid return
        if (current->type == RT_INITIALISER) PARSER_ERROR_AT("Cannot return a value from a model constructor.");

        EXPRESSION;
        CONSUME(T_SEMICOLON, "Expected ';' after return value.");
        EMIT_BYTE(OP_RETURN);
    }
}

#endif