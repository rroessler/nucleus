#ifndef NUC_UNARY_H
#define NUC_UNARY_H

// Nucleus Headers
#include "../emit.h"
#include "../parser.h"
#include "../precedence.h"

/** Parses a unary expression. */
static void unary(bool canAssign) {
    // retrieve the operation being completed
    TokenType op = parser.previous.type;

    // compile the operand
    parsePrecedence(P_UNARY);

    // and emit the unary operation
    switch (op) {
        CASE_EMIT(T_MINUS, OP_NEGATE);
        CASE_EMIT(T_BANG, OP_NOT);

        default:  // unreachable
            return;
    }
}

#endif