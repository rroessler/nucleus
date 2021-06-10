#ifndef NUC_COMPILER_CORE_H
#define NUC_COMPILER_CORE_H

// Nucleus Headers
#include "../../bytecode/chunk.h"

// FORWARD DECLARATIONS
static inline void chunk_emitByte(uint8_t byte);

/**************************
 *  GLOBAL FUSER METHODS  *
 **************************/

/** Retrieves the current chunk being compiled to. */
static inline nuc_Chunk* fuser_currentChunk() { return &current->reaction->chunk; }

/** Marks the top Local Variable as initialised */
static void fuser_markInitialised() {
    if (current->scopeDepth == 0) return;
    current->locals[current->localCount - 1].depth = current->scopeDepth;
}

/** Starts a compilation scope. */
static void fuser_beginScope() { current->scopeDepth++; }

/** Ends a compilation scope. */
static void fuser_endScope() {
    current->scopeDepth--;  // decrement the current scope

    // and want to POP all the local values
    while (current->localCount > 0 && current->locals[current->localCount - 1].depth > current->scopeDepth) {
        // close up values if captured or pop if otherwise
        chunk_emitByte(current->locals[current->localCount - 1].isCaptured ? OP_CLOSE_UPVALUE : OP_POP);
        current->localCount--;
    }
}

#endif