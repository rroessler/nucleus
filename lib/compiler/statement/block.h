#ifndef NUC_BLOCK_STATEMENT_H
#define NUC_BLOCK_STATEMENT_H

// forward declaration
static void declaration();

/** Parses a Nucleus Code Block */
static void block() {
    while (!check(T_RIGHT_BRACE) && !check(T_EOF)) declaration();
    consume(T_RIGHT_BRACE, "Expected '}' after code block.");
}

#endif