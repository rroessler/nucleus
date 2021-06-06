#ifndef NUC_ATOMIZER_CALL_H
#define NUC_ATOMIZER_CALL_H

// forward declaration
static void atomizer_runtimeError(const char* format, ...);

/**************
 *  CALL API  *
 **************/

/**
 * Coordinates a atomizer call routine.
 * @param closure           Closure to call.
 * @param argCount          Number of arguments given.
 */
static bool atomizer_call(ObjClosure* closure, int argCount) {
    // make sure number of reactions is valid
    if (argCount < closure->reac->arity - closure->reac->defaults) {  // not enough
        atomizer_runtimeError("Expected at least %d arguments for reaction call but got %d.", closure->reac->arity, argCount);
        return false;
    } else if (argCount > closure->reac->arity) {  // to many
        atomizer_runtimeError("Expected at most %d arguments for reaction call but got %d.", closure->reac->arity, argCount);
        return false;
    }

    // and that we also haven't exceeded the maximum number of frames
    if (atomizer.frameCount == FRAMES_MAX) {
        atomizer_runtimeError("Stack overflow!");
        return false;
    }

    CallFrame* frame = &atomizer.frames[atomizer.frameCount++];
    frame->closure = closure;
    frame->ip = closure->reac->chunk.code;
    frame->slots = atomizer.stackTop - argCount - 1;
    return true;
}

/**
 * Sets a current reaction to be called.
 * @param callee            Particle calling reaction.
 * @param argCount          Number of arguments given.
 */
static bool atomizer_callValue(Particle callee, int argCount) {
    if (IS_OBJ(callee)) {  // type checking
        switch (OBJ_TYPE(callee)) {
            case OBJ_NATIVE: {
                NativeRn native = AS_NATIVE(callee);
                Particle result = native(argCount, atomizer.stackTop - argCount);
                atomizer.stackTop -= argCount + 1;
                PUSH(result);
                return true;
            }

            case OBJ_CLOSURE:  // call similarly to reaction
                return atomizer_call(AS_CLOSURE(callee), argCount);

            case OBJ_MODEL: {
                // create the new instance
                ObjModel* model = AS_MODEL(callee);
#ifdef NUC_NAN_BOXING
                Particle inst = NUC_OBJ(model_newInstance(model));
                AS_OBJ(inst)->mutable = model->unstable;
                atomizer.stackTop[-argCount - 1] = inst;
#else
                atomizer.stackTop[-argCount - 1] =
                    model->unstable
                        ? NUC_OBJ_MUTABLE(model_newInstance(model))
                        : NUC_OBJ(model_newInstance(model));
#endif

                // and invoke the constructor
                Particle constructor;
                if (table_get(&model->methods, atomizer.initString, &constructor)) {
                    return atomizer_call(AS_CLOSURE(constructor), argCount);
                } else if (argCount != 0) {
                    atomizer_runtimeError("Expected 0 arguments for model without constructor.");
                    return false;
                }

                return true;
            }

            case OBJ_BOUND_METHOD: {
                ObjBoundMethod* bound = AS_BOUND_METHOD(callee);
                atomizer.stackTop[-argCount - 1] = bound->receiver;
                return atomizer_call(bound->method, argCount);
            }

            default:  // otherwise non-callable
                break;
        }
    }

    atomizer_runtimeError("Can only call reactions.");
    return false;  // failed
}

/****************
 *  INVOKE API  *
 ****************/

/**
 * Invokes a model method.
 * @param model                 Model in which method occurs.
 * @param name                  Name of method.
 * @param argCount              Number of arguments.
 */
static bool atomizer_invokeFromModel(ObjModel* model, ObjString* name, int argCount) {
    Particle method;
    if (!table_get(&model->methods, name, &method)) {
        atomizer_runtimeError("Undefined model property \"%s\".", name->chars);
        return false;
    }

    // and call as a closure
    return atomizer_call(AS_CLOSURE(method), argCount);
}

/**
 * Invokes an instance method. These methods could originate from a model, or if on a reaction,
 * number or string, could be native methods.
 * @param name              Name of invokation method.
 * @param argCount          Number of arguments given.
 */
static bool atomizer_invoke(ObjString* name, int argCount) {
    Particle receiver = atomizer_peek(argCount);  // grab the receiver

// depending on the item received
#ifdef NUC_NAN_BOXING
    if (IS_OBJ(receiver) && IS_INSTANCE(receiver)) {
        ObjInstance* inst = AS_INSTANCE(receiver);

        Particle value;
        if (table_get(&inst->fields, name, &value)) {
            atomizer.stackTop[-argCount - 1] = value;
            return atomizer_callValue(value, argCount);
        } else if (table_get(&inst->model->defaults, name, &value)) {
            atomizer.stackTop[-argCount - 1] = value;
            return atomizer_callValue(value, argCount);
        }

        return atomizer_invokeFromModel(inst->model, name, argCount);
    } else if (IS_NUMBER(receiver)) {
    }
#else
    switch (receiver.type) {
        case TYPE_OBJ: {  // invoke specific object methods
            if (!IS_INSTANCE(receiver)) break;
            ObjInstance* inst = AS_INSTANCE(receiver);

            Particle value;
            if (table_get(&inst->fields, name, &value)) {
                atomizer.stackTop[-argCount - 1] = value;
                return atomizer_callValue(value, argCount);
            } else if (table_get(&inst->model->defaults, name, &value)) {
                atomizer.stackTop[-argCount - 1] = value;
                return atomizer_callValue(value, argCount);
            }

            return atomizer_invokeFromModel(inst->model, name, argCount);
        }

        // invoke numeric instance methods
        case TYPE_NUMBER:

        default:
            break;
    }
#endif

    // fall through cases chuck a wobbly
    atomizer_runtimeError("This variable type cannot invoke a method.");
    return false;
}

/****************
 *  DEFINE API  *
 ****************/

/**
 * Defines a fieldfor a model implementation.
 * @param name              Name of field to define.
 */
static void atomizer_defineField(ObjString* name) {
    Particle field = atomizer_peek(0);
    ObjModel* model = AS_MODEL(atomizer_peek(1));
    table_set(&model->defaults, name, field);
    POP();
}

/**
 * Defines a method for a model implementation.
 * @param name              Name of method to define.
 */
static void atomizer_defineMethod(ObjString* name) {
    Particle method = atomizer_peek(0);
    ObjModel* model = AS_MODEL(atomizer_peek(1));
    table_set(&model->methods, name, method);
    POP();
}

/**
 * Binds a method to global / local variable outside of the models scope.
 * @param model             Model of method.
 * @param name              Name of model method.
 */
static bool atomizer_bindMethod(ObjModel* model, ObjString* name) {
    Particle method;
    if (!table_get(&model->methods, name, &method)) {  // if fails then let user know
        atomizer_runtimeError("Tried accessing undefined property \"%s\".", name->chars);
        return false;
    }

    // and define as a bound method
    ObjBoundMethod* bound = model_newBoundMethod(atomizer_peek(0), AS_CLOSURE(method));
    POP();                 // pop the class instance
    PUSH(NUC_OBJ(bound));  // and push the method
    return true;
}

#endif