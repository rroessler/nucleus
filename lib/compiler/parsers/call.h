#ifndef NUC_CALL_H
#define NUC_CALL_H

// Nucleus Headers
#include "../emit.h"
#include "../parser.h"

/** Compiles given Reaction call arguments. */
static uint8_t argumentList() {
    uint8_t argCount = 0;
    if (!check(T_RIGHT_PAREN)) {
        do {
            expression();
            if (argCount == UINT8_MAX) error("Cannot have more than 255 arguments in reaction call.");
            argCount++;
        } while (match(T_COMMA));
    }

    consume(T_RIGHT_PAREN, "Expected ')' to close reaction call.");
    return argCount;
}

/** Coordinates a Call Operation */
static void call(bool canAssign) {
    uint8_t argCount = argumentList();
    EMIT_SHORT(OP_CALL, argCount);
}

/** Parses call accessors */
static void accessor(bool canAssign) {
    consume(T_IDENTIFIER, "Expected a property or reaction name after period.");
    uint8_t name = identifierConstant(&parser.previous);

    if (canAssign && match(T_EQUAL)) {
        expression();
        EMIT_SHORT(OP_SET_PROPERTY, name);
    } else if (match(T_LEFT_PAREN)) {
        uint8_t argCount = argumentList();
        EMIT_SHORT(OP_INVOKE, name);
        emitByte(argCount);
    } else {
        EMIT_SHORT(OP_GET_PROPERTY, name);
    }
}

#endif