#ifndef NUC_CATCH_STATEMENT_H
#define NUC_CATCH_STATEMENT_H

// Nucleus Headers
#include "../../emit.h"

// FORWARD DECLARATION
static void nuc_statement();

/** Compiles a Nucleus TRY-CATCH block */
static void fuser_catchStatement() {
    EMIT_BYTE(OP_CATCH_MODE);                  // set catch mode operation
    int catchJump = EMIT_JUMP(OP_JUMP_CATCH);  // save a catch jump
    nuc_statement();                           // parse the try statement

    int finallyJump = EMIT_JUMP(OP_JUMP);  // try succeeded jump
    EMIT_BYTE(OP_END_CATCH_MODE);          // end catching mode
    PATCH_JUMP(catchJump);                 // patch start of catch area

    // now begin catch area
    CONSUME(T_CATCH, "Expected coinciding 'catch' to 'try' block.");
    CONSUME(T_LEFT_PAREN, "Expected '(' after 'catch' keyword.");
    fuser_beginScope();

    /** SET UP ERROR VARIABLE HERE */
    uint16_t global = fuser_parseVariable("Expected a disruption variable name.", NUC_IMMUTABLE);
    fuser_defineVariable(global);

    CONSUME(T_RIGHT_PAREN, "Expected ')' after 'catch' disruption variable.");
    CONSUME(T_LEFT_BRACE, "Expected '{' to begin 'catch' block.");
    fuser_block();     // begin block
    fuser_endScope();  // and end the catch scope

    // continue you on as desired by the user
    PATCH_JUMP(finallyJump);  // patch ending to skip if no catch occured
}

#endif