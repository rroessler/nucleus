#ifndef NUC_RULE_ARRAY_H
#define NUC_RULE_ARRAY_H

// Nucleus Headers
#include "../../emit.h"
#include "../parser.h"

/** Parses an Array Literal. */
static void rule_arrayLiteral(bool canAssign) {
    // want to eat expressions and record the number of array inputs
    uint16_t arrayCount = 0;
    if (!CHECK(T_RIGHT_BRACK)) {
        do {
            EXPRESSION;
            if (arrayCount == UINT16_MAX) PARSER_ERROR_AT("Cannot have more than 65535 array initialisers.");
            arrayCount++;
        } while (MATCH(T_COMMA));
    }

    // and now expect an END OF ARRAY LITERAL
    CONSUME(T_RIGHT_BRACK, "Expected ']' to close an array literal.");
    EMIT_BYTE(OP_ARRAY);      // emit the array operation
    EMIT_UINT16(arrayCount);  // and the array count
}

/** Parses an array member accessor. */
static void rule_member(bool canAssign) {
    EXPRESSION;  // immediately eat the following expression
    CONSUME(T_RIGHT_BRACK, "Expected ']' to close member access operator.");

    if (canAssign && MATCH(T_EQUAL)) {  // request to set array member
        EXPRESSION;
        EMIT_BYTE(OP_SET_MEMBER);
    } else {  // request to get array member
        EMIT_BYTE(OP_GET_MEMBER);
    }
}

#endif