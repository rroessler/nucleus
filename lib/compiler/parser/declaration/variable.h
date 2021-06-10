#ifndef NUC_VARIABLE_DECLARATION_H
#define NUC_VARIABLE_DECLARATION_H

// Nucleus Includes
#include "../../../particle/particle.h"
#include "../../emit.h"
#include "../../local/local.h"
#include "../expression.h"
#include "../rules/precedence.h"

// FORWARD DECLARATIONS
static void fuser_markInitialised();
DEF_EXPR;  // declare the EXPRESSION macro

/**********************
 *  VARIABLE METHODS  *
 **********************/

/**
 * Creates an identifier constant.
 * @param name              Token holding identifier name.
 */
static uint16_t fuser_identifierConstant(Token* name) {
    return chunk_makeConstant(NUC_OBJ(objString_copy(name->start, name->length)));
}

/** 
 * Declares a local variable to the current compiler. 
 * @param immutable                         Denotes an immutable variable.
 */
static void fuser_declareVariable(bool immutable) {
    if (current->scopeDepth == 0) return;

    // retrieve the locals name
    Token* name = &parser.previous;

    // check if the local has already been declared, but only within THIS scope
    for (int i = current->localCount - 1; i >= 0; i--) {  // iterate backwards, as hit rate will occur with CURRENT scopes
        nuc_Local* local = &current->locals[i];

        // if we finish reading the current scope, then break out of the loop
        if (local->depth != -1 && local->depth < current->scopeDepth) break;

        // check there is no overlap for a variable
        if (fuser_areIdentifiersEqual(name, &local->name)) {
            PARSER_ERROR_AT("A variable already exists with this name in this scope.");
        }
    }

    // and add it to the local scope
    fuser_addLocal(*name, immutable);
}

/**
 * Emits a global reference to define.
 * @param global            Global reference value.
 */
static void fuser_defineVariable(uint16_t global) {
    if (current->scopeDepth > 0) {
        fuser_markInitialised();  // mark all initialised locals
        return;
    }

    EMIT_BYTE(OP_DEFINE_GLOBAL);
    EMIT_UINT16(global);
}

/**
 * Adds a global variable reference as immutable to the current compiler's list
 * of immutables.
 * @param ghash                             Global hash reference.
 */
static inline void fuser_addGlobalImmutable(uint32_t ghash) {
    current->immutables[current->immutableCount++] = ghash;
}

/**
 * Determines if an global is immutable.
 * @param ghash                             Global hash reference to check.
 */
static inline bool fuser_checkGlobalImmutable(uint32_t ghash) {
    for (int i = 0; i < current->immutableCount; i++) {
        if (current->immutables[i] == ghash) return NUC_IMMUTABLE;
    }

    // otherwise will always be mutable
    return NUC_MUTABLE;
}

/**
 * Parses a variable declaration.
 * @param message               Error message.
 * @param immutable             Denotes a variable as immutable.
 */
static uint16_t fuser_parseVariable(const char* message, bool immutable) {
    CONSUME(T_IDENTIFIER, message);  // expect an identifier now

    // want to declare a variable within the desired scope
    fuser_declareVariable(immutable);
    if (current->scopeDepth > 0) return 0;

    // save if the global is immutable or not to the global scope
    uint16_t global = fuser_identifierConstant(&parser.previous);
    if (immutable) fuser_addGlobalImmutable(hash_generic(parser.previous.start, parser.previous.length));
    return global;  // and return the global
}

/** 
 * Compiles a variable declaration. 
 * @param immutable                     Denotes if an item is specified with the 'const' keyword.
 */
static void fuser_variableDeclaration(bool immutable) {
    // parse the variable
    uint16_t global = fuser_parseVariable("Expected a variable name.", immutable);

    // and match a given token
    if (MATCH(T_EQUAL)) {
        EXPRESSION;
    } else {  // invalid expression so emit byte
        EMIT_BYTE(OP_NULL);
    }

    // and now expect a closing semicolon
    CONSUME(T_SEMICOLON, "Expected a ';' after variable declaration.");
    fuser_defineVariable(global);
}

/** 
 * Creates a named global from a given token. 
 * @param name              Name of variable.
 * @param ignoreExpression  Denotes to ignore expression parsing before emitting SET operation.
 */
static void fuser_namedVariable(Token name, bool canAssign, bool ignoreExpression) {
    // and either set/get
    uint8_t getOp, setOp;
    int arg = fuser_resolveLocal(current, &name);
    if (arg != -1) {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
    } else if ((arg = fuser_resolveUpvalue(current, &name)) != -1) {
        getOp = OP_GET_UPVALUE;
        setOp = OP_SET_UPVALUE;
    } else {
        arg = fuser_identifierConstant(&name);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
    }

// setting up a simple macro for ease of reading
#define NUC_ALLOW_MUTATION                                         \
    if (!NUC_CHECK_CFLAG(NUC_CFLAG_MUTATE_NEXT)) {                 \
        PARSER_ERROR_AT("Cannot reassign an immutable constant."); \
    } else {                                                       \
        NUC_UNSET_CFLAG(NUC_CFLAG_MUTATE_NEXT);                    \
    }

    if (canAssign && MATCH(T_EQUAL)) {
        // need do do some elaborate checking for IMMUTABLE items, IGNORE if @mutate is set
        if (setOp == OP_SET_LOCAL && current->locals[arg].immutable) {
            NUC_ALLOW_MUTATION;
        } else if (setOp == OP_SET_UPVALUE && current->upvalues[arg].immutable) {
            NUC_ALLOW_MUTATION;
        } else if (fuser_checkGlobalImmutable(hash_generic(name.start, name.length))) {
            NUC_ALLOW_MUTATION;
        }

        if (!ignoreExpression) EXPRESSION;
        EMIT_BYTE(setOp);
    } else {
        EMIT_BYTE(getOp);
    }

    // and emit the argument regardless
    EMIT_UINT16(arg);

#undef NUC_ALLOW_MUTATION
}

/** Parses a named variable. */
static void rule_variable(bool canAssign) {
    fuser_namedVariable(parser.previous, canAssign, false);
}

#endif