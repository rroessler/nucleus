#ifndef NUC_OBJ_REACTION_H
#define NUC_OBJ_REACTION_H

// Nucleus Headers
#include "../../bytecode/chunk.h"
#include "../../utils/memory.h"
#include "closure.h"
#include "string.h"

// forward declaration of allocation item
nuc_Obj* obj_alloc(size_t size, nuc_ObjType type);

/** Reaction Types */
typedef enum {
    RT_REACTION,
    RT_METHOD,
    RT_INITIALISER,
    RT_SCRIPT,
    RT_EVAL,
} nuc_ReactionType;

/** Nucleus Reaction Object */
typedef struct {
    nuc_Obj obj;
    int arity;            // total arguments expected
    int defaults;         // arguments defaulted
    int uvCount;          // upvalue counts
    nuc_Chunk chunk;      // compiled chunk
    nuc_ObjString* name;  // reaction name
} nuc_ObjReaction;

// type definition for a Native Reaction
typedef nuc_Particle (*nuc_NativeReaction)(int argCount, nuc_Particle* args);

/** Nucleus Native Reaction Object */
typedef struct {
    nuc_Obj obj;
    nuc_NativeReaction reaction;
} nuc_ObjNative;

/**********************
 *  REACTION METHODS  *
 **********************/

/** Allocates memory for a new Nucleus Reaction Object. */
nuc_ObjReaction* reaction_new() {
    nuc_ObjReaction* reaction = NUC_ALLOC_OBJ(nuc_ObjReaction, OBJ_REACTION);
    reaction->arity = 0;
    reaction->defaults = 0;
    reaction->uvCount = 0;
    reaction->name = NULL;
    chunk_init(&reaction->chunk);
    return reaction;
}

/**
 * Creates a new Native Reaction object.
 * @param reaction              Reaction for native object.
 */
nuc_ObjNative* native_new(nuc_NativeReaction reaction) {
    nuc_ObjNative* native = NUC_ALLOC_OBJ(nuc_ObjNative, OBJ_NATIVE);
    native->reaction = reaction;
    return native;
}

/**
 * Prints a reaction to stdout.
 * @param reaction                  Reaction to print.
 */
static inline void reaction_print(nuc_ObjReaction* reaction) {
    if (reaction->name == NULL) {
        printf("<script>");
    } else {
        printf("<reaction: %s>", reaction->name->chars);
    }
}

#endif