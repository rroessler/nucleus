#ifndef NUC_FUSER_EMIT_H
#define NUC_FUSER_EMIT_H

// Nucleus Headers
#include "../chunk/chunk.h"
#include "../chunk/operation.h"
#include "parser.h"

/****************
 *  EMITER API  *
 ****************/

// forward declaraion
static Chunk* currentChunk();
static void emitReturn();

/**
 * Make a new constant value for the current chunk.
 * @param value             Particle value for constant.
 */
static uint8_t makeConstant(Particle value) {
    int constant = chunk_addConstant(currentChunk(), value);
    if (constant >= UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }

    // return the constant as a byte
    return (uint8_t)constant;
}

/**
 * Writes a singular byte to the current bytecode chunk.
 * @param byte                      Byte to write.
 */
static void emitByte(uint8_t byte) { chunk_write(currentChunk(), byte, parser.previous.line); }

/**
 * Emits TWO bytes in succession.
 * @param b1                Byte one.
 * @param b2                Byte two.
 */
#define EMIT_SHORT(b1, b2) \
    emitByte(b1);          \
    emitByte(b2)

/** Emits a constant as a operation value. */
static void emitConstant(Particle value) { EMIT_SHORT(OP_CONSTANT, makeConstant(value)); }

/**
 * Emits a jump instruction with two bytes saved for later (to fill)
 * @param inst              Instruction to jump with.
 */
static int emitJump(uint8_t inst) {
    emitByte(inst);
    EMIT_SHORT(0xFF, 0xFF);
    return currentChunk()->count - 2;
}

/**
 * Patches a set of bytes to be a valid jump.
 * @param offset                Offset for jump to be stored.
 */
static void patchJump(int offset) {
    // grab the jump bytes to override
    int jump = currentChunk()->count - offset - 2;

    // make sure the jump is valid within the chunk
    if (jump > UINT16_MAX) error("Cannot jump to a location larger than 16-bit.");

    // and ADD the jump back into the code chunk
    currentChunk()->code[offset] = (jump >> 8) & 0xFF;
    currentChunk()->code[offset + 1] = jump & 0xFF;
}

/**
 * Emits the loop operation with a given start of loop.
 * @param loopStart             IP for start of loop.
 */
static void emitLoop(int loopStart) {
    emitByte(OP_LOOP);

    int offset = currentChunk()->count - loopStart + 2;
    if (offset > UINT16_MAX) error("Loop body is too large. Exceeds 16-bit range.");

    emitByte((offset >> 8) & 0xFF);
    emitByte(offset & 0xFF);
}

/** Simple macro to help clean up switch case below. */
#define CASE_EMIT(token, opcode) \
    case token:                  \
        emitByte(opcode);        \
        break

/** Simple macro to help clean up emit case statements. */
#define CASE_EMIT_SHORT(token, op1, op2) \
    case token:                          \
        EMIT_SHORT(op1, op2);            \
        break

#endif