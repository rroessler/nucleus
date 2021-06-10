#ifndef NUC_STATEMENT_H
#define NUC_STATEMENT_H

// Nucleus Headers
#include "../expression.h"
#include "../rules/precedence.h"
#include "block.h"
#include "if.h"
#include "loop.h"
#include "return.h"

/** Parses an expression statement. */
static void nuc_expressionStatement() {
    EXPRESSION;
    CONSUME(T_SEMICOLON, "Expected ';' after an expression.");
    EMIT_BYTE(OP_POP);
}

/** Compiles a Nucleus Statement */
static void nuc_statement() {
    if (MATCH(T_LEFT_BRACE)) {  // if we see a left brace, most likely a code block from here
        fuser_beginScope();
        fuser_block();  // gets enclosed within it's own scope
        fuser_endScope();
    } else if (MATCH(T_IF)) {  // if (cond) { code... }
        fuser_ifStatement();
    } else if (MATCH(T_REPEAT_IF)) {  // repif (cond) { code... }
        fuser_whileStatement();
    } else if (MATCH(T_DO)) {  // do { code } repif (cond)
        fuser_doStatement();
    } else if (MATCH(T_FOR)) {  // for (num loopVar : start, end, incrementor?, comparator?) { cond... }
        fuser_forStatement();
    } else if (MATCH(T_RETURN)) {
        fuser_returnStatement();
    } else {  // default is an expression statement
        nuc_expressionStatement();
    }
}

#endif