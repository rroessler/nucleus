#ifndef NUC_COMPILER_GLOBALS_H
#define NUC_COMPILER_GLOBALS_H

// C Standard Library
#include <stdlib.h>

// Nucleus Headers
#include "../common.h"
#include "../particle/particle.h"
#include "local/type.h"

/******************************
 *  GLOBAL COMPILATION ITEMS  *
 ******************************/

/** Generic Nucleus Compilation Structure. */
typedef struct nuc_Fuser {
    // reaction compilation
    nuc_ObjReaction* reaction;
    nuc_ReactionType type;        // type of compilation
    struct nuc_Fuser* enclosing;  // stack compilers

    // local variable compilation
    nuc_Local locals[UINT16_COUNT];      // allowing 2 ** 16 locals max
    size_t localCount;                   // total locals
    nuc_Upvalue upvalues[UINT16_COUNT];  // available upvalues slots
    int scopeDepth;                      // current scope depth
    uint32_t immutables[UINT16_COUNT];   // global immutables list
    size_t immutableCount;

    // available compiler flags
    uint32_t flags;
} nuc_Fuser;

/** Model Compilation Structure. */
typedef struct nuc_ModelFuser {
    struct nuc_ModelFuser* enclosing;
    bool hasBase;
} nuc_ModelFuser;

// global current compiler
nuc_Fuser* current = NULL;

// global model compiler
nuc_ModelFuser* currentModel = NULL;

#endif