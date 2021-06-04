#ifndef NUC_ATOMIZER_H
#define NUC_ATOMIZER_H

// C Standard Library
#include <stdarg.h>
#include <stdio.h>

// Nucleus Headers
#include "../chunk/chunk.h"
#include "../chunk/operation.h"
#include "../compiler/fuser.h"
#include "../memory.h"
#include "../particle/duality.h"
#include "../particle/print.h"
#include "../particle/table.h"
#include "flags.h"
#include "state.h"

// conditional includes
#ifdef NUC_DEBUG_PRINT_CODE
    #include "../debug.h"
#endif

/*************
 *  GLOBALS  *
 *************/

/** Atomizer Virtual Machine */
typedef struct {
    // bytecode manipulation
    Chunk* chunk;
    uint8_t* ip;

    // call frame manipulation
    CallFrame frames[FRAMES_MAX];
    int frameCount;
    ObjUpvalue* openUpvalues;  // pointer list of currently open upvalues

    // stack variables
    Particle stack[STACK_MAX];
    Particle* stackTop;  // pointer to top of stack

    // global variables
    Obj* objects;
    Table globals;
    Table strings;  // for string interning => makes common strings quickly accesible

    // garbage collection
    int grayCount;
    int grayCapacity;
    Obj** grayStack;
    size_t bytesAllocated;
    size_t nextGC;

    // atomizer flags
    uint32_t flags;
} Atomizer;

/** Global Atomizer Instance */
Atomizer atomizer;

/************************************
 *  METHODS THAT REQUIRED ATOMIZER  *
 ************************************/

// these are methods that need to be declared here, as they
// require access to the "atomizer" global structure.

/**
 * Allocates an object of given size and type into memory.
 * @param size              Size of object.
 * @param type              Internal object type.
 */
static Obj* obj_alloc(size_t size, ObjType type) {
    Obj* object = (Obj*)nuc_realloc(NULL, 0, size);
    object->type = type;
    object->isMarked = false;
    object->mutable = false;

    // set up singley linked list
    object->next = atomizer.objects;
    atomizer.objects = object;

#ifdef NUC_DEBUG_LOG_GC
    printf("%p allocate %zu for %d\n", (void*)object, size, type);
#endif

    return object;
}

/** Frees all objects from singley linked list. */
void obj_freeAll() {
    Obj* object = atomizer.objects;
    while (object != NULL) {
        Obj* next = object->next;
        obj_free(object);
        object = next;
    }
}

/**
 * Captures and creates an upvalue given by local variable.
 * @param local             Local variable to capture as an upvalue.
 */
static ObjUpvalue* upvalue_capture(Particle* local) {
    ObjUpvalue* prevUV = NULL;
    ObjUpvalue* uv = atomizer.openUpvalues;
    while (uv != NULL && uv->location > local) {
        prevUV = uv;
        uv = uv->next;
    }

    // if we have found our upvalue as already open, then return
    if (uv != NULL && uv->location == local) return uv;

    // create a captured upvalue
    ObjUpvalue* createdUV = upvalue_new(local);
    if (prevUV == NULL) {
        atomizer.openUpvalues = createdUV;
    } else {
        prevUV->next = createdUV;
    }

    return createdUV;
}

/**
 * Closes all open upvalues up to a set last upvalue.
 * @param last              Last valid upvalue to not close.
 */
static void upvalue_closeAll(Particle* last) {
    // while available to close
    while (atomizer.openUpvalues != NULL && atomizer.openUpvalues->location >= last) {
        ObjUpvalue* uv = atomizer.openUpvalues;
        uv->closed = *uv->location;
        uv->location = &uv->closed;
        atomizer.openUpvalues = uv->next;
    }
}

// string interning
#include "intern.h"

/************************
 *  STACK MANIPULATION  *
 ************************/

/** Resets the current atomizers stack. */
static void atomizer_resetStack() {
    atomizer.stackTop = atomizer.stack;
    atomizer.frameCount = 0;
}

/**
 * Pushes a particle value to the Atomizer stack.
 * @param value             Particle to push.
 */
void atomizer_push(Particle value) {
    *atomizer.stackTop = value;
    atomizer.stackTop++;
}

/** Pops the top particle value from the Atomizer stack. */
Particle atomizer_pop() {
    atomizer.stackTop--;
    return *atomizer.stackTop;
}

/**
 * Sets a given atomizer flag.
 * @param flag              Flag to set.
 */
static void atomizer_setFlag(uint32_t flag) { atomizer.flags |= flag; }

/**
 * Unsets a given atomizer flag.
 * @param flag              Flag to set.
 */
static void atomizer_unsetFlag(uint32_t flag) { atomizer.flags &= ~flag; }

/** Resets all atomizer flags. */
static void atomizer_resetFlags() { atomizer.flags = NUC_AFLAG_RESET; }

/**
 * Returns if a flag has been set.
 * @param flag              Flag to check.
 */
static bool atomizer_checkFlag(uint32_t flag) { return (atomizer.flags & flag) != 0; }

/** Macro to help push atomizer values. */
#define PUSH(val) atomizer_push(val)

/** Macro to help pop atomizer values. */
#define POP(val) atomizer_pop(val)

/**
 * Peeks a value at a given distance on the stack.
 * @param distance          Distance of item from top of stack.
 */
static Particle atomizer_peek(int distance) { return atomizer.stackTop[-1 - distance]; }

/******************
 *  ATOMIZER API  *
 ******************/

// additional atomizer API, forward declared
#include "gc.h"
#include "math.h"
#include "stdlib.h"

/** Initialises the Atomizer Instance */
void atomizer_init() {
    atomizer_resetStack();
    atomizer.objects = NULL;
    atomizer.openUpvalues = NULL;
    table_init(&atomizer.globals);
    table_init(&atomizer.strings);

    // initialise garbage collection variables
    atomizer.grayCount = 0;
    atomizer.grayCapacity = 0;
    atomizer.grayStack = NULL;
    atomizer.bytesAllocated = 0;
    atomizer.nextGC = 1024 * 1024;

    // initialise the atomizer flags
    atomizer_resetFlags();

    // define all native standard library methods
    nuc_predefineStdlib();
}

/** Frees the Atomizer Instance */
void atomizer_free() {
    table_free(&atomizer.globals);
    table_free(&atomizer.strings);
    obj_freeAll();
    free(atomizer.grayStack);
}

/**
 * Throws an error at runtime.
 * @param format                Format string to use.
 * @param args                  Input arguments
 */
static void atomizer_runtimeError(const char* format, ...) {
    fputs("\n\x1b[1;31mError:\x1b[0m ", stderr);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    for (int i = atomizer.frameCount - 1; i >= 0; i--) {
        CallFrame* frame = &atomizer.frames[i];
        ObjReaction* reac = frame->closure->reac;
        size_t inst = frame->ip - reac->chunk.code - 1;

        // and retrieve some items for displaying the called line
        int line = reac->chunk.lines[inst];
        const char* source = lexer_getLine(line);

        fprintf(stderr, "[\x1b[2mline\x1b[0m \x1b[33m%d\x1b[0m] ", line);
        if (reac->name == NULL) {
            fprintf(stderr, "in \x1b[32mscript\x1b[0m.");
        } else {
            fprintf(stderr, "at \x1b[3;31m\"%s()\"\x1b[0m.", reac->name->chars);
        }

        // and now printing a TRIMMED source
        fprintf(stderr, "\n[\x1b[2;36msource\x1b[0m] \x1b[36m`");
        char* ptr = NULL;
        while (isspace((unsigned)*source)) source++;     // chomp at start of source
        ptr = (char*)source + strlen(source) - 1;        // jump to the last char of source
        while (isspace((unsigned)*ptr)) ptr--;           // trim the end of the string
        while (source <= ptr) fputc(*source++, stderr);  // and
        fprintf(stderr, "`\x1b[0m\n");
    }
    fputs("\n", stderr);

    // and clean the atomizer stack
    atomizer_resetStack();
}

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
                ObjModel* model = AS_MODEL(callee);
                atomizer.stackTop[-argCount - 1] =
                    model->unstable
                        ? NUC_OBJ_MUTABLE(model_newInstance(model))
                        : NUC_OBJ(model_newInstance(model));
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

/**
 * Disrupts the atomization if a particle is immutable and there was an attempt
 * to set a new value to it.
 * @param value             Particle to check.
 */
#define NUC_DISRUPT_IF_IMMUTABLE(value)                                    \
    if (!value.mutable) {                                                  \
        if (!atomizer_checkFlag(NUC_AFLAG_NEXT_SET_MUTABLE)) {             \
            atomizer_runtimeError("Cannot modify an immutable particle."); \
            return ASTATE_RUNTIME_UNSTABLE;                                \
        } else {                                                           \
            /** Unset the flag after checking */                           \
            atomizer_unsetFlag(NUC_AFLAG_NEXT_SET_MUTABLE);                \
        }                                                                  \
    }

/** 
 * Runs the MAIN Virtual machine loop, iterating over all bytecode instructions 
 * and completing their execution. 
 */
static ATOMIC_STATE quantize() {
    // grab the current CALL FRAME
    CallFrame* frame = &atomizer.frames[atomizer.frameCount - 1];

/** Reads a single byte from the Atomizer. */
#define READ_BYTE() (*frame->ip++)

/** Reads a short (2 bytes) from the atomizer into a single value. */
#define READ_SHORT() (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8 | frame->ip[-1])))

/** Reads a constant from the current atomizer chunk constants. */
#define READ_CONSTANT() (frame->closure->reac->chunk.constants.values[READ_BYTE()])

/** Macro to help easily read strings. */
#define READ_STRING() AS_STRING(READ_CONSTANT())

/** Allows inputing a binary operation easily. */
#define BINARY_OP(valueType, op)                                            \
    do {                                                                    \
        /** Confirm both Numbers. */                                        \
        if (!IS_NUMBER(atomizer_peek(0)) || !IS_NUMBER(atomizer_peek(1))) { \
            atomizer_runtimeError("Operands must both be numbers.");        \
            return ASTATE_RUNTIME_UNSTABLE;                                 \
        }                                                                   \
                                                                            \
        /** And determine binary result. */                                 \
        double b = AS_NUMBER(POP());                                        \
        double a = AS_NUMBER(POP());                                        \
        PUSH(valueType(a op b));                                            \
    } while (false)

/** Simplifies writing out Binary Operation Cases */
#define CASE_BINARY(opcode, valueType, op) \
    case opcode:                           \
        BINARY_OP(valueType, op);          \
        break

    // iterate ad-finitum
    for (;;) {
#ifdef NUC_DEBUG_TRACE_EXECUTION
        // coordinate some stack tracing
        printf("[\x1b[2;36mstack\x1b[0m]  ");
        for (Particle* slot = atomizer.stack; slot < atomizer.stackTop; slot++) {
            printf("[ ");
            particle_print(*slot, true);
            printf(" ]");
        }
        printf("\n");

        // want to coordinate some debug tracing if desired
        nuc_disassembleInstruction(
            &frame->closure->reac->chunk,
            (int)(frame->ip - frame->closure->reac->chunk.code),
            "[\x1b[2;32mtrace\x1b[0m]");
#endif

        uint8_t inst;  // placeholder for the current instruction
        switch (inst = READ_BYTE()) {
            /** Math Operations */
            // CASE_BINARY(OP_ADD, NUC_NUMBER, +);
            CASE_BINARY(OP_SUB, NUC_NUMBER, -);
            CASE_BINARY(OP_MUL, NUC_NUMBER, *);
            CASE_BINARY(OP_DIV, NUC_NUMBER, /);
            CASE_BINARY(OP_GREATER, NUC_BOOL, >);
            CASE_BINARY(OP_LESS, NUC_BOOL, <);
            case OP_ADD: {
                // check if we can complete this with strings
                if (IS_STRING(atomizer_peek(0)) && IS_STRING(atomizer_peek(1))) {
                    atomizer_concat();
                    break;
                }

                // otherwise complete as a binary operation for numbers
                BINARY_OP(NUC_NUMBER, +);
            } break;

            case OP_NEGATE:
                if (!IS_NUMBER(atomizer_peek(0))) {
                    atomizer_runtimeError("Operand must be a number.");
                    return ASTATE_RUNTIME_UNSTABLE;
                }
                PUSH(NUC_NUMBER(-AS_NUMBER(POP())));
                break;
            case OP_NOT:
                PUSH(NUC_BOOL(particle_isFalsey(POP())));
                break;
            case OP_EQUAL: {  // pop both items off and evaluate
                Particle b = POP();
                Particle a = POP();
                PUSH(NUC_BOOL(particle_isEqual(a, b)));
            } break;

            /** Constant / Literal Operations */
            case OP_CONSTANT: {
                Particle constant = READ_CONSTANT();
                PUSH(constant);
            } break;
            case OP_NULL:
                PUSH(NUC_NULL);
                break;
            case OP_TRUE:
            case OP_FALSE:
                PUSH(NUC_BOOL(inst == OP_TRUE));
                break;

            /** Directive Operations */
            case OP_MUTATE:
                atomizer_setFlag(NUC_AFLAG_NEXT_SET_MUTABLE);
                break;

            /** Control Operations */
            case OP_RETURN: {  // request to complete quantization
                Particle res = POP();
                upvalue_closeAll(frame->slots);  // close all upvalues here
                atomizer.frameCount--;

                // if no frames then exit
                if (atomizer.frameCount == 0) {
                    POP();
                    return ASTATE_STABLE;
                }

                // otherwise push the current return result
                atomizer.stackTop = frame->slots;
                PUSH(res);
                frame = &atomizer.frames[atomizer.frameCount - 1];
            } break;
            case OP_JUMP_IF_FALSE_OR_POP: {
                uint16_t offset = READ_SHORT();
                if (particle_isFalsey(atomizer_peek(0))) {
                    frame->ip += offset;  // if false then JUMP
                } else {
                    POP();  // otherwise POP the result
                }
            } break;
            case OP_JUMP_IF_FALSE: {  // jump only if top of stack is false
                uint16_t offset = READ_SHORT();
                if (particle_isFalsey(atomizer_peek(0))) frame->ip += offset;  // if false then JUMP
            } break;
            case OP_JUMP: {  // request to jump
                uint16_t offset = READ_SHORT();
                frame->ip += offset;
                break;
            } break;
            case OP_LOOP: {
                uint16_t offset = READ_SHORT();
                frame->ip -= offset;
                break;
            } break;
            case OP_CALL: {
                int argCount = READ_BYTE();
                if (!atomizer_callValue(atomizer_peek(argCount), argCount)) return ASTATE_RUNTIME_UNSTABLE;
                frame = &atomizer.frames[atomizer.frameCount - 1];
            } break;
            case OP_CLOSURE: {
                ObjReaction* reac = AS_REACTION(READ_CONSTANT());
                ObjClosure* closure = closure_new(reac);
                PUSH(NUC_OBJ(closure));

                // now want to capture upvalues
                for (int i = 0; i < closure->uvCount; i++) {
                    uint8_t isLocal = READ_BYTE();
                    uint8_t index = READ_BYTE();
                    if (isLocal) {
                        closure->upvalues[i] = upvalue_capture(frame->slots + index);
                        continue;
                    }

                    closure->upvalues[i] = frame->closure->upvalues[index];
                }
            } break;

            /** Model Based Operations */
            case OP_MODEL: {
                PUSH(NUC_OBJ(model_new(READ_STRING(), NUC_MODEL_STABLE)));
            } break;
            case OP_METHOD:
                atomizer_defineMethod(READ_STRING());
                break;
            case OP_FIELD:
                atomizer_defineField(READ_STRING());
                break;
            case OP_GET_PROPERTY: {
                // for now only allow instances, however LATER, this can be extended to CALL
                // internal object properties/methods (eg: ".toString()" for numbers)
                if (!IS_INSTANCE(atomizer_peek(0))) {
                    atomizer_runtimeError("Only model instances can have properties.");
                    return ASTATE_RUNTIME_UNSTABLE;
                }

                ObjInstance* inst = AS_INSTANCE(atomizer_peek(0));
                ObjString* name = READ_STRING();

                // check if a model field
                Particle value;  // temporary value to write to
                if (table_get(&inst->model->defaults, name, &value)) {
                    POP();
                    PUSH(value);
                    break;
                } else if (table_get(&inst->fields, name, &value)) {
                    POP();
                    PUSH(value);
                    break;
                }

                // and now check for bound methods
                if (!atomizer_bindMethod(inst->model, name)) {
                    POP();  // bind method failed, so push a null particle
                    PUSH(NUC_NULL);
                }
            } break;
            case OP_SET_PROPERTY: {
                // inversely, ONLY instances will be allowed to have properties set
                if (!IS_INSTANCE(atomizer_peek(1))) {
                    atomizer_runtimeError("Only model instances can have fields.");
                    return ASTATE_RUNTIME_UNSTABLE;
                }

                // make sure the instance is mutable
                NUC_DISRUPT_IF_IMMUTABLE(atomizer_peek(1));
                ObjString* accessor = READ_STRING();

                ObjInstance* inst = AS_INSTANCE(atomizer_peek(1));
                if (!table_set(&inst->model->defaults, accessor, atomizer_peek(0))) {
                    table_set(&inst->fields, accessor, atomizer_peek(0));
                }
                Particle value = POP();
                POP();
                PUSH(value);
            } break;

            /** Atomizer Specific Operations */
            case OP_POP:  // simply pops the current top of the stack
                POP();
                break;
            case OP_DEFINE_GLOBAL: {
                ObjString* name = READ_STRING();
                table_set(&atomizer.globals, name, atomizer_peek(0));
                POP();
            } break;
            case OP_SET_IMMUTABLE: {
                Particle value = POP();
                value.mutable = false;
                PUSH(value);
            } break;
            case OP_GET_GLOBAL: {
                ObjString* name = READ_STRING();  // read the name of the variable
                Particle value;                   // placeholder for value to read
                if (!table_get(&atomizer.globals, name, &value)) {
                    atomizer_runtimeError("Undefined variable \"%s\".", name->chars);
                    return ASTATE_RUNTIME_UNSTABLE;
                }

                // and push the value onto the stack
                PUSH(value);
            } break;
            case OP_SET_GLOBAL: {
                ObjString* name = READ_STRING();  // get global to set

                // make sure the item is mutable, to do this need to GET the value first
                Particle value;
                if (!table_get(&atomizer.globals, name, &value)) {
                    atomizer_runtimeError("Tried to set undefined variable \"%s\".", name->chars);
                    table_delete(&atomizer.globals, name);  // clean up
                    return ASTATE_RUNTIME_UNSTABLE;
                }

                // and disrupt if it is immutable
                NUC_DISRUPT_IF_IMMUTABLE(value);

                // and set to globals confidently as could not have passed earlier
                table_set(&atomizer.globals, name, atomizer_peek(0));
            } break;
            case OP_GET_LOCAL: {  // save a local variable to the top of the stack
                uint8_t slot = READ_BYTE();
                PUSH(frame->slots[slot]);
            } break;
            case OP_SET_LOCAL: {
                uint8_t slot = READ_BYTE();
                NUC_DISRUPT_IF_IMMUTABLE(atomizer_peek(0));
                frame->slots[slot] = atomizer_peek(0);
            } break;
            case OP_GET_UPVALUE: {
                uint8_t slot = READ_BYTE();
                PUSH(*frame->closure->upvalues[slot]->location);
            } break;
            case OP_SET_UPVALUE: {
                uint8_t slot = READ_BYTE();
                NUC_DISRUPT_IF_IMMUTABLE(atomizer_peek(0));
                *frame->closure->upvalues[slot]->location = atomizer_peek(0);
            } break;
            case OP_CLOSE_UPVALUE: {
                upvalue_closeAll(atomizer.stackTop - 1);
                POP();
            } break;

            /** Library Operations */
            case OP_PRINT: {
                Particle val = POP();
                if (IS_STRING(val)) {  // if is a string, want to do so normally (here at least)
                    printf("%s", AS_CSTRING(val));
                } else {  // otherwise print as expected
                    particle_print(val, true);
                }
            } break;

            default:  // if a bad operation occurs
                atomizer_runtimeError("Encountered an unknown operation.");
                return ASTATE_RUNTIME_UNSTABLE;
        }
    }

// undefining localised macros
#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef READ_STRING
#undef BINARY_OP
#undef CASE_BINARY
}

/**
 * Callable method to run a virtual machine instance.
 * @param source                Nucleus source code to run.
 */
ATOMIC_STATE atomize(const char* source) {
    ObjReaction* reac = fuse(source);
    if (reac == NULL) return ASTATE_COMPILE_UNSTABLE;

    PUSH(NUC_OBJ(reac));
    ObjClosure* closure = closure_new(reac);
    POP();
    PUSH(NUC_OBJ(closure));
    atomizer_call(closure, 0);

    // and process
    return quantize();
}

#endif