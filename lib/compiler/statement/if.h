#ifndef NUC_IF_STATEMENT_H
#define NUC_IF_STATEMENT_H

// Nucleus Headers
#include "../emit.h"

// forward declaration
static void expression();
static void statement();

/** Compiles a Nucleus IF Statement */
static void ifStatement() {
    consume(T_LEFT_PAREN, "Expected '(' after 'if' keyword.");
    expression();
    consume(T_RIGHT_PAREN, "Expected ')' after condition.");

    // coordinate the start of a jump
    int thenJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    statement();

    // set up a else jump
    int elseJump = emitJump(OP_JUMP);

    // and patch the jump after compiling
    patchJump(thenJump);
    emitByte(OP_POP);

    // and end with 'else'
    if (match(T_ELSE)) statement();
    patchJump(elseJump);  // clean up else jump
}

#endif