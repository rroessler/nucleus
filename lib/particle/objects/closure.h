#ifndef NUC_OBJ_CLOSURE_H
#define NUC_OBJ_CLOSURE_H

// Nucleus Headers
#include "../../utils/memory.h"
#include "reaction.h"

/** Upvalue Nucleus Object */
typedef struct nuc_ObjUpvalue {
    nuc_Obj obj;
    nuc_Particle* location;
    nuc_Particle closed;
    struct nuc_ObjUpvalue* next;
} nuc_ObjUpvalue;

/** Closure Structure */
typedef struct nuc_ObjClosure {
    nuc_Obj obj;
    nuc_ObjReaction* reaction;
    nuc_ObjUpvalue** upvalues;
    int uvCount;
} nuc_ObjClosure;

/*****************
 *  CLOSURE API  *
 *****************/

/**
 * Allocates a new Upvalue Object.
 * @param slot              Slot of upvalue.
 */
nuc_ObjUpvalue* upvalue_new(nuc_Particle* slot) {
    nuc_ObjUpvalue* upvalue = NUC_ALLOC_OBJ(nuc_ObjUpvalue, OBJ_UPVALUE);
    upvalue->location = slot;
    upvalue->closed = NUC_NULL;
    upvalue->next = NULL;
    return upvalue;
}

/**
 * Allocates a new Closure Object.
 * @param reaction              Reaction to associate with closure object.
 */
nuc_ObjClosure* closure_new(nuc_ObjReaction* reaction) {
    // initialise upvalues into memory
    nuc_ObjUpvalue** upvalues = NUC_ALLOC(nuc_ObjUpvalue*, reaction->uvCount);
    for (int i = 0; i < reaction->uvCount; i++) upvalues[i] = NULL;

    // and allocate the closure
    nuc_ObjClosure* closure = NUC_ALLOC_OBJ(nuc_ObjClosure, OBJ_CLOSURE);
    closure->reaction = reaction;
    closure->upvalues = upvalues;
    closure->uvCount = reaction->uvCount;
    return closure;
}

#endif