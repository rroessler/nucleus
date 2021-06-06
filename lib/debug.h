#ifndef NUC_DEBUG_H
#define NUC_DEBUG_H

// C Standard Library
#include <stdio.h>

// Nucleus Headers
#include "chunk/chunk.h"
#include "chunk/operation.h"
#include "particle/print.h"

// operation padding length
#define PRINT_OP_PAD_LEN 34

/**
 * Prints a jump instruction.
 * @param name              Name of instruction.
 * @param sign              Sign of instruction.
 * @param chunk             Chunk in which instruction occurs.
 * @param offset            Offset of instruction.
 */
static int nuc_printJumpInstruction(const char* name, int sign, Chunk* chunk, int offset) {
    uint16_t jump = (uint16_t)(chunk->code[offset + 1] << 8);
    jump |= chunk->code[offset + 2];
    printf("%-*s \x1b[33m%.4X\x1b[0m \x1b[2m>\x1b[0m \x1b[33m%.4X\n\x1b[0m", PRINT_OP_PAD_LEN, name, offset, offset + 3 + sign * jump);
    return offset + 3;
}

/**
 * Prints an instruction type simply.
 * @param name                  Name of instruction to print.
 * @param offset                Offset to increment.
 */
static int nuc_printSimpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

/**
 * Prints a constant instruction.
 * @param name                  Name of instruction.
 * @param chunk                 Chunk where constant originates.
 * @param offset                Current offset.
 */
static int nuc_printConstantInstruction(const char* name, Chunk* chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    printf("%-*s \x1b[2;33m%4d\x1b[0m \x1b[2m|\x1b[0m ", PRINT_OP_PAD_LEN, name, constant);
    particle_print(chunk->constants.values[constant], true);
    printf("\n");
    return offset + 2;
}

/**
 * Prints a byte instruction.
 * @param name                  Name of instruction.
 * @param chunk                 Chunk of byte instruction.
 * @param offset                Current offset.
 */
static int nuc_printByteInstruction(const char* name, Chunk* chunk, int offset) {
    uint8_t slot = chunk->code[offset + 1];
    printf("%-*s \x1b[2;33m%4d\x1b[0m\n", PRINT_OP_PAD_LEN, name, slot);
    return offset + 2;
}

/**
 * Prints a invoker instruction.
 * @param name                  Name of instruction.
 * @param chunk                 Chunk of byte instruction.
 * @param offset                Current offset.
 */
static int nuc_printInvokeInstruction(const char* name, Chunk* chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    uint8_t argCount = chunk->code[offset + 2];
    printf("%-*s \x1b[2;33m%4d\x1b[0m \x1b[2m|\x1b[0m ", PRINT_OP_PAD_LEN, name, constant);
    particle_print(chunk->constants.values[constant], true);
    printf(" : (\x1b[33m%d\x1b[0m args)\n", argCount);
    return offset + 3;
}

/**
 * Disassembles a given chunk instruction at set offset.
 * @param chunk                     Chunk to disassemble instruction of.
 * @param offset                    Offset of instruction.
 * @param prompt                    Prompt to display before disassembled instruction
 */
int nuc_disassembleInstruction(Chunk* chunk, int offset, const char* prompt) {
    printf("%s  \x1b[2;33m%04X\x1b[0m ", prompt, offset);  // print the offset

    // want to also add some line information
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
        printf("       \x1b[2m|\x1b[0m ");
    } else {
        printf("  \x1b[33m%4d\x1b[0m \x1b[2m|\x1b[0m ", chunk->lines[offset]);
    }

/** Displays a CONSTANT Case instruction */
#define CASE_CONSTANT(op, name) \
    case op:                    \
        return nuc_printConstantInstruction(name, chunk, offset)

/** Displays a SIMPLE Case instruction */
#define CASE_SIMPLE(op, name) \
    case op:                  \
        return nuc_printSimpleInstruction(name, offset)

/** Displays a BYTE Case instruction */
#define CASE_BYTE(op, name) \
    case op:                \
        return nuc_printByteInstruction(name, chunk, offset)

/** Displays a JUMP Case instruction */
#define CASE_JUMP(op, name) \
    case op:                \
        return nuc_printJumpInstruction(name, 1, chunk, offset)

#define CASE_INVOKE(op, name) \
    case op:                  \
        return nuc_printInvokeInstruction(name, chunk, offset)

    // and now print the instruction
    uint8_t inst = chunk->code[offset];
    switch (inst) {
        /** Constant / Literal Operations */
        CASE_CONSTANT(OP_CONSTANT, "\x1b[33mOP_CONSTANT\x1b[0m");
        CASE_SIMPLE(OP_NULL, "\x1b[1mOP_NULL\x1b[0m");
        CASE_SIMPLE(OP_TRUE, "\x1b[35mOP_TRUE\x1b[0m");
        CASE_SIMPLE(OP_FALSE, "\x1b[35mOP_FALSE\x1b[0m");

        /** Math Operations */
        CASE_SIMPLE(OP_NEGATE, "\x1b[3;35mOP_NEGATE\x1b[0m");
        CASE_SIMPLE(OP_NOT, "\x1b[3;35mOP_NOT\x1b[0m");
        CASE_SIMPLE(OP_ADD, "\x1b[3;35mOP_ADD\x1b[0m");
        CASE_SIMPLE(OP_SUB, "\x1b[3;35mOP_SUB\x1b[0m");
        CASE_SIMPLE(OP_MUL, "\x1b[3;35mOP_MUL\x1b[0m");
        CASE_SIMPLE(OP_DIV, "\x1b[3;35mOP_DIV\x1b[0m");
        CASE_SIMPLE(OP_EQUAL, "\x1b[3;35mOP_EQUAL\x1b[0m");
        CASE_SIMPLE(OP_GREATER, "\x1b[3;35mOP_GREATER\x1b[0m");
        CASE_SIMPLE(OP_LESS, "\x1b[3;35mOP_LESS\x1b[0m");

        /** Atomizer Specific Operations */
        CASE_SIMPLE(OP_POP, "\x1b[34mOP_POP\x1b[0m");
        CASE_BYTE(OP_SET_IMMUTABLE, "\x1b[34mOP_SET_IMMUTABLE\x1b[0m");
        CASE_CONSTANT(OP_DEFINE_GLOBAL, "\x1b[34mOP_DEFINE_GLOBAL\x1b[0m");
        CASE_CONSTANT(OP_GET_GLOBAL, "\x1b[34mOP_GET_GLOBAL\x1b[0m");
        CASE_CONSTANT(OP_SET_GLOBAL, "\x1b[34mOP_SET_GLOBAL\x1b[0m");
        CASE_BYTE(OP_GET_LOCAL, "\x1b[34mOP_GET_LOCAL\x1b[0m");
        CASE_BYTE(OP_SET_LOCAL, "\x1b[34mOP_SET_LOCAL\x1b[0m");
        CASE_BYTE(OP_GET_UPVALUE, "\x1b[34mOP_GET_UPVALUE\x1b[0m");
        CASE_BYTE(OP_SET_UPVALUE, "\x1b[34mOP_SET_UPVALUE\x1b[0m");
        CASE_SIMPLE(OP_CLOSE_UPVALUE, "\x1b[34mOP_CLOSE_UPVALUE\x1b[0m");

        /** Model Operations */
        CASE_CONSTANT(OP_MODEL, "\x1b[33mOP_MODEL\x1b[0m");
        CASE_SIMPLE(OP_INHERIT, "\x1b[33mOP_INHERIT\x1b[33m");
        CASE_CONSTANT(OP_METHOD, "\x1b[33mOP_METHOD\x1b[0m");
        CASE_INVOKE(OP_INVOKE, "\x1b[33mOP_INVOKE\x1b[0m");
        CASE_CONSTANT(OP_FIELD, "\x1b[33mOP_FIELD\x1b[0m");
        CASE_CONSTANT(OP_GET_PROPERTY, "\x1b[33mOP_GET_PROPERTY\x1b[0m");
        CASE_CONSTANT(OP_SET_PROPERTY, "\x1b[33mOP_SET_PROPERTY\x1b[0m");
        CASE_CONSTANT(OP_SET_BASE_PROPERTY, "\x1b[33mOP_SET_BASE_PROPERTY\x1b[0m");
        CASE_CONSTANT(OP_GET_SUPER, "\x1b[33mOP_GET_SUPER\x1b[0m");
        CASE_INVOKE(OP_SUPER_INVOKE, "\x1b[33mOP_SUPER_INVOKE\x1b[0m");

        /** Library Operations */
        CASE_SIMPLE(OP_PRINT, "\x1b[3;32mOP_PRINT\x1b[0m");

        /** Directive Operations */
        CASE_BYTE(OP_MUTATE, "\x1b[35mOP_MUTATE\x1b[0m");

        /** Control Operations */
        CASE_SIMPLE(OP_RETURN, "\x1b[3;31mOP_RETURN\x1b[0m");
        CASE_JUMP(OP_JUMP, "\x1b[31mOP_JUMP\x1b[0m");
        CASE_JUMP(OP_JUMP_IF_FALSE, "\x1b[31mOP_JUMP_IF_FALSE\x1b[0m");
        CASE_JUMP(OP_JUMP_IF_FALSE_OR_POP, "\x1b[31mOP_JUMP_IF_FALSE_OR_POP\x1b[0m");
        CASE_JUMP(OP_LOOP, "\x1b[31mOP_LOOP\x1b[0m");
        CASE_BYTE(OP_CALL, "\x1b[31mOP_CALL\x1b[0m");
        case OP_CLOSURE: {  // this one requires some 'special' attention
            offset++;
            uint8_t constant = chunk->code[offset++];
            printf("%-*s \x1b[2;33m%4d\x1b[0m \x1b[2m|\x1b[0m ", PRINT_OP_PAD_LEN, "\x1b[31mOP_CLOSURE\x1b[0m", constant);
            particle_print(chunk->constants.values[constant], true);
            printf("\n");

            // and display
            ObjReaction* reac = AS_REACTION(chunk->constants.values[constant]);
            for (int i = 0; i < reac->uvCount; i++) {
                int isLocal = chunk->code[offset++];
                int index = chunk->code[offset++];
                printf("%s  \x1b[2;33m%04X\x1b[0m ", prompt, offset - 2);  // print the offset
                printf("       \x1b[2m|\x1b[0m \x1b[3;34m%-*s\x1b[0m \x1b[2m|\x1b[0m \x1b[33m%d\x1b[0m\n", PRINT_OP_PAD_LEN - 4, isLocal ? "local" : "upvalue", index);
            }
            return offset;
        }

        default:  // bad operation codes
            printf("\x1b[1;31mUnknown opcode\x1b[0m \x1b[33m0x%.2X\x1b[0m\n", inst);
            return offset + 1;
    }

        // undefine some local macros
#undef CASE_CONSTANT
#undef CASE_SIMPLE
#undef CASE_BYTE
}

/**
 * Disassembles a given chunk with display name.
 * @param chunk                     Chunk to disassemble.
 * @param name                      Display name.
 */
void nuc_disassembleChunk(Chunk* chunk, const char* name) {
    printf("\n[\x1b[2;35mchunk\x1b[0m]  \x1b[35m\"%s\"\x1b[0m\n", name);  // print the display name

    // and begin disassembling instructions
    for (int offset = 0; offset < chunk->count;) offset = nuc_disassembleInstruction(chunk, offset, "[\x1b[2;35mchunk\x1b[0m]");
    printf("\n");  // and pad display
}

#endif