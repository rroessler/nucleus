#ifndef NUC_LOCAL_H
#define NUC_LOCAL_H

// C Standard Library
#include <string.h>

// forward declaration
static int fuser_resolveLocal(Fuser* fuser, Token* name);
static int fuser_resolveUpvalue(Fuser* fuser, Token* name);
static void fuser_markInitialised();

/**
 * Adds a local variable into the current fuser scope.
 * @param name              Token name of local.
 */
static void fuser_addLocal(Token name) {
    // check if exceeded local definitions
    if (current->localCount == UINT8_COUNT) {
        error("Too many local variables within scope.");
        return;
    }

    // and create a new local with the desired depth and name
    Local* local = &current->locals[current->localCount++];
    local->name = name;
    local->depth = -1;
}

/**
 * Adds a closure upvalue to the current compiler.
 * @param fuser                 Fuser to compare upvalue indices from.
 * @param index                 Index of upvalue.
 * @param isLocal               Whether upvalue is a local variable.
 */
static int fuser_addUpvalue(Fuser* fuser, uint8_t index, bool isLocal) {
    // grab the current upvalue count
    int uvCount = fuser->reac->uvCount;

    // check for duplication before proceeding to creation
    for (int i = 0; i < uvCount; i++) {
        Upvalue* upvalue = &fuser->upvalues[i];
        if (upvalue->index == index && upvalue->isLocal == isLocal) return i;
    }

    // make sure we haven't exceeded the upvalue limit
    if (uvCount == UINT8_COUNT) {
        error("Too many closure variables within reaction.");
        return 0;
    }

    // and add as could not find it
    fuser->upvalues[uvCount].isLocal = isLocal;
    fuser->upvalues[uvCount].index = index;
    return fuser->reac->uvCount++;
}

/** 
 * Creates a named global from a given token. 
 * @param name              Name of variable.
 * @param ignoreExpression  Denotes to ignore expression parsing before emitting SET operation.
 */
static void namedVariable(Token name, bool canAssign, bool ignoreExpression) {
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
        arg = identifierConstant(&name);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
    }

    if (canAssign && match(T_EQUAL)) {
        if (!ignoreExpression) expression();
        EMIT_SHORT(setOp, (uint8_t)arg);
    } else {
        EMIT_SHORT(getOp, (uint8_t)arg);
    }
}

/**
 * Compares two identifier tokens to see if they are equal.
 * @param a                 Identifier A.
 * @param b                 Identifier B.
 */
static bool areIdentifiersEqual(Token* a, Token* b) {
    if (a->length != b->length) return false;
    return memcmp(a->start, b->start, a->length) == 0;
}

/** Declares a local variable to the current compiler. */
static void declareVariable() {
    if (current->scopeDepth == 0) return;

    // retrieve the locals name
    Token* name = &parser.previous;

    // check if the local has already been declared, but only within THIS scope
    for (int i = current->localCount - 1; i >= 0; i--) {  // iterate backwards, as hit rate will occur with CURRENT scopes
        Local* local = &current->locals[i];

        // if we finish reading the current scope, then break out of the loop
        if (local->depth != -1 && local->depth < current->scopeDepth) break;

        // check there is no overlap for a variable
        if (areIdentifiersEqual(name, &local->name)) {
            error("A variable already exists with this name in this scope.");
        }
    }

    // and add it to the local scope
    fuser_addLocal(*name);
}

/**
 * Emits a global reference to define.
 * @param global            Global reference value.
 * @param immutable         If an item is declared as immutable.
 */
static void defineVariable(uint8_t global, bool immutable) {
    if (current->scopeDepth > 0) {
        fuser_markInitialised();  // mark all initialised locals
        return;
    }

    // declare item as immutable if needed
    if (immutable) emitByte(OP_SET_IMMUTABLE);
    EMIT_SHORT(OP_DEFINE_GLOBAL, global);
}

/**
 * Parses a variable declaration.
 * @param message               Error message.
 */
static uint8_t parseVariable(const char* message) {
    consume(T_IDENTIFIER, message);  // expect an identifier now

    // want to declare a variable within the desired scope
    declareVariable();
    if (current->scopeDepth > 0) return 0;

    return identifierConstant(&parser.previous);
}

#endif