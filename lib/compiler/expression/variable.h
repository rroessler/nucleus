#ifndef NUC_VARIABLE_DECLARATION_H
#define NUC_VARIABLE_DECLARATION_H

// Nucleus Includes
#include "../../particle/duality.h"
#include "../emit.h"
#include "../parser.h"

// forward declarations
static void expression();
static void defineVariable(uint8_t global, bool immutable);
static uint8_t parseVariable(const char* message);
static void namedVariable(Token name, bool canAssign, bool ignoreExpression);

/**
 * Creates an identifier constant.
 * @param name              Token holding identifier name.
 */
static uint8_t identifierConstant(Token* name) {
    return makeConstant(NUC_OBJ(objString_copy(name->start, name->length)));
}

/** 
 * Compiles a variable declaration. 
 * @param immutable         Denotes if the variable is immutable.
 */
static void variableDeclaration(bool immutable) {
    // parse the variable
    uint8_t global = parseVariable("Expected a variable name.");

    if (match(T_EQUAL)) {  // set as an expression
        expression();
    } else {  // set as initialiser value
        emitByte(OP_NULL);
    }

    // now expect a closing semicolon
    consume(T_SEMICOLON, "Expected a ';' after variable declaration.");
    defineVariable(global, immutable);
}

/** Parses a named variable. */
static void variable(bool canAssign) {
    namedVariable(parser.previous, canAssign, false);
}

#endif