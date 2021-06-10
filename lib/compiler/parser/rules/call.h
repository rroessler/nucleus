#ifndef NUC_CALL_H
#define NUC_CALL_H

// Nucleus Headers
#include "../../emit.h"
#include "../parser.h"

/** Compiles given Reaction call arguments. */
static uint8_t fuser_argumentList() {
    uint8_t argCount = 0;
    if (!CHECK(T_RIGHT_PAREN)) {
        do {
            EXPRESSION;
            if (argCount == UINT8_MAX) PARSER_ERROR_AT("Cannot have more than 255 arguments in reaction call.");
            argCount++;
        } while (MATCH(T_COMMA));
    }

    CONSUME(T_RIGHT_PAREN, "Expected ')' to close reaction call.");
    return argCount;
}

/** Coordinates a Call Operation */
static void rule_call(bool canAssign) {
    uint8_t argCount = fuser_argumentList();
    EMIT_SHORT(OP_CALL, argCount);
}

/** Parses call accessors */
static void rule_accessor(bool canAssign) {
    CONSUME(T_IDENTIFIER, "Expected a property or reaction name after period.");
    uint16_t name = fuser_identifierConstant(&parser.previous);

    if (canAssign && MATCH(T_EQUAL)) {
        EXPRESSION;
        EMIT_BYTE(OP_SET_PROPERTY);
        EMIT_UINT16(name);
    } else if (MATCH(T_LEFT_PAREN)) {
        uint8_t argCount = fuser_argumentList();
        EMIT_BYTE(OP_INVOKE);
        EMIT_UINT16(name);
        EMIT_BYTE(argCount);
    } else {
        EMIT_BYTE(OP_GET_PROPERTY);
        EMIT_UINT16(name);
    }
}

#endif