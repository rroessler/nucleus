#ifndef NUC_FUSER_H
#define NUC_FUSER_H

// Nucleus Headers
#include "../common.h"
#include "emit.h"
#include "lexer.h"
#include "parser.h"
#include "parsers/constant.h"
#include "parsers/grouping.h"
#include "parsers/unary.h"
#include "precedence.h"
#include "rules.h"

// conditional includes
#ifdef NUC_DEBUG_PRINT_CODE
    #include "../debug.h"
#endif

// forward declarations
void gc_markObject();

/*************
 *  GLOBALS  *
 *************/

/** Local variable struct. */
typedef struct {
    Token name;
    int depth;
    bool isCaptured;
} Local;

/** Fuser/Compiler Structure */
typedef struct Fuser {
    // reaction compilation
    ObjReaction* reac;
    ReactionType type;
    struct Fuser* enclosing;  // to allow for stacked compilers

    // local variable compilation
    Local locals[UINT8_COUNT];
    int localCount;
    Upvalue upvalues[UINT8_COUNT];  // closure compilation
    int scopeDepth;
} Fuser;

/** Model Compiler Structure (fixes global "this" references) */
typedef struct ModelFuser {
    struct ModelFuser* enclosing;
} ModelFuser;

/** Set a global current Compiler */
Fuser* current = NULL;
ModelFuser* currentModel = NULL;

/*****************************
 *  FUSER SCOPE BASED ITEMS  *
 *****************************/

// local variable parsing / compilation
#include "parsers/local.h"

/**
 * Resolves a local reference by name.
 * @param fuser             Compiler to resolve a local of.
 * @param name              Name of local variable.
 */
static int fuser_resolveLocal(Fuser* fuser, Token* name) {
    for (int i = fuser->localCount - 1; i >= 0; i--) {
        Local* local = &fuser->locals[i];
        if (areIdentifiersEqual(name, &local->name)) {
            if (local->depth == -1) error("Cannot read local variable in its own initialiser.");
            return i;
        }
    }
    return -1;
}

/**
 * Resolves a closure upvalue by name.
 * @param fuser             Compiler to resolve upvalue from.
 * @param name              Name of upvalue to resolve.
 */
static int fuser_resolveUpvalue(Fuser* fuser, Token* name) {
    if (fuser->enclosing == NULL) return -1;

    int local = fuser_resolveLocal(fuser->enclosing, name);
    if (local != -1) {
        fuser->enclosing->locals[local].isCaptured = true;
        return fuser_addUpvalue(fuser, (uint8_t)local, true);
    }

    // recursion to help resolve upvalues
    int upvalue = fuser_resolveUpvalue(fuser->enclosing, name);
    if (upvalue != -1) return fuser_addUpvalue(fuser, (uint8_t)upvalue, false);

    // otherwise bad match
    return -1;
}

/** Marks the top Local Variable as initialised */
static void fuser_markInitialised() {
    if (current->scopeDepth == 0) return;
    current->locals[current->localCount - 1].depth = current->scopeDepth;
}

/** Parses a model "this" reference */
static void modelThis(bool canAssign) {
    // ensure we have a class accessing "this"
    if (currentModel == NULL) {
        error("Cannot user \"this\" outside of a model.");
        return;
    }

    // and parse as a variable
    emitByte(OP_MUTATE);  // want "this" to be mutable
    variable(false);
}

/********************
 *  ERROR HANDLING  *
 ********************/

/**
 * Throws an error from the previously parsed token.
 * @param message               Error message.
 */
static void error(const char* message) { parser_errorAt(&parser.previous, message); }

/**
 * Throws an error from the current token.
 * @param message               Error message.
 */
static void errorAtCurrent(const char* message) { parser_errorAt(&parser.current, message); }

/******************
 *  COMPILER API  *
 ******************/

/** Initialises a compiler instance. */
static void fuser_init(Fuser* fuser, ReactionType type) {
    fuser->enclosing = current;
    fuser->reac = NULL;
    fuser->type = type;
    fuser->localCount = 0;
    fuser->scopeDepth = 0;

    // and NOW allocate new reaction
    fuser->reac = reaction_new();

    // and set the current compiler
    current = fuser;
    if (type != RT_SCRIPT) current->reac->name = objString_copy(parser.previous.start, parser.previous.length);

    // claim slot 0 for VM use only
    Local* local = &current->locals[current->localCount++];
    local->depth = 0;
    local->isCaptured = false;

    // allow THIS referencing
    if (type != RT_REACTION) {
        local->name.start = "this";
        local->name.length = 4;
    } else {
        local->name.start = "";
        local->name.length = 0;
    }
}

/** Retrieves the currently chunk being compiled to. */
static Chunk* currentChunk() { return &current->reac->chunk; }

/** Starts a compilation scope. */
static void fuser_beginScope() { current->scopeDepth++; }

/** Ends a compilation scope. */
static void fuser_endScope() {
    current->scopeDepth--;  // decrement the current scope

    // and want to POP all the local values
    while (current->localCount > 0 && current->locals[current->localCount - 1].depth > current->scopeDepth) {
        // close up values if captured or pop if otherwise
        emitByte(current->locals[current->localCount - 1].isCaptured ? OP_CLOSE_UPVALUE : OP_POP);
        current->localCount--;
    }
}

/** Coordinates ending the compilation process. */
static ObjReaction* fuser_end() {
    emitReturn();
    ObjReaction* reac = current->reac;

#ifdef NUC_DEBUG_PRINT_CODE
    // display chunk if desired
    if (!parser.hadError) nuc_disassembleChunk(
        currentChunk(),
        reac->name != NULL
            ? reac->name->chars  // using reaction name
            : "<script>");       // or the base script
#endif

    // and return the compiled reaction
    current = current->enclosing;  // and dec the compilation stack
    return reac;
}

/****************************
 *  EXPRESSION PARSING API  *
 ****************************/

/** Parses a Nucleus Expression. */
static void expression() {
    parsePrecedence(P_ASSIGNMENT);
}

/***************
 *  FUSER API  *
 ***************/

#include "expression/declaration.h"
#include "statement/statement.h"

/**
 * Coordinates compilation of Nucleus source code.
 * @param source                Source code.
 */
ObjReaction* fuse(const char* source) {
    lexer_init(source);             // initialise the lexer
    Fuser fuser;                    // and the compiler
    fuser_init(&fuser, RT_SCRIPT);  // set to base SCRIPT type

    // set the parser to allow input
    parser.hadError = false;
    parser.panicMode = false;

    // prime the compilation pump
    advance();

    // and run the main loop of fusing a code chunk
    while (!match(T_EOF)) declaration();

    // return if we had an error
    ObjReaction* reac = fuser_end();  // stop the compilation
    return parser.hadError ? NULL : reac;
}

/** Marks the compilers roots as safe from garbage collection. */
void gc_markCompilerRoots() {
    Fuser* fuser = current;
    while (fuser != NULL) {
        gc_markObject((Obj*)fuser->reac);
        fuser = fuser->enclosing;
    }
}

#endif