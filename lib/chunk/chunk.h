#ifndef NUC_CHUNK_H
#define NUC_CHUNK_H

// C Standard Library
#include <stdlib.h>

// Nucleus Headers
#include "../common.h"
#include "../memory.h"
#include "../particle/value.h"

// forward declaration
static void atomizer_push(Particle value);
static Particle atomizer_pop();

/** Nucleus Code Chunks */
typedef struct {
    int count;
    int capacity;
    uint8_t* code;
    int* lines;
    ParticleArray constants;
} Chunk;

/**
 * Initialises a bytecode chunk.
 * @param chunk                 Chunk to initialise.
 */
void chunk_init(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    particleArray_init(&chunk->constants);
}

/**
 * Writes a byte to a bytecode chunk. If there is no space, the chunk size is increased.
 * @param chunk                 Chunk to write to.
 * @param byte                  Byteto write.
 */
void chunk_write(Chunk* chunk, uint8_t byte, int line) {
    // resize the chunk if needed
    GROW_ARRAY_IF_MULT(uint8_t, int, chunk, code, lines);

    // and write the byte
    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

/**
 * Frees a chunk from memory.
 * @param chunk                 Chunk to free from memory.
 */
void chunk_free(Chunk* chunk) {
    // free the chunk
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->capacity);
    particleArray_free(&chunk->constants);
    chunk_init(chunk);  // and re-initialise to default
}

/**
 * Adds a constant value to a chunk.
 * @param chunk                 Chunk to add constant to.
 * @param value                 Particle value to add to chunk.
 */
int chunk_addConstant(Chunk* chunk, Particle value) {
    atomizer_push(value);
    particleArray_write(&chunk->constants, value);
    atomizer_pop();
    return chunk->constants.count - 1;  // and return the index of the constant
}

#endif