#ifndef NUC_BYTECODE_EMIT_H
#define NUC_BYTECODE_EMIT_H

// Nucleus Headers
#include "../bytecode/chunk.h"
#include "../bytecode/ops.h"
#include "core/core.h"
#include "global.h"
#include "parser/parser.h"

/****************
 *  EMITER API  *
 ****************/

/**
 * Make a new constant value for the current chunk.
 * @param value             Particle value for constant.
 */
static uint16_t chunk_makeConstant(nuc_Particle value) {
    int constant = chunk_addConstant(fuser_currentChunk(), value);
    if (constant >= UINT16_MAX) {
        PARSER_ERROR_AT("Chunk exceeds maximum allowable constants.");
        return 0;
    }

    // return the constant as a short
    return (uint16_t)constant;
}

/**
 * Writes a singular byte to the current bytecode chunk.
 * @param byte                      Byte to write.
 */
static inline void chunk_emitByte(uint8_t byte) {
    chunk_write(fuser_currentChunk(), byte, parser.previous.line);
}

/**
 * Macro to emit one byte to the current chunk.
 * @param b             Byte to emit.
 */
#define EMIT_BYTE(b) chunk_emitByte(b)

/**
 * Macro to emit TWO bytes in succession
 * @param b1            Byte one.
 * @param b2            Byte two.
 */
#define EMIT_SHORT(b1, b2) \
    chunk_emitByte(b1);    \
    chunk_emitByte(b2)

/**
 * Macro to emit a UINT16 value.
 * @param u16           `uint16_t` to emit.
 */
#define EMIT_UINT16(u16)                          \
    chunk_emitByte((uint8_t)((u16 >> 8) & 0xFF)); \
    chunk_emitByte((uint8_t)(u16 & 0xFF))

/** Macro to emit FOUR Bytes in succession */
#define EMIT_ADDR(b1, b2, b3, b4) \
    EMIT_SHORT(b1, b2);           \
    EMIT_SHORT(b3, b4)

/** Emits the Return Operation */
static void chunk_emitReturn() {
    if (current->type == RT_INITIALISER) {
        EMIT_BYTE(OP_GET_LOCAL);
        EMIT_SHORT(0x00, 0x00);
    } else {
        EMIT_BYTE(OP_NULL);
    }
    EMIT_BYTE(OP_RETURN);
}

/** 
 * Emits a constant as a operation value. 
 * @param value         Emits a particle as a constant.
 */
static inline void chunk_emitConstant(nuc_Particle value) {
    EMIT_BYTE(OP_CONSTANT);
    uint16_t constant = chunk_makeConstant(value);
    EMIT_UINT16(constant);
}

/**
 * Emits a jump instruction with two bytes saved for later (to fill)
 * @param inst              Instruction to jump with.
 */
static inline int chunk_emitJump(uint8_t inst) {
    chunk_emitByte(inst);
    EMIT_ADDR(0xFF, 0xFF, 0xFF, 0xFF);
    return fuser_currentChunk()->count - 4;
}

/**
 * Patches a set of bytes to be a valid jump.
 * @param offset                Offset for jump to be stored.
 */
static void chunk_patchJump(int offset) {
    // grab the jump bytes to override
    int jump = fuser_currentChunk()->count - offset - 4;

    // make sure the jump is valid within the chunk
    if (jump > UINT32_MAX) PARSER_ERROR_AT("Cannot jump to a location larger than 32-bit.");

    // and ADD the jump back into the code chunk
    fuser_currentChunk()->code[offset] = (jump >> 24) & 0xFF;
    fuser_currentChunk()->code[offset + 1] = (jump >> 16) & 0xFF;
    fuser_currentChunk()->code[offset + 2] = (jump >> 8) & 0xFF;
    fuser_currentChunk()->code[offset + 3] = jump & 0xFF;
}

/**
 * Emits the loop operation with a given start of loop.
 * @param loopStart             IP for start of loop.
 */
static void chunk_emitLoop(int loopStart) {
    EMIT_BYTE(OP_LOOP);  // emit the loop
    int offset = fuser_currentChunk()->count - loopStart + 4;
    if (offset > UINT32_MAX) PARSER_ERROR_AT("Loop body is too large. Exceeds 32-bit range.");
    EMIT_BYTE((offset >> 24) & 0xFF);
    EMIT_BYTE((offset >> 16) & 0xFF);
    EMIT_BYTE((offset >> 8) & 0xFF);
    EMIT_BYTE(offset & 0xFF);
}

/*******************
 *  HELPER MACROS  *
 *******************/

#define EMIT_RET chunk_emitReturn()
#define EMIT_CONST(val) chunk_emitConstant(val)
#define EMIT_JUMP(inst) chunk_emitJump(inst)
#define PATCH_JUMP(offset) chunk_patchJump(offset)
#define EMIT_LOOP(offset) chunk_emitLoop(offset)

/** Simple macro to help clean up switch case below. */
#define CASE_EMIT(token, opcode) \
    case token:                  \
        chunk_emitByte(opcode);  \
        break

/** Simple macro to help clean up emit case statements. */
#define CASE_EMIT_SHORT(token, op1, op2) \
    case token:                          \
        EMIT_SHORT(op1, op2);            \
        break

#endif