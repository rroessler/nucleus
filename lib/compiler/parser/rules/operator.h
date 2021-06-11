#ifndef NUC_OPERATORS_H
#define NUC_OPERATORS_H

// Nucleus Headers
#include "../../emit.h"
#include "../parser.h"
#include "precedence.h"

// forward declaration
static nuc_ParseRule* parser_getRule(TokenType type);

/**
 * Forces a given operation. This method is GREAT for forcing internal comparisons,
 * and math on the fly (especially for loop iterators).
 */
static void parser_forceOperation(TokenType op) {
    // and now emit an operation based on the operator
    switch (op) {
        CASE_EMIT(T_PLUS, OP_ADD);
        CASE_EMIT(T_MINUS, OP_SUB);
        CASE_EMIT(T_STAR, OP_MUL);
        CASE_EMIT(T_SLASH, OP_DIV);
        CASE_EMIT(T_PERCENTAGE, OP_MOD);
        CASE_EMIT(T_HAT, OP_XOR);
        CASE_EMIT(T_POW, OP_POW);
        CASE_EMIT(T_BITW_AND, OP_BITW_AND);
        CASE_EMIT(T_BITW_OR, OP_BITW_OR);
        CASE_EMIT(T_ROL, OP_ROL);
        CASE_EMIT(T_ROR, OP_ROR);

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
static void rule_binary(bool canAssign) {
    // get the current operator, and required parsing rule
    TokenType op = parser.previous.type;
    nuc_ParseRule* rule = parser_getRule(op);
    fuser_parsePrecedence((Precedence)(rule->prec + 1));

    // and now emit an operation based on the operator
    parser_forceOperation(op);
}

/** Parses a unary expression. */
static void rule_unary(bool canAssign) {
    // retrieve the operation being completed
    TokenType op = parser.previous.type;

    // compile the operand
    fuser_parsePrecedence(P_UNARY);

    // and emit the unary operation
    switch (op) {
        CASE_EMIT(T_MINUS, OP_NEGATE);
        CASE_EMIT(T_BANG, OP_NOT);
        CASE_EMIT(T_BITW_NOT, OP_BITW_NOT);

        default:  // unreachable
            return;
    }
}

#endif