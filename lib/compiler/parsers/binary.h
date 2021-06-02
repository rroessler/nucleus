#ifndef NUC_BINARY_H
#define NUC_BINARY_H

// Nucleus Headers
#include "../emit.h"
#include "../parser.h"
#include "../precedence.h"

// forward declaration
static ParseRule* getRule(TokenType type);

/**
 * Forces a given operation. This method is GREAT for forcing internal comparisons,
 * and math on the fly (especially for loop iterators).
 */
static void forceOperation(TokenType op) {
    // and now emit an operation based on the operator
    switch (op) {
        CASE_EMIT(T_PLUS, OP_ADD);
        CASE_EMIT(T_MINUS, OP_SUB);
        CASE_EMIT(T_STAR, OP_MUL);
        CASE_EMIT(T_SLASH, OP_DIV);

        CASE_EMIT_SHORT(T_BANG_EQUAL, OP_EQUAL, OP_NOT);
        CASE_EMIT(T_EQUAL_EQUAL, OP_EQUAL);
        CASE_EMIT(T_GREATER, OP_GREATER);
        CASE_EMIT_SHORT(T_GREATER_EQUAL, OP_LESS, OP_NOT);
        CASE_EMIT(T_LESS, OP_LESS);
        CASE_EMIT_SHORT(T_LESS_EQUAL, OP_GREATER, OP_EQUAL);

        default:  // unreachable
            return;
    }
}

/** Parses a Binary Expression */
static void binary(bool canAssign) {
    // get the current operator, and required parsing rule
    TokenType op = parser.previous.type;
    ParseRule* rule = getRule(op);
    parsePrecedence((Precedence)(rule->prec + 1));

    // and now emit an operation based on the operator
    forceOperation(op);
}

#endif