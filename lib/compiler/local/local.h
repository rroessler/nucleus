#ifndef NUC_LOCAL_H
#define NUC_LOCAL_H

// C Standard Library
#include <string.h>

// Nucleus Headers
#include "../../common.h"
#include "../emit.h"
#include "../global.h"
#include "../lexer/token.h"
#include "../parser/declaration/variable.h"
#include "../parser/expression.h"
#include "../parser/parser.h"
#include "type.h"

/****************************
 *  LOCAL VARIABLE METHODS  *
 ****************************/

/**
 * Adds a local variable into the current fuser scope.
 * @param name              Token name of local.
 * @param immutable         Whether the local is immutable
 */
static void fuser_addLocal(Token name, bool immutable) {
    // check if exceeded local defintions
    if (current->localCount == UINT16_COUNT) {
        PARSER_ERROR_AT("Too many local variables within scope.");
        return;
    }

    // and create a new local with the desired depth and name
    nuc_Local* local = &current->locals[current->localCount++];
    local->name = name;
    local->depth = -1;
    local->immutable = immutable;
}

/**
 * Adds a closure upvalue to the current compiler.
 * @param fuser                 Fuser to compare upvalue indices from.
 * @param index                 Index of upvalue.
 * @param isLocal               Whether upvalue is a local variable.
 * @param immutable             If an upvalue is immutable.
 */
static int fuser_addUpvalue(nuc_Fuser* fuser, uint16_t index, bool isLocal, bool immutable) {
    int uvCount = fuser->reaction->uvCount;  // grab the current upvalue count

    // check for duplication before proceeding to creation
    for (int i = 0; i < uvCount; i++) {
        nuc_Upvalue* upvalue = &fuser->upvalues[i];
        if (upvalue->index == index && upvalue->isLocal == isLocal) return i;
    }

    // make sure we haven't exceeded the upvalue limit
    if (uvCount == UINT16_COUNT) {
        PARSER_ERROR_AT("Too many closure variables within reaction.");
        return 0;
    }

    // and add as could not find it
    fuser->upvalues[uvCount].isLocal = isLocal;
    fuser->upvalues[uvCount].index = index;
    fuser->upvalues[uvCount].immutable = immutable;
    return fuser->reaction->uvCount++;
}

/**
 * Compares two identifier tokens to see if they are equal.
 * @param a                 Identifier A.
 * @param b                 Identifier B.
 */
static bool fuser_areIdentifiersEqual(Token* a, Token* b) {
    if (a->length != b->length) return false;
    return memcmp(a->start, b->start, a->length) == 0;
}

/**
 * Resolves a local reference by name.
 * @param fuser             Compiler to resolve a local of.
 * @param name              Name of local variable.
 */
static int fuser_resolveLocal(nuc_Fuser* fuser, Token* name) {
    for (int i = fuser->localCount - 1; i >= 0; i--) {
        nuc_Local* local = &fuser->locals[i];
        if (fuser_areIdentifiersEqual(name, &local->name)) {
            if (local->depth == -1) PARSER_ERROR_AT("Cannot read local variable in its own initialiser.");
            return i;
        }
    }
    return -1;
}

/**
 * Find the origin of an upvalue and check's if it is immutable.
 * @param fuser             Compiler to derive upvalue from.
 * @param name              Name of upvalue to check immutability of.
 */
static bool fuser_findUpvalueImmutability(nuc_Fuser* fuser, Token* name) {
    if (fuser->enclosing == NULL) return NUC_MUTABLE;  // here doesn't matter, as will FAIL later anyway

    int local = fuser_resolveLocal(fuser->enclosing, name);
    if (local != -1) return fuser->enclosing->locals[local].immutable;
    return fuser_findUpvalueImmutability(fuser->enclosing, name);  // continue looking recursively
}

/**
 * Resolves a closure upvalue by name.
 * @param fuser             Compiler to resolve upvalue from.
 * @param name              Name of upvalue to resolve.
 */
static int fuser_resolveUpvalue(nuc_Fuser* fuser, Token* name) {
    if (fuser->enclosing == NULL) return -1;

    int local = fuser_resolveLocal(fuser->enclosing, name);
    if (local != -1) {
        fuser->enclosing->locals[local].isCaptured = true;
        return fuser_addUpvalue(fuser, (uint16_t)local, true, fuser->enclosing->locals[local].immutable);
    }

    // recursion to help resolve upvalues
    int upvalue = fuser_resolveUpvalue(fuser->enclosing, name);
    if (upvalue != -1) return fuser_addUpvalue(fuser, (uint16_t)upvalue, false, fuser_findUpvalueImmutability(fuser->enclosing, name));

    // otherwise bad match
    return -1;
}

#endif