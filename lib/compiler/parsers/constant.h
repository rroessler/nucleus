#ifndef NUC_CONSTANT_H
#define NUC_CONSTANT_H

// Nucleus Headers
#include "../../particle/object.h"
#include "../emit.h"
#include "../expression/variable.h"
#include "../parser.h"

// forward declaration
static void modelThis(bool canAssign);
static void modelSuper(bool canAssign);
static void reaction(ReactionType type);

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

/** Parses and compiles a base model method. */
static void baseModel_method() {
    uint8_t constant = identifierConstant(&parser.previous);
    reaction(RT_REACTION);  // will be a default reaction
    EMIT_SHORT(OP_SET_BASE_PROPERTY, constant);
}

/** Parses and compiles a base model field. */
static void baseModel_field() {
    uint8_t constant = identifierConstant(&parser.previous);
    advance();     // eat the colon
    expression();  // and the expression
    consume(T_SEMICOLON, "Expected ';' after field declaration.");
    EMIT_SHORT(OP_SET_BASE_PROPERTY, constant);
}

/** Parses an inline model set by "{}"" */
static void baseModel(bool canAssign) {
    // call to get the base model object
    Token token = syntheticToken(T_IDENTIFIER, "BaseModel");
    uint8_t global = identifierConstant(&token);
    EMIT_SHORT(OP_GET_GLOBAL, global);
    EMIT_SHORT(OP_CALL, 0);  // and call with ZERO arguments

    while (!check(T_RIGHT_BRACE) && !check(T_EOF)) {
        consume(T_IDENTIFIER, "Expecting a object method/field.");
        switch (parser.current.type) {
            case T_LEFT_PAREN:  // method
                baseModel_method();
                break;

            case T_COLON:  // field
                baseModel_field();
                break;

            default:  // bad token
                errorAtCurrent("Unknown symbol found in model declaration.");
                return;
        }

        // and pop item on the stack
        emitByte(OP_POP);
    }

    consume(T_RIGHT_BRACE, "Expected '}' after base object model body.");
}

/** Parses and compiles an prefix reaction. */
static void inlineReaction(bool canAssign) { reaction(RT_REACTION); }

#endif