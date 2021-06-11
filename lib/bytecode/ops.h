#ifndef NUC_OPS_H
#define NUC_OPS_H

/** Nucleus Bytecode Operations */
typedef enum {
    /** Literals */
    OP_FALSE,  // creates a local 'false' particle
    OP_TRUE,   // creates a local 'true' particle
    OP_NULL,   // creates a local 'null' particle

    /** Math Operations */
    OP_NEGATE,    // - (unary)
    OP_ADD,       // + (binary)
    OP_SUB,       // - (binary)
    OP_MUL,       // * (binary)
    OP_DIV,       // / (binary)
    OP_MOD,       // % (binary)
    OP_POW,       // ** (binary)
    OP_XOR,       // ^ (binary)
    OP_BITW_OR,   // | (binary)
    OP_BITW_AND,  // & (binary)
    OP_BITW_NOT,  // ~ (unary)
    OP_ROL,       // << (binary)
    OP_ROR,       // >> (binary)

    /** Comparisons */
    OP_NOT,      // ! (unary)
    OP_EQUAL,    // = (compares two locals)
    OP_GREATER,  // > (compares two locals)
    OP_LESS,     // < (compares two locals)

    /** Internal Control Operations */
    OP_POP,
    OP_CATCH_MODE,
    OP_END_CATCH_MODE,
    OP_RETURN,

    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_JUMP_IF_FALSE_OR_POP,
    OP_JUMP_CATCH,
    OP_LOOP,

    /** Variable Operations */
    OP_CONSTANT,
    OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_SET_LOCAL,
    OP_GET_LOCAL,
    OP_SET_UPVALUE,
    OP_GET_UPVALUE,
    OP_CLOSE_UPVALUE,

    /** Reaction / Property Operations */
    OP_CALL,
    OP_CLOSURE,
    OP_INVOKE,
    OP_FIELD,
    OP_METHOD,
    OP_GET_PROPERTY,
    OP_SET_PROPERTY,
    OP_SET_BASE_PROPERTY,

    /** Array Operations */
    OP_ARRAY,
    OP_GET_MEMBER,
    OP_SET_MEMBER,

    /** Model Specific Operations */
    OP_MODEL,
    OP_INHERIT,
    OP_GET_SUPER,
    OP_SUPER_INVOKE,

    /** Directives */
    OP_MUTATE,
    OP_SET_IMMUTABLE,

    /** Library Methods */
    OP_GET_NATIVE,

} OpCode;

#endif