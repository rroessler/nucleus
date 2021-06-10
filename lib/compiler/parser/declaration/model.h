#ifndef NUC_MODEL_DECLARATION_H
#define NUC_MODEL_DECLARATION_H

// C Standard Library
#include <stdlib.h>

// Nucleus Headers
#include "../../global.h"
#include "../expression.h"
#include "../rules/precedence.h"
#include "reaction.h"

// FORWARD DECLARATION
static uint16_t fuser_identifierConstant(Token* name);
static void fuser_declareVariable(bool immutable);
static void fuser_defineVariable(uint16_t global);
static void fuser_namedVariable(Token name, bool canAssign, bool ignoreExpression);
static void rule_variable(bool canAssign);
static bool fuser_areIdentifiersEqual(Token* a, Token* b);
static void fuser_addLocal(Token name, bool immutable);
DEF_EXPR;  // declare the EXPRESSION macro

/**************************
 *  MODEL PARSING METHOD  *
 **************************/

/** Parses / Compiles Model Method */
static void nuc_method() {
    uint16_t constant = fuser_identifierConstant(&parser.previous);

    // parse the item as a reaction
    nuc_ReactionType type = RT_METHOD;
    if (parser.previous.length == 10 && memcmp(parser.previous.start, "@construct", 10) == 0) type = RT_INITIALISER;
    nuc_reaction(type);

    // and emit as a method
    EMIT_BYTE(OP_METHOD);
    EMIT_UINT16(constant);
}

/** Parses / Compiles a Model Field */
static void nuc_field() {
    uint16_t constant = fuser_identifierConstant(&parser.previous);
    ADVANCE;  // eat the colon
    EXPRESSION;
    CONSUME(T_SEMICOLON, "Expected ';' after field declaration.");

    // and emit as a field
    EMIT_BYTE(OP_FIELD);
    EMIT_UINT16(constant);
}

/** Parses and Compiles a Model Declaration */
static void fuser_modelDeclaration() {
    CONSUME(T_IDENTIFIER, "Expected a model name.");
    Token modelName = parser.previous;
    uint16_t nameConstant = fuser_identifierConstant(&modelName);
    fuser_declareVariable(NUC_IMMUTABLE);  // MODEL declarations are immutable
    fuser_addGlobalImmutable(hash_generic(modelName.start, modelName.length));

    // now want to emit the model bytes
    EMIT_BYTE(OP_MODEL);
    EMIT_UINT16(nameConstant);
    fuser_defineVariable(nameConstant);  // and wan to name to be immutable

    // and set as a new model fuser
    nuc_ModelFuser modelFuser;
    modelFuser.hasBase = false;
    modelFuser.enclosing = currentModel;
    currentModel = &modelFuser;

    // and check for model derivations
    if (MATCH(T_DERIVES)) {
        CONSUME(T_IDENTIFIER, "Expected base model name.");
        rule_variable(false);
        if (fuser_areIdentifiersEqual(&modelName, &parser.previous)) PARSER_ERROR_AT("A model cannot inherit from itself.");

        fuser_beginScope();
        fuser_addLocal(syntheticToken(T_IDENTIFIER, "super"), NUC_IMMUTABLE);  // "super" should be immutable
        fuser_defineVariable(0);

        fuser_namedVariable(modelName, false, false);
        EMIT_BYTE(OP_INHERIT);
        modelFuser.hasBase = true;
    }

    // bring the class to the top of the stack
    fuser_namedVariable(modelName, false, true);

    // now need to consume the model body
    CONSUME(T_LEFT_BRACE, "Expected '{' before model body.");
    while (!CHECK(T_RIGHT_BRACE) && !CHECK(T_EOF)) {
        CONSUME(T_IDENTIFIER, "Expecting a method/field name.");
        switch (parser.current.type) {
            case T_LEFT_PAREN:  // method declarations
                nuc_method();
                break;

            case T_COLON:  // field declarations
                nuc_field();
                break;

            default:  // bad token
                PARSER_ERROR_AT_CURRENT("Unknown symbol found in model declaration.");
                return;
        }
    }

    // and cap off the end of the model declaration
    CONSUME(T_RIGHT_BRACE, "Expected '}' after model body.");
    CONSUME(T_SEMICOLON, "Expected ';' after model declaration.");
    EMIT_BYTE(OP_POP);  // and pop class off the stack

    if (modelFuser.hasBase) fuser_endScope();

    // and remove top reference of model fuser
    currentModel = currentModel->enclosing;
}

#endif