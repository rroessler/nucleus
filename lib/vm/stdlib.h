#ifndef NUC_STDLIB_H
#define NUC_STDLIB_H

// Nucleus Headers
#include "../particle/object.h"
#include "../particle/table.h"

// Nucleus Standard Library
#include "../stdlib/native.h"

/**
 * Defines a Native Reaction for use within the Atomizer.
 * @param name                  Name of reaction.
 * @param reac                  Reaction definition.
 */
static void nuc_defineNative(const char* name, NativeRn reac) {
    PUSH(NUC_OBJ(objString_copy(name, (int)strlen(name))));
    PUSH(NUC_OBJ(native_new(reac)));
    table_set(&atomizer.globals, AS_STRING(atomizer.stack[0]), atomizer.stack[1]);
    POP();
    POP();
}

/**
 * Defines a Native Property for use within the Atomizer.
 * @param accessor              Accessor of property.
 * @param value                 Value associated with definition.
 */
static void nuc_defineNativeProperty(const char* accessor, Particle value) {
    PUSH(NUC_OBJ(objString_copy(accessor, (int)strlen(accessor))));
    PUSH(value);
    table_set(&atomizer.globals, AS_STRING(atomizer.stack[0]), atomizer.stack[1]);
    POP();
    POP();
}

/** Predefines all available standard library methods. */
static void nuc_predefineStdlib() {
    for (int i = 0; i < NUC_STDLIB_REACTIONS_LEN; i++) nuc_defineNative(nuc_nativeReactions[i].name, nuc_nativeReactions[i].native);
    for (int i = 0; i < NUC_STDLIB_PROPERTIES_LEN; i++) nuc_defineNativeProperty(nuc_nativeProperties[i].name, nuc_nativeProperties[i].value);
}

#endif