#ifndef NUC_ATOMIZER_INVOKE_H
#define NUC_ATOMIZER_INVOKE_H

// Nucleus Headers
#include "../../particle/objects/type.h"
#include "../disruptions/disruption.h"
#include "call.h"

/********************
 *  INVOKE METHODS  *
 ********************/

/**
 * Invokes a model method.
 * @param model                 Model in which method occurs.
 * @param name                  Name of method.
 * @param argCount              Number of arguments.
 */
static bool atomizer_invokeFromModel(nuc_ObjModel* model, nuc_ObjString* name, int argCount) {
    nuc_Particle method;
    if (!table_get(&model->methods, name, &method)) {
        atomizer_catchableError(NUC_EXIT_REF, "Undefined model property \"%s\".", name->chars);
        return false;
    }

    // and call as a closure
    return atomizer_call(AS_CLOSURE(method), argCount);
}

/**
 * Invokes an instance method. These methods could originate from a model, or if on a reaction,
 * number or string, could be a primative method.
 * @param name              Name of invokation method.
 * @param argCount          Number of arguments given.
 */
static bool atomizer_invoke(nuc_ObjString* name, int argCount) {
    nuc_Particle receiver = PEEK(argCount);  // grab the receiver for the invocation

    // depending on the particle type
    if (IS_OBJ(receiver)) {
        if (IS_INSTANCE(receiver)) {  // if an instance, then complete as necessary
            nuc_ObjInstance* inst = AS_INSTANCE(receiver);

            nuc_Particle value;  // now want to find the called item
            if (table_get(&inst->fields, name, &value)) {
                atomizer.top[-argCount - 1] = value;  // got a valid field
                return atomizer_callValue(value, argCount);
            } else if (table_get(&inst->model->defaults, name, &value)) {
                atomizer.top[-argCount - 1] = value;  // got a valid default field
                return atomizer_callValue(value, argCount);
            }

            // otherwise try invoking a method call
            return atomizer_invokeFromModel(inst->model, name, argCount);

        } else if (IS_STRING(receiver)) {  // calls a string primative method
            atomizer_catchableError(NUC_EXIT_REF, "Unknown primative method for string particle.");
            return false;

        } else if (IS_MODEL(receiver)) {  // static model calls
            NUC_UNIMP_RETURN("Model static methods and fields have not yet been implemented.");
        }
    } else if (IS_NUMBER(receiver)) {  // calls numeric primative method
        atomizer_catchableError(NUC_EXIT_REF, "Unknown primative method for numeric particle.");
        return false;
    }

    // fall through cases chuck a wobbly
    atomizer_catchableError(NUC_EXIT_TYPE, "This variable type cannot invoke a method.");
    return false;
}

#endif