#ifndef NUC_DISRUPTION_MODEL_H
#define NUC_DISRUPTION_MODEL_H

// Nucleus Headers
#include "../core/core.h"
#include "codes.h"

/**
 * Creates and pushes a disruption model to the atomizer.
 * @param msg               Error message.
 * @param length            Length of error message.
 * @param code              Error code.
 */
static inline void atomizer_buildDisruptionModel(const char* msg, size_t length, uint8_t code) {
    // retrieve the parent model
    nuc_Particle base;
    table_get(&atomizer.globals, atomizer.disruption, &base);

    // create the disruption instance
    nuc_Particle disruption = NUC_OBJ(model_newInstance(AS_MODEL(base)));
    PUSH(disruption);
    nuc_ObjInstance* instance = AS_INSTANCE(PEEK(0));

    // now set the message string
    nuc_ObjString* msgAccessor = objString_copy("message", 7);
    table_set(&instance->fields, msgAccessor, NUC_OBJ(objString_copy(msg, length)));

    // and after that the exit code given
    nuc_ObjString* codeAccessor = objString_copy("code", 4);
    table_set(&instance->fields, codeAccessor, NUC_NUM(code));
}

#endif