#ifndef NUC_CLOSURE_H
#define NUC_CLOSURE_H

// Nucleus Headers
#include "reaction.h"

/** Upvalue Compiler Structure */
typedef struct {
    uint8_t index;
    bool isLocal;
} Upvalue;

/** Upvalue Nucleus Object */
typedef struct ObjUpvalue {
    Obj obj;
    Particle* location;
    Particle closed;
    struct ObjUpvalue* next;
} ObjUpvalue;

/** Closure Structure */
struct ObjClosure {
    Obj obj;
    ObjReaction* reac;
    ObjUpvalue** upvalues;
    int uvCount;
};

/**
 * Allocates a new Upvalue Object.
 * @param slot              Slot of upvalue.
 */
ObjUpvalue* upvalue_new(Particle* slot) {
    ObjUpvalue* upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
    upvalue->location = slot;
    upvalue->closed = NUC_NULL;
    upvalue->next = NULL;
    return upvalue;
}

/**
 * Allocates a new Closure Object.
 * @param reac              Reaction to associate with closure object.
 */
ObjClosure* closure_new(ObjReaction* reac) {
    // initialise upvalues into memory
    ObjUpvalue** upvalues = ALLOCATE(ObjUpvalue*, reac->uvCount);
    for (int i = 0; i < reac->uvCount; i++) upvalues[i] = NULL;

    // and allocate the closure
    ObjClosure* closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
    closure->reac = reac;
    closure->upvalues = upvalues;
    closure->uvCount = reac->uvCount;
    return closure;
}

#endif