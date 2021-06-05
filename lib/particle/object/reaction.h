#ifndef NUC_REACTION_H
#define NUC_REACTION_H

// Nucleus Headers
#include "../../chunk/chunk.h"
#include "../../memory.h"
#include "../object.h"
#include "string.h"

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

// forward declaration
typedef struct ObjClosure ObjClosure;

/** Type Definition for Reaction Types. */
typedef enum {
    RT_REACTION,
    RT_METHOD,
    RT_INITIALISER,
    RT_SCRIPT,
} ReactionType;

/** Nucleus Reaction Object */
typedef struct {
    Obj obj;
    int arity;
    int uvCount;
    int defaults;
    Chunk chunk;
    ObjString* name;
} ObjReaction;

/** Call Frames Structure */
typedef struct {
    ObjClosure* closure;
    uint8_t* ip;
    Particle* slots;
    uint8_t slotCount;
} CallFrame;

/** Creates an allocates memory for a new Reaction Object. */
ObjReaction* reaction_new() {
    ObjReaction* reac = ALLOCATE_OBJ(ObjReaction, OBJ_REACTION);
    reac->arity = 0;
    reac->uvCount = 0;
    reac->defaults = 0;
    reac->name = NULL;
    chunk_init(&reac->chunk);
    return reac;
}

/**
 * Prints a reaction.
 * @param reac              Reaction to print.
 */
static void reaction_print(ObjReaction* reac) {
    if (reac->name == NULL) {
        printf("<script>");
        return;
    }
    printf("\x1b[3;31m<rn: %s>\x1b[0m", reac->name->chars);
}

#endif