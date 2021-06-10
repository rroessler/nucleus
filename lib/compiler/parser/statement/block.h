#ifndef NUC_BLOCK_STATEMENT_H
#define NUC_BLOCK_STATEMENT_H

// Nucleus Headers
#include "../parser.h"

// forward declaration
static void nuc_declaration();

/** Parses a Nucleus Code Block */
static void fuser_block() {
    while (!CHECK(T_RIGHT_BRACE) && !CHECK(T_EOF)) nuc_declaration();
    CONSUME(T_RIGHT_BRACE, "Expected '}' after code block.");
}

#endif