#ifndef NUC_ATOMIZER_CALL_H
#define NUC_ATOMIZER_CALL_H

// Nucleus Headers
#include "../../common.h"
#include "../disruptions/disruption.h"

/******************
 *  CALL METHODS  *
 ******************/

/**
 * Calls a given closure object's reaction. Also denotes a given argument count.
 * @param closure                       Closure to call reaction of.
 * @param argCount                      Total arguments given.
 */
static bool atomizer_call(nuc_ObjClosure* closure, int argCount) {
    // make sure the number of reactions is valid
    if (argCount < closure->reaction->arity - closure->reaction->defaults) {
        // missing arguments
        atomizer_catchableError(NUC_EXIT_ARG, "Expected at least %d arguments for reaction call but got %d.", closure->reaction->arity, argCount);
        return false;
    } else if (argCount > closure->reaction->arity) {
        // too many arguments
        atomizer_catchableError(NUC_EXIT_ARG, "Expected at most %d arguments for reaction call but got %d.", closure->reaction->arity, argCount);
        return false;
    }

    // check that the total number of frames hasn't been exceeded
    if (atomizer.frameCount == FRAMES_MAX) {
        atomizer_runtimeError(NUC_EXIT_OVERFLOW, "Exceeded maximum number of call frames.");
        return false;
    }

    // call the reaction by setting up a nested frame
    nuc_CallFrame* frame = &atomizer.frames[atomizer.frameCount++];
    frame->closure = closure;
    frame->ip = closure->reaction->chunk.code;
    frame->slots = atomizer.top - argCount - 1;
    return true;
}

/**
 * Calls a given particle value. If not a callable particle, then throws a runtime error.
 * @param callee                Particle that can be called.
 * @param argCount              Total arguments given for call.
 */
static bool atomizer_callValue(nuc_Particle callee, int argCount) {
    if (IS_OBJ(callee)) {
        // switch based on the given callee's object typing
        switch (OBJ_TYPE(callee)) {
            case OBJ_NATIVE: {  // want to call the item as a native method
                nuc_NativeReaction native = AS_NATIVE(callee);
                nuc_Particle result = native(argCount, atomizer.top - argCount);
                if (!NUC_CHECK_AFLAG(NUC_AFLAG_DISRUPTED)) {
                    atomizer.top -= argCount + 1;
                    PUSH(result);
                } else {
                    nuc_Particle disruption = POP();
                    atomizer.top -= argCount + 1;
                    PUSH(disruption);
                }
                return !NUC_CHECK_AFLAG(NUC_AFLAG_DISRUPTED);
            }

            case OBJ_CLOSURE:  // can simple call as a closure
                return atomizer_call(AS_CLOSURE(callee), argCount);

            case OBJ_MODEL: {  // creates a new instance of a model
                nuc_ObjModel* base = AS_MODEL(callee);
                nuc_Particle instance = NUC_OBJ(model_newInstance(base));
                atomizer.top[-argCount - 1] = instance;

                // and invoke the constructor
                nuc_Particle constructor;
                if (table_get(&base->methods, atomizer.constructor, &constructor)) {
                    return atomizer_call(AS_CLOSURE(constructor), argCount);
                } else if (argCount != 0) {  // otherwise is no constructor but given values
                    atomizer_catchableError(NUC_EXIT_ARG, "Expected no arguments for a model without a constructor.");
                    return false;
                }

                // and denote success
                return true;
            }

            case OBJ_BOUND_METHOD: {  // calls a model method bound to a generic variable
                nuc_ObjBoundMethod* bound = AS_BOUND_METHOD(callee);
                atomizer.top[-argCount - 1] = bound->receiver;
                return atomizer_call(bound->method, argCount);
            }

            default:  // otherwise is uncallable
                break;
        }
    }

    // not an object so cannot confirm if valid to call
    atomizer_catchableError(NUC_EXIT_TYPE, "Cannot call given particle. Only reactions can be called.");
    return false;  // denote failed
}

#endif