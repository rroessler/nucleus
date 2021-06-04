#ifndef NUC_STDLIB_H
#define NUC_STDLIB_H

// Nucleus Headers
#include "../particle/object.h"
#include "../particle/table.h"

// Nucleus Standard Library
#include "../stdlib/native.h"

// helper macro to define natives
#define NUC_DEFINE_NATIVE(accessor, value)                                         \
    PUSH(NUC_OBJ(objString_copy(accessor, (int)strlen(accessor))));                \
    PUSH(value);                                                                   \
    table_set(&atomizer.globals, AS_STRING(atomizer.stack[0]), atomizer.stack[1]); \
    POP();                                                                         \
    POP()

/**
 * Defines a Native Reaction for use within the Atomizer.
 * @param name                  Name of reaction.
 * @param reac                  Reaction definition.
 */
static void nuc_defineNative(const char* name, NativeRn reac) {
#ifdef NUC_DEBUG_PRINT_CODE
    printf("[\x1b[2;34mnative: method\x1b[0m] -> \x1b[36m%s\x1b[0m\n", name);
#endif

    // and define the native
    NUC_DEFINE_NATIVE(name, NUC_OBJ(native_new(reac)));
}

/**
 * Defines a Native Property for use within the Atomizer.
 * @param accessor              Accessor of property.
 * @param value                 Value associated with definition.
 */
static void nuc_defineNativeProperty(const char* accessor, Particle value) {
#ifdef NUC_DEBUG_PRINT_CODE
    printf("[\x1b[2;34mnative: property\x1b[0m] -> \x1b[36m%s\x1b[0m\n", accessor);
#endif

    // and define the native
    NUC_DEFINE_NATIVE(accessor, value);
}

/** Predefines all available standard library methods. */
static void nuc_predefineStdlib() {
    for (int i = 0; i < NUC_STDLIB_REACTIONS_LEN; i++) nuc_defineNative(nuc_nativeReactions[i].name, nuc_nativeReactions[i].native);
    for (int i = 0; i < NUC_STDLIB_PROPERTIES_LEN; i++) nuc_defineNativeProperty(nuc_nativeProperties[i].name, nuc_nativeProperties[i].value);

    /**
     * After defining simple methods/properties as global items, want to define
     * some base "models" for certain items.
     * 
     * For now, this will just be the "BaseModel" model, which will act as a
     * mutable object.
     */
    ObjString* modelName = objString_copy("BaseModel", (int)strlen("BaseModel"));
    Particle model = NUC_OBJ(model_new(modelName, NUC_MODEL_UNSTABLE));  // and generate the model

    // and now define within the atomizer
    PUSH(NUC_OBJ(modelName));
    PUSH(model);
    table_set(&atomizer.globals, AS_STRING(atomizer.stack[0]), atomizer.stack[1]);
    POP();
    POP();
}

#endif