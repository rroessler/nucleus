#ifndef NUC_FUSER_H
#define NUC_FUSER_H

// Nucleus Headers
#include "core/flags.h"
#include "global.h"
#include "lexer/lexer.h"
#include "parser/declaration/declaration.h"
#include "parser/parser.h"

#ifdef NUC_DEBUG_BYTECODE  // debug includes
    #include "../bytecode/debug.h"
#endif

/*******************
 *  FUSER METHODS  *
 *******************/

/** 
 * Initialises a compiler instance. 
 * @param fuser             Compiler to initialise.
 * @param type              Type of compilation.
 */
static void fuser_init(nuc_Fuser* fuser, nuc_ReactionType type) {
    fuser->enclosing = current;
    fuser->reaction = NULL;
    fuser->type = type;
    fuser->localCount = 0;
    fuser->scopeDepth = 0;
    fuser->immutableCount = 0;
    NUC_RESET_CFLAGS;

    // and now allocate the new reaction
    fuser->reaction = reaction_new();

    // set the current compiler
    current = fuser;
    if (type != RT_SCRIPT) current->reaction->name = objString_copy(parser.previous.start, parser.previous.length);

    // claim slot 0 for VM use only
    nuc_Local* local = &current->locals[current->localCount++];
    local->depth = 0;
    local->isCaptured = false;
    local->immutable = true;  // want to denote as immutable

    // allow THIS referencing
    if (type != RT_REACTION) {
        local->name.start = "this";
        local->name.length = 4;
    } else {
        local->name.start = "";
        local->name.length = 0;
    }
}

/** Coordinates ending the compilation process. */
static nuc_ObjReaction* fuser_complete() {
    chunk_emitReturn();
    nuc_ObjReaction* reaction = current->reaction;

#ifdef NUC_DEBUG_BYTECODE  // display chunk if desired
    if (!parser.hadError) nuc_disassembleChunk(
        fuser_currentChunk(),
        reaction->name != NULL
            ? reaction->name->chars  // using reaction name
            : "<script>");           // or the base script
#endif

    // and return the compiled reaction
    current = current->enclosing;  // decrement the compiler stack
    return reaction;
}

/**
 * Coordinates compilation of Nucleus source code.
 * @param source                Source code.
 */
nuc_ObjReaction* nuc_fuse(const char* source) {
    lexer_init(source);             // initalise the lexer
    nuc_Fuser fuser;                // and the compiler
    fuser_init(&fuser, RT_SCRIPT);  // set the base script

    // set the parser into start mode
    parser.hadError = false;
    parser.panicMode = false;

    // prime the compilation
    ADVANCE;

    // and run the main loop of fusing a code chunk
    while (!MATCH(T_EOF)) nuc_declaration();

    // return NULL if an error occured
    nuc_ObjReaction* reaction = fuser_complete();  // stop the compilation
    return parser.hadError ? NULL : reaction;
}

#endif