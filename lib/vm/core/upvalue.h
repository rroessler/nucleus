#ifndef NUC_ATOMIZER_UPVALUE_H
#define NUC_ATOMIZER_UPVALUE_H

// Nucleus Headers
#include "../../particle/object.h"

/**
 * Captures and creates an upvalue given by local variable.
 * @param local             Local variable to capture as an upvalue.
 */
static nuc_ObjUpvalue* upvalue_capture(nuc_Particle* local) {
    nuc_ObjUpvalue* prevUV = NULL;
    nuc_ObjUpvalue* uv = atomizer.openUVs;
    while (uv != NULL && uv->location > local) {
        prevUV = uv;
        uv = uv->next;
    }

    // if we have found our upvalue as already open, then return
    if (uv != NULL && uv->location == local) return uv;

    // create a captured upvalue
    nuc_ObjUpvalue* createdUV = upvalue_new(local);
    if (prevUV == NULL) {
        atomizer.openUVs = createdUV;
    } else {
        prevUV->next = createdUV;
    }

    return createdUV;
}

/**
 * Closes all open upvalues up to a set last upvalue.
 * @param last              Last valid upvalue to not close.
 */
static void nuc_upvalue_closeAll(nuc_Particle* last) {
    // while available to close
    while (atomizer.openUVs != NULL && atomizer.openUVs->location >= last) {
        nuc_ObjUpvalue* uv = atomizer.openUVs;
        uv->closed = *uv->location;
        uv->location = &uv->closed;
        atomizer.openUVs = uv->next;
    }
}

#endif