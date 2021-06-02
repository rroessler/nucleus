#ifndef NUC_NATIVE_REACTION_H
#define NUC_NATIVE_REACTION_H

// Nucleus Headers
#include "reaction.h"

/** Type definition for a Native Reaction. */
typedef Particle (*NativeRn)(int argCount, Particle* args);

/** Native Reaction Wrapper for Native Nucleus methods. */
typedef struct {
    Obj obj;
    NativeRn reac;
} ObjNative;

/**
 * Creates a new Native Reaction object.
 * @param reac              Reaction for native object.
 */
ObjNative* native_new(NativeRn reac) {
    ObjNative* native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
    native->reac = reac;
    return native;
}

#endif