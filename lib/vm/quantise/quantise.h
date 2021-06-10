#ifndef NUC_QUANTISE_H
#define NUC_QUANTISE_H

// C Standard Library
#include <math.h>

// Nucleus Headers
#include "../global.h"
#include "stdlib.h"

// FORWARD DECLARATION
static inline nuc_Particle atomizer_resolveNative(nuc_ObjString* name);

// conditional includes
#ifdef NUC_DEBUG_PRINT_CODE
    #include "../../debug.h"
#endif

/*********************
 *  INTERNAL MACROS  *
 *********************/

/** Reads one byte from the current FRAME. */
#define READ_BYTE() (*frame->ip++)

/** Reads a short (two bytes) from the current FRAME. */
#define READ_SHORT() (frame->ip += 2, (uint16_t)(((frame->ip[-2] << 8) | frame->ip[-1])))

/** Reads an address (four bytes) from the current FRAME. */
#define READ_ADDR() (frame->ip += 4, (uint32_t)(((frame->ip[-4] << 24) | (frame->ip[-3] << 16) | (frame->ip[-2] << 8) | frame->ip[-1])))

/** Reads a constant from the available chunk constants. */
#define READ_CONSTANT() (frame->closure->reaction->chunk.constants.values[READ_SHORT()])

/** Reads a string constant from the current frame. */
#define READ_STRING() AS_STRING(READ_CONSTANT())

/** Confirms the top two items on the stack are numerics. */
#define EXPECT_NUMERICS(op)                                                                            \
    if (!IS_NUMBER(PEEK(0)) || !IS_NUMBER(PEEK(1))) {                                                  \
        atomizer_catchableError(NUC_EXIT_TYPE, "The \"" #op "\" operator expected numeric operands."); \
        break;                                                                                         \
    }

#define CONTINUE \
    continue;    \
    }

/** POPS two particles of the Stack as A and B. */
#define POP_AB()            \
    nuc_Particle b = POP(); \
    nuc_Particle a = POP()

/**
 * Allows inputing a numeric binary operation with ease.
 * @param valueType                 Type of output.
 * @param op                        Operation to complete.
 */
#define NUMERIC_BIN_OP(valueType, op)     \
    {                                     \
        /** Confirm Both Numbers. */      \
        EXPECT_NUMERICS(op);              \
                                          \
        /** Compute the binary result. */ \
        double b = AS_NUMBER(POP());      \
        double a = AS_NUMBER(POP());      \
        PUSH(valueType(a op b));          \
    }

/**
 * Allows inputing a numeric binary operation specifically cast to 32-bit integers.
 * @param valueType                  Type of output.
 * @param op                         Operation to complete.
 */
#define BITWISE_BIN_OP(valueType, op)          \
    {                                          \
        /** Confirm Both Numbers. */           \
        EXPECT_NUMERICS(op);                   \
                                               \
        /** Compute the binary result. */      \
        int32_t b = (int32_t)AS_NUMBER(POP()); \
        int32_t a = (int32_t)AS_NUMBER(POP()); \
        PUSH(valueType(a op b));               \
    }

/**
 * Coordinates operator cases.
 * @param opcode                Instruction code to match case of.
 * @param METHOD                BITWISE / NUMERIC.
 * @param type                  Return type.
 * @param op                    Operator to use.
 */
#define CASE_OPERATOR(opcode, METHOD, type, op) \
    case opcode:                                \
        METHOD##_BIN_OP(type, op);              \
        continue

/******************
 *  CASE METHODS  *
 ******************/

#include "case/operator.h"  // operator cases

/******************
 *  QUANTISATION  *
 ******************/

/**
 * Coordinates the MAIN virtual machine loop for Nucleus atomization. This method
 * iterates over a current call frame's compiled bytecode and executes instructions
 * as required. 
 */
void atomizer_quantise() {
    // set a call frame reference to use for instructions
    nuc_CallFrame* frame = &atomizer.frames[atomizer.frameCount - 1];

    for (;;) {
/*******************
 *  DEBUG DISPLAY  *
 *******************/
#ifdef NUC_DEBUG_TRACE
    #ifdef NUC_DEBUG_STACK_TRACE  // display stack
        printf("[\x1b[2;36mstack\x1b[0m]  ");
        for (nuc_Particle* slot = atomizer.stack; slot < atomizer.top; slot++) {
            printf("[ ");
            particle_print(*slot, true);
            printf(" ]");
        }
        putchar('\n');
    #endif

    // and also coordinate
    #ifdef NUC_DEBUG_OP_TRACE  // displays operation
        nuc_disassembleInstruction(
            &frame->closure->reaction->chunk,
            (int)(frame->ip - frame->closure->reaction->chunk.code),
            "\n[\x1b[2;32mtrace\x1b[0m]");
    #endif

            // putchar('\n');
#endif

        /************************
         *  OPERATION HANDLING  *
         ************************/

        uint8_t inst;  // placeholder for the current instruction
        switch (inst = READ_BYTE()) {
            /*********************
             *  MATH OPERATIONS  *
             *********************/
            CASE_OPERATOR(OP_SUB, NUMERIC, NUC_NUM, -);
            CASE_OPERATOR(OP_DIV, NUMERIC, NUC_NUM, /);
            CASE_OPERATOR(OP_XOR, BITWISE, NUC_NUM, ^);
            CASE_OPERATOR(OP_BITW_OR, BITWISE, NUC_NUM, |);
            CASE_OPERATOR(OP_BITW_AND, BITWISE, NUC_NUM, &);
            CASE_OPERATOR(OP_ROR, BITWISE, NUC_NUM, >>);
            CASE_OPERATOR(OP_ROL, BITWISE, NUC_NUM, <<);

            // modulo needs to be completed as fmod(), this can be quicker for
            // native integers on some machines so best to use it for ALL modulo
            // operations
            case OP_MOD: {
                EXPECT_NUMERICS(%);  // want to expect two numerics
                double b = POP();
                double a = POP();
                PUSH(NUC_NUM(fmod(b, a)));
                continue;
            }

            // the ADD instruction allows concatenation for strings / numerics,
            // otherwise handles straight numerics simply, and for the rest will return NULL
            case OP_ADD: {
                // if both numerics then handle immediately
                if (IS_NUMBER(PEEK(0)) && IS_NUMBER(PEEK(1))) {
                    NUMERIC_BIN_OP(NUC_NUM, +);
                    continue;
                }

                // otherwise we want a pair of either SS, SN or NS
                if (quantise_concat()) continue;  // coordinates concatentation
                break;                            // break for error handling
            }

            // the MUL instruction allows for string repetition by MULTIPLYING
            // a given string. Otherwise expects straight numerics
            case OP_MUL: {
                if (IS_STRING(PEEK(0)) && IS_NUMBER(PEEK(1))) {  // if SN
                    quantise_repeat(NUC_REPEAT_SN);
                    continue;
                } else if (IS_NUMBER(PEEK(0)) && IS_STRING(PEEK(1))) {  // or NS
                    quantise_repeat(NUC_REPEAT_NS);
                    continue;
                }

                // otherwise EXPECT numerics
                NUMERIC_BIN_OP(NUC_NUM, *);
                continue;
            }

            // NEGATION simply type checks and returns the negative of a given top of stack.
            case OP_NEGATE: {
                if (!IS_NUMBER(PEEK(0))) {
                    atomizer_catchableError(NUC_EXIT_TYPE, "The \"-\" unary operator expects a numeric operand.");
                    break;
                }
                PUSH(NUC_NUM(-AS_NUMBER(POP())));
                continue;
            }

            // BITWISE NOT simply type checks for a numeric, then casts the numeric to
            // a integer to perform bitwise logic
            case OP_BITW_NOT: {
                if (!IS_NUMBER(PEEK(0))) {
                    atomizer_catchableError(NUC_EXIT_TYPE, "The \"~\" unary operator expects a numeric operand.");
                    break;
                }
                PUSH(NUC_NUM(~(int32_t)AS_NUMBER(POP())));
                continue;
            }

            // NOT inverses and returns the boolean result of ANY particle. Empty base models,
            // empty strings, and empty arrays will return "true" here as the are logically false.
            case OP_NOT: {
                PUSH(NUC_BOOL(quantise_isFalsey(POP())));
                continue;
            }

            // EQUAL simply compares two particles are the same
            case OP_EQUAL: {
                PUSH(NUC_BOOL(quantise_isEqual(POP(), POP())));
                continue;
            }

            // LESS compares LEFT to RIGHT that (a < b). If an invalid type arrangement is given, and error is
            // set up to be catchable and can be handle IMMEDIATELY after this instruction
            case OP_LESS: {
                POP_AB();
                bool res = quantise_isLess(a, b);
                if (!NUC_CHECK_AFLAG(NUC_AFLAG_DISRUPTED)) {
                    PUSH(NUC_BOOL(res));
                    continue;
                }
            } break;

            // GREATER compares RIGHT to LEFT that (b < a). Again if an invalid type arrangement is given,
            // a type error is set up to be CAUGHT if needed
            case OP_GREATER: {
                bool res = quantise_isLess(POP(), POP());
                if (!NUC_CHECK_AFLAG(NUC_AFLAG_DISRUPTED)) {
                    PUSH(NUC_BOOL(res));
                    continue;
                }
            } break;

            /***********************************
             *  CONSTANT / LITERAL OPERATIONS  *
             ***********************************/
            case OP_CONSTANT: {
                PUSH(READ_CONSTANT());
                continue;
            }
            case OP_NULL: {
                PUSH(NUC_NULL);
                continue;
            }
            case OP_TRUE: {
                PUSH(NUC_TRUE);
                continue;
            }
            case OP_FALSE: {
                PUSH(OP_FALSE);
                continue;
            }

            /************************
             *  CONTROL OPERATIONS  *
             ************************/
            case OP_RETURN: {  // handles return keyword / exiting of script
                nuc_Particle res = POP();
                nuc_upvalue_closeAll(frame->slots);  // close the frames upvalues
                atomizer.frameCount--;               // and decrement the frame count

                // if still frames, then continue
                if (atomizer.frameCount > 0) {
                    atomizer.top = frame->slots;
                    PUSH(res);
                    frame = &atomizer.frames[atomizer.frameCount - 1];
                    continue;
                }

                // otherwise no more frames to run
                POP();
                return;
            }

            // found a request to JUMP to a given address
            case OP_JUMP: {
                uint32_t offset = READ_ADDR();
                frame->ip += offset;
                continue;
            };

            // found a request to JUMP but only if the top of the stack is false
            case OP_JUMP_IF_FALSE: {
                uint32_t offset = READ_ADDR();
                if (quantise_isFalsey(PEEK(0))) frame->ip += offset;
                continue;
            }

            // found a request to jump, otherwise to POP
            case OP_JUMP_IF_FALSE_OR_POP: {
                uint32_t offset = READ_ADDR();
                if (quantise_isFalsey(PEEK(0))) {
                    frame->ip += offset;  // if false then JUMP
                } else {
                    POP();  // otherwise want to POP
                }
                continue;
            }

            // request to LOOP so decrement to start of loop
            case OP_LOOP: {
                uint32_t offset = READ_ADDR();
                frame->ip -= offset;
                continue;
            }

            /*********************
             *  CALL OPERATIONS  *
             *********************/
            case OP_CALL: {
                int argCount = READ_BYTE();
                if (!atomizer_callValue(PEEK(argCount), argCount)) break;  // allow errors to be caught AFTER switch case
                frame = &atomizer.frames[atomizer.frameCount - 1];
                continue;  // no errors so immediately continue
            }

            // handles requests to make CLOSURES from a given reaction constant.
            case OP_CLOSURE: {
                nuc_ObjReaction* reaction = AS_REACTION(READ_CONSTANT());
                nuc_ObjClosure* closure = closure_new(reaction);
                PUSH(NUC_OBJ(closure));

                // now want to capture upvalues for the closure
                for (int i = 0; i < closure->uvCount; i++) {
                    uint8_t isLocal = READ_BYTE();
                    uint16_t index = READ_SHORT();
                    if (isLocal) {
                        closure->upvalues[i] = upvalue_capture(frame->slots + index);
                    } else {
                        closure->upvalues[i] = frame->closure->upvalues[index];
                    }
                }

                continue;
            }

            /*************************
             *  VARIABLE OPERATIONS  *
             *************************/
            case OP_POP: {
                POP();  // simply pops the top of the stack
                continue;
            }

            // Defines a GLOBAL variable that gets added to the atomizer.globals hash table
            case OP_DEFINE_GLOBAL: {
                nuc_ObjString* name = READ_STRING();
                table_set(&atomizer.globals, name, PEEK(0));
                POP();
                continue;
            }

            // Retrieves a GLOBAL variable from the globals hash table. Throws a reference error if not found.
            case OP_GET_GLOBAL: {
                nuc_ObjString* name = READ_STRING();  // read the name of the variable
                nuc_Particle value;                   // set a placeholder for the reference
                if (!table_get(&atomizer.globals, name, &value)) {
                    atomizer_catchableError(NUC_EXIT_REF, "Undefined variable reference to \"%s\".", name->chars);
                    break;  // allow error handler to complete
                }

                PUSH(value);
                continue;
            }

            // Sets a GLOBAL variable by name with the value at the top of the stack. Throws a reference
            // error if the variable is not currently defined.
            case OP_SET_GLOBAL: {
                nuc_ObjString* name = READ_STRING();  // read the name of variable
                nuc_Particle value;                   // and set placeholder for reference

                // check the value exists
                if (!table_get(&atomizer.globals, name, &value)) {
                    atomizer_catchableError(NUC_EXIT_REF, "Tried to set an undefined variable \"%s\".", name->chars);
                    table_delete(&atomizer.globals, name);  // clear up item
                    break;
                }

                // and set back to globals as confident exists
                table_set(&atomizer.globals, name, PEEK(0));
                continue;
            }

            // Gets a global based on the given slot. The slot refers to index on the stack.
            case OP_GET_LOCAL: {
                uint16_t slot = READ_SHORT();
                if (slot >= atomizer.top - frame->slots) {
                    PUSH(NUC_NULL);  // exceeded to the total slots available to push the NULL result
                    continue;
                }

                // otherwise valid retrival
                PUSH(frame->slots[slot]);
                continue;
            }

            // Sets a local variable with the current top of the atomizer stack.
            case OP_SET_LOCAL: {
                uint16_t slot = READ_SHORT();
                frame->slots[slot] = PEEK(0);
                continue;
            }

            // Retrieves an upvalue from the current frames closure. Uses the location property
            // which is a pointer reference to the upvalue.
            case OP_GET_UPVALUE: {
                uint16_t slot = READ_SHORT();
                PUSH(*frame->closure->upvalues[slot]->location);
                continue;
            }

            // Sets an upvalue by slot. Uses the location property to pass a pointer reference to
            // the new value to set.
            case OP_SET_UPVALUE: {
                uint16_t slot = READ_SHORT();
                *frame->closure->upvalues[slot]->location = PEEK(0);
                continue;
            }

            // Closes upvalues from the last position on the stack.
            case OP_CLOSE_UPVALUE: {
                nuc_upvalue_closeAll(atomizer.top - 1);
                POP();
                continue;
            }

            /*************************
             *  LIBRARY OPERTATIONS  *
             *************************/
            case OP_GET_NATIVE: {
                nuc_ObjString* name = READ_STRING();  // read the native name

                // naively attempt to retrieve the native
                nuc_Particle native;
                if (table_get(&atomizer.natives, name, &native)) {
                    PUSH(native);
                    continue;
                }

                // otherwise need to LOAD the native
                native = atomizer_resolveNative(name);
                if (NUC_CHECK_AFLAG(NUC_AFLAG_DISRUPTED)) break;
                PUSH(native);
                continue;
            }

            /**********************
             *  MODEL OPERATIONS  *
             **********************/
            case OP_MODEL: {  // creates a new model base
                PUSH(NUC_OBJ(model_new(READ_STRING())));
                continue;
            }

            // denotes for a submodel to INHERIT all methods / default fields from the parent
            case OP_INHERIT: {
                nuc_Particle base = PEEK(1);  // get the parent

                // make sure it is a model
                if (!IS_MODEL(base)) {
                    atomizer_catchableError(NUC_EXIT_TYPE, "Parent is not a model and so cannot be derived from.");
                    break;  // break to let error handler catch
                }

                nuc_ObjModel* subModel = AS_MODEL(PEEK(0));
                table_addAll(&AS_MODEL(base)->methods, &subModel->methods);
                table_addAll(&AS_MODEL(base)->defaults, &subModel->defaults);
                POP();  // remove the subModel
                continue;
            }

            // defines a METHOD for a model.
            case OP_METHOD: {
                atomizer_defineMethod(READ_STRING());
                continue;
            }

            // defines a FIELD for a model.
            case OP_FIELD: {
                atomizer_defineField(READ_STRING());
                continue;
            }

            // invokes a model METHOD by name
            case OP_INVOKE: {
                nuc_ObjString* method = READ_STRING();
                int argCount = READ_BYTE();
                if (!atomizer_invoke(method, argCount)) break;  // let error handler catch
                frame = &atomizer.frames[atomizer.frameCount - 1];
                continue;
            }

            // invokes as base model method => the SUPER invocation
            case OP_SUPER_INVOKE: {
                nuc_ObjString* method = READ_STRING();
                int argCount = READ_BYTE();
                nuc_ObjModel* base = AS_MODEL(POP());
                if (!atomizer_invokeFromModel(base, method, argCount)) break;
                frame = &atomizer.frames[atomizer.frameCount - 1];
                continue;
            }

            // gets the SUPER object of a derived model.
            case OP_GET_SUPER: {
                nuc_ObjString* name = READ_STRING();
                nuc_ObjModel* base = AS_MODEL(POP());
                if (!atomizer_bindMethod(base, name)) break;
                continue;  // bound method succeeded
            }

            // Gets a property from a given model instance. This will query the instances fields, defaults,
            // and bound methods.
            case OP_GET_PROPERTY: {
                if (!IS_INSTANCE(PEEK(0))) {
                    atomizer_catchableError(NUC_EXIT_REF, "Only model instances can have properties.");
                    break;  // and break to error handler
                }

                // grab the instance and property requested
                nuc_ObjInstance* instance = AS_INSTANCE(PEEK(0));
                nuc_ObjString* name = READ_STRING();

                // check if we have a simple property (either default / field)
                nuc_Particle value;  // temp val to save result to
                if (table_get(&instance->model->defaults, name, &value)) {
                    POP();
                    PUSH(value);
                    continue;
                } else if (table_get(&instance->fields, name, &value)) {
                    POP();
                    PUSH(value);
                    continue;
                }

                // otherwise check for bound methods
                if (!atomizer_bindMethod(instance->model, name)) {
                    POP();  // method failed so pop
                    PUSH(NUC_NULL);
                    break;  // and let error handler play with it
                }

                continue;
            }

            // operation to handle SETTING model properties.
            case OP_SET_BASE_PROPERTY:
            case OP_SET_PROPERTY: {
                if (!IS_INSTANCE(PEEK(1))) {
                    atomizer_catchableError(NUC_EXIT_TYPE, "Only model instances can have settable fields / methods.");
                    break;
                }

                // grab the accessor and add the value to the model
                nuc_ObjString* accessor = READ_STRING();
                nuc_ObjInstance* instance = AS_INSTANCE(PEEK(1));
                if (!table_set(&instance->model->defaults, accessor, PEEK(0))) {
                    table_set(&instance->fields, accessor, PEEK(0));  // TODO: work out why this ???
                }

                // and complete the following if NOT setting a base property
                if (*frame->ip == OP_SET_PROPERTY) {
                    nuc_Particle value = POP();
                    POP();
                    PUSH(value);
                }
                continue;
            }

            /** Default Case - Unknown Instruction */
            default:
                atomizer_runtimeError(NUC_EXIT_INTERNAL, "Encountered an unknown operation \x1b[33m%d\x1b[0m.", inst);
                return;  // want to IMMEDIATELY return out
        }

        // now want to check some things for our event loop
        if (!NUC_CHECK_AFLAG(NUC_AFLAG_DISRUPTED)) {
            continue;  // no errors, immediately continue
        } else if (NUC_CHECK_AFLAG(NUC_AFLAG_DISRUPTION_CATCHABLE)) {
            // error that fell through IS catchable, so we can FIX catch conditions here
            NUC_UNIMP("Catchable errors have not been implemented yet.");
            return;
        }

        // Fell through BOTH, so must have a disruption that is not caught. Hence want to STOP looping
        return;  // the exit code has been set so will be found as normal
    }
}

/*********************
 *  MACRO UNDEFINES  *
 *********************/

#undef READ_BYTE
#undef READ_SHORT
#undef READ_ADDR
#undef READ_CONSTANT
#undef READ_STRING
#undef EXPECT_NUMERICS
#undef NUMERIC_BIN_OP
#undef BITWISE_BIN_OP

#endif