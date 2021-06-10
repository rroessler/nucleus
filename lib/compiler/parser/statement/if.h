#ifndef NUC_IF_STATEMENT_H
#define NUC_IF_STATEMENT_H

// Nucleus Headers
#include "../../emit.h"

// FORWARD DECLARATIONS
static void nuc_statement();

/** Compiles a Nucleus IF Statement */
static void fuser_ifStatement() {
    CONSUME(T_LEFT_PAREN, "Expected '(' after 'if' keyword.");
    EXPRESSION;
    CONSUME(T_RIGHT_PAREN, "Expected ')' after condition.");

    // coordinate the start of a jump
    int thenJump = EMIT_JUMP(OP_JUMP_IF_FALSE);
    EMIT_BYTE(OP_POP);
    nuc_statement();

    // set up a else jump
    int elseJump = EMIT_JUMP(OP_JUMP);

    // and patch the jump after compiling
    PATCH_JUMP(thenJump);
    EMIT_BYTE(OP_POP);

    // and end with 'else'
    if (MATCH(T_ELSE)) nuc_statement();
    PATCH_JUMP(elseJump);  // clean up else jump
}

#endif