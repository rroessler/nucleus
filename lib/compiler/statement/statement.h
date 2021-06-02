#ifndef NUC_STATEMENT_H
#define NUC_STATEMENT_H

// Nucleus Headers
#include "block.h"
#include "if.h"
#include "loop.h"
#include "return.h"

/** Parses an expression statement. */
static void expressionStatement() {
    expression();
    consume(T_SEMICOLON, "Expected ';' after an expression.");
    emitByte(OP_POP);
}

/** Compiles a Nucleus Statement */
static void statement() {
    if (match(T_LEFT_BRACE)) {  // if we see a left brace, most likely a code block from here
        fuser_beginScope();
        block();  // gets enclosed within it's own scope
        fuser_endScope();
    } else if (match(T_IF)) {  // if (cond) { code... }
        ifStatement();
    } else if (match(T_REPEAT_IF)) {  // repif (cond) { code... }
        whileStatement();
    } else if (match(T_DO)) {  // do { code } repif (cond)
        doStatement();
    } else if (match(T_FOR)) {  // for (num loopVar : start, end, incrementor?, comparator?) { cond... }
        forStatement();
    } else if (match(T_RETURN)) {
        returnStatement();
    } else {  // default is an expression statement
        expressionStatement();
    }
}

#endif