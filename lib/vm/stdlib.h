#ifndef NUC_STDLIB_IMPORT_H
#define NUC_STDLIB_IMPORT_H

// Nucleus Headers
#include "../stdlib/natives.h"

/***********************
 *  NATIVE RESOLUTION  *
 ***********************/

/**
 * Resolves and loads a native to the available hash table. Although this will be slow on the
 * first call, it means quicker start times for atomization and less memory use when not
 * entirely using the whole native library.
 * @param name                          Name of native to load.
 */
static inline nuc_Particle atomizer_resolveNative(nuc_ObjString* name) {
    nuc_Particle value;
    for (int i = 0; i < NUC_NATIVE_REACTIONS_LEN; i++) {
        if (memcmp(name->chars, nuc_nativeReactionRefs[i].name, name->length) != 0) continue;
        value = NUC_OBJ(native_new(nuc_nativeReactionRefs[i].native));
        table_set(&atomizer.natives, name, value);  // DON'T need to worry about garbage collection here!
                                                    // This is becuase the nuc_ObjString* is already ON the stack
        return value;
    }

    // in case of an error
    atomizer_catchableError(NUC_EXIT_REF, "Unknown native \"%s\".", name->chars);
    return NUC_NULL;
}

/*******************
 *  MODEL LITERAL  *
 *******************/

/** Defines the MODEL LITERAL that is used to create anonymous models using the "{}" syntax. */
static inline void nuc_defineModelLiteral() {
    nuc_ObjString* modelName = objString_copy("Model", 5);
    nuc_Particle model = NUC_OBJ(model_new(modelName));  // generate the model
    table_set(&atomizer.globals, modelName, model);
}

/*******************
 *  MODEL LIBRARY  *
 *******************/

/** Defines a FIXED set of available models that a programmer may use. */
static inline void nuc_defineModelLibrary() {
    nuc_defineModelLiteral();  // the "{}" model literal
}

#endif