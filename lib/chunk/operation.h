#ifndef NUC_OPERATION_H
#define NUC_OPERATION_H

// C Standard Library
#include <stdio.h>

// Nucleus Headers
#include "../common.h"

/** Nucleus Bytecode Operations */
typedef enum {
    // constants / literals
    OP_CONSTANT,
    OP_FALSE,
    OP_TRUE,
    OP_NULL,

    // math operations
    OP_NEGATE,    // - (unary)
    OP_ADD,       // + (binary)
    OP_SUB,       // - (binary)
    OP_MUL,       // * (binary)
    OP_DIV,       // / (binary)
    OP_MOD,       // % (binary)
    OP_XOR,       // ^ (binary)
    OP_BITW_OR,   // | (binary)
    OP_BITW_AND,  // & (binary)
    OP_ROL,       // << (binary)
    OP_ROR,       // >> (binary)

    // equality and comparisons
    OP_NOT,      // !
    OP_EQUAL,    // =
    OP_GREATER,  // >
    OP_LESS,     // <

    // control operations
    OP_POP,
    OP_RETURN,
    OP_JUMP_IF_FALSE_OR_POP,
    OP_JUMP_IF_FALSE,
    OP_JUMP,
    OP_LOOP,
    OP_CALL,
    OP_CLOSURE,

    // atomizer specific operations
    OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_SET_LOCAL,
    OP_GET_LOCAL,
    OP_SET_UPVALUE,
    OP_GET_UPVALUE,
    OP_CLOSE_UPVALUE,

    // library operations
    OP_PRINT,
} OpCode;

#endif