#ifndef NUC_CONSTANT_H
#define NUC_CONSTANT_H

// Nucleus Headers
#include "../../particle/object.h"
#include "../emit.h"
#include "../expression/variable.h"
#include "../parser.h"

// forward declaration
static void modelThis(bool canAssign);

/** Parses a "number" constant. */
static void number(bool canAssign) {
    double value = strtod(parser.previous.start, NULL);
    emitConstant(NUC_NUMBER(value));
}

/** Parses a "string" constant. */
static void string(bool canAssign) {
    emitConstant(NUC_OBJ(objString_copy(parser.previous.start + 1, parser.previous.length - 2)));
}

/** Parses a "literal" constant. */
static void literal(bool canAssign) {
    switch (parser.previous.type) {
        CASE_EMIT(T_FALSE, OP_FALSE);
        CASE_EMIT(T_TRUE, OP_TRUE);
        CASE_EMIT(T_NULL, OP_NULL);
        default:  // unreachable
            return;
    }
}

/** Parses an inline model set by "{}"" */
static void model(bool canAssign) {
    // call to get the base model object
    Token token = syntheticToken(T_IDENTIFIER, "BaseModel");
    EMIT_SHORT(OP_GET_GLOBAL, identifierConstant(&token));
    EMIT_SHORT(OP_CALL, 0);  // and call with ZERO arguments

    consume(T_RIGHT_BRACE, "Expected '}' after base object model body.");
}

#endif