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

#endif