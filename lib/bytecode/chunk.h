#ifndef NUC_CHUNK_H
#define NUC_CHUNK_H

// C Standard Library
#include <stdlib.h>

// Nucleus Headers
#include "../common.h"
#include "../particle/value.h"
#include "../utils/memory.h"

// forward declaration
static inline void atomizer_push(nuc_Particle value);
static inline nuc_Particle atomizer_pop();

/*********************
 *  CHUNK CONSTANTS  *
 *********************/

/** Nucleus Bytecode Chunks */
typedef struct {
    int count;                  // array based
    int capacity;               // items
    uint8_t* code;              // bytecode
    long* lines;                // lines connected to bytecode
    nuc_ParticleArr constants;  // chunk constants
} nuc_Chunk;

/*******************
 *  CHUNK METHODS  *
 *******************/

/**
 * Initialises a bytecode chunk.
 * @param chunk                 Chunk to initialise.
 */
void chunk_init(nuc_Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    particleArr_init(&chunk->constants);
}

/**
 * Frees a chunk from memory.
 * @param chunk                 Chunk to free from memory.
 */
void chunk_free(nuc_Chunk* chunk) {
    NUC_FREE_ARR(uint8_t, chunk->code, chunk->capacity);
    NUC_FREE_ARR(long, chunk->lines, chunk->capacity);
    particleArr_free(&chunk->constants);
    chunk_init(chunk);  // and re-initialise to default
}

/**
 * Writes a byte to a chunk. If there is no space, the chunk size is increased.
 * @param chunk                 Chunk to write to.
 * @param byte                  Byte to write.
 */
void chunk_write(nuc_Chunk* chunk, uint8_t byte, long line) {
    NUC_GROW_ARR_IF_MULT(uint8_t, long, chunk, code, lines, GROW_FAST);  // grow if space is needed
    chunk->code[chunk->count] = byte;                                    // write byte
    chunk->lines[chunk->count] = line;                                   // and the associate line
    chunk->count++;
}

/**
 * Adds a constant value to a chunk.
 * @param chunk                 Chunk to add constant to.
 * @param value                 Constant to add.
 */
int chunk_addConstant(nuc_Chunk* chunk, nuc_Particle value) {
    atomizer_push(value);
    particleArr_write(&chunk->constants, value);
    atomizer_pop();
    return chunk->constants.count - 1;  // and return location
}

#endif