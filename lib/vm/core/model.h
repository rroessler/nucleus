#ifndef NUC_ATOMIZER_MODEL_H
#define NUC_ATOMIZER_MODEL_H

// Nucleus Includes
#include "core.h"

/**************************
 *  MODEL DEFINE METHODS  *
 **************************/

/**
 * Defines a fieldfor a model implementation.
 * @param name              Name of field to define.
 */
static inline void atomizer_defineField(nuc_ObjString* name) {
    nuc_Particle field = PEEK(0);
    nuc_ObjModel* model = AS_MODEL(PEEK(1));
    table_set(&model->defaults, name, field);
    POP();
}

/**
 * Defines a method for a model implementation.
 * @param name              Name of method to define.
 */
static inline void atomizer_defineMethod(nuc_ObjString* name) {
    nuc_Particle method = PEEK(0);
    nuc_ObjModel* model = AS_MODEL(PEEK(1));
    table_set(&model->methods, name, method);
    POP();
}

/**
 * Binds a method to global / local variable outside of the models scope.
 * @param model             Model of method.
 * @param name              Name of model method.
 */
static bool atomizer_bindMethod(nuc_ObjModel* model, nuc_ObjString* name) {
    nuc_Particle method;
    if (!table_get(&model->methods, name, &method)) {  // if fails then let user know
        atomizer_catchableError(NUC_EXIT_REF, "Tried accessing undefined model property \"%s\".", name->chars);
        return false;
    }

    // and define as a bound method
    nuc_ObjBoundMethod* bound = model_newBoundMethod(PEEK(0), AS_CLOSURE(method));
    POP();                 // pop the class instance
    PUSH(NUC_OBJ(bound));  // and push the method
    return true;
}

#endif