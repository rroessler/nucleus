#ifndef NUC_BASE_MODEL_H
#define NUC_BASE_MODEL_H

// Nucleus Headers
#include "../../emit.h"
#include "../declaration/reaction.h"
#include "../parser.h"
#include "call.h"

/** Parses and compiles a base model method. */
static void baseModel_method() {
    uint16_t constant = fuser_identifierConstant(&parser.previous);
    nuc_reaction(RT_REACTION);  // will be a default reaction
    EMIT_BYTE(OP_SET_BASE_PROPERTY);
    EMIT_UINT16(constant);
}

/** Parses and compiles a base model field. */
static void baseModel_field() {
    uint16_t constant = fuser_identifierConstant(&parser.previous);
    ADVANCE;     // eat the colon
    EXPRESSION;  // and the expression
    CONSUME(T_SEMICOLON, "Expected ';' after field declaration.");
    EMIT_BYTE(OP_SET_BASE_PROPERTY);
    EMIT_UINT16(constant);
}

/** Parses an inline model set by "{}"" */
static void rule_baseModel(bool canAssign) {
    // call to get the base model object
    Token token = syntheticToken(T_IDENTIFIER, "Model");
    uint16_t global = fuser_identifierConstant(&token);
    EMIT_BYTE(OP_GET_GLOBAL);
    EMIT_UINT16(global);
    EMIT_SHORT(OP_CALL, 0);  // and call with ZERO arguments

    while (!CHECK(T_RIGHT_BRACE) && !CHECK(T_EOF)) {
        CONSUME(T_IDENTIFIER, "Expecting a object method/field.");
        switch (parser.current.type) {
            case T_LEFT_PAREN:  // method
                baseModel_method();
                break;

            case T_COLON:  // field
                baseModel_field();
                break;

            default:  // bad token
                PARSER_ERROR_AT_CURRENT("Unknown symbol found in model declaration.");
                return;
        }

        // and pop item on the stack
        EMIT_BYTE(OP_POP);
    }

    CONSUME(T_RIGHT_BRACE, "Expected '}' after base object model body.");
}

/** Parses a model "this" reference */
static void rule_modelThis(bool canAssign) {
    // ensure we have a class accessing "this"
    if (currentModel == NULL) {
        PARSER_ERROR_AT("Cannot user \"this\" outside of a model.");
        return;
    }

    // and parse as a variable
    rule_variable(false);
}

/** Parses a model "super" reference */
static void rule_modelSuper(bool canAssign) {
    // error checking
    if (currentModel == NULL) {
        PARSER_ERROR_AT("Cannot use 'super' keyword outside of a model.");
    } else if (!currentModel->hasBase) {
        PARSER_ERROR_AT("Cannot user 'super' keyword in a model with no base.");
    }

    CONSUME(T_PERIOD, "Expected a period after \"super\" keyword.");
    CONSUME(T_IDENTIFIER, "Expected parent model method name.");
    uint16_t name = fuser_identifierConstant(&parser.previous);

    // start the super call
    fuser_namedVariable(syntheticToken(T_THIS, "this"), false, false);
    if (MATCH(T_LEFT_PAREN)) {
        uint8_t argCount = fuser_argumentList();
        fuser_namedVariable(syntheticToken(T_SUPER, "super"), false, false);
        EMIT_BYTE(OP_SUPER_INVOKE);
        EMIT_UINT16(name);
        EMIT_BYTE(argCount);
    } else {
        fuser_namedVariable(syntheticToken(T_SUPER, "super"), false, false);
        EMIT_BYTE(OP_GET_SUPER);
        EMIT_UINT16(name);
    }
}

#endif