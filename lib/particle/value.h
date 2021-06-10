#ifndef NUC_PARTICLE_H
#define NUC_PARTICLE_H

// C Standard Library
#include <string.h>

// Nucleus Headers
#include "../common.h"
#include "../utils/memory.h"

// forward declare object types
typedef struct nuc_Obj nuc_Obj;

/****************
 *  NAN BOXING  *
 ****************/

// particle type definition (as a memory location)
typedef uint64_t nuc_Particle;

#define QNAN ((uint64_t)0x7ffc000000000000)
#define SIGN_BIT ((uint64_t)0x8000000000000000)

// literal tags
#define TAG_NULL 1
#define TAG_FALSE 2
#define TAG_TRUE 3

// immutability booleans
#define NUC_MUTABLE false
#define NUC_IMMUTABLE true

/**
 * Converts a double to a particle numeric.
 * @param num           Double to convert.
 */
static inline nuc_Particle numToParticle(double num) {
    nuc_Particle value;
    memcpy(&value, &num, sizeof(double));
    return value;
}

/**
 * Converts a particle to a double.
 * @param value             Particle to convert.
 */
static inline double particleToNum(nuc_Particle value) {
    double num;
    memcpy(&num, &value, sizeof(nuc_Particle));
    return num;
}

/*********************
 *  PARTICLE MACROS  *
 *********************/

// creation
#define NUC_NUM(num) numToParticle(num)
#define NUC_NULL ((nuc_Particle)(uint64_t)(QNAN | TAG_NULL))
#define NUC_FALSE ((nuc_Particle)(uint64_t)(QNAN | TAG_FALSE))
#define NUC_TRUE ((nuc_Particle)(uint64_t)(QNAN | TAG_TRUE))
#define NUC_BOOL(b) ((b) ? NUC_TRUE : NUC_FALSE)
#define NUC_OBJ(obj) \
    (nuc_Particle)(SIGN_BIT | QNAN | (uint64_t)(uintptr_t)(obj))

// casts
#define AS_NUMBER(value) particleToNum(value)
#define AS_BOOL(value) ((value) == NUC_TRUE)
#define AS_OBJ(value) \
    ((nuc_Obj*)(uintptr_t)((value) & ~(SIGN_BIT | QNAN)))

// checking
#define IS_NUMBER(value) (((value)&QNAN) != QNAN)
#define IS_NULL(value) ((value) == NUC_NULL)
#define IS_BOOL(value) (((value) | 1) == NUC_TRUE)
#define IS_OBJ(value) \
    (((value) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))

/***************************
 *  INTERNAL PARTICLE ARR  *
 ***************************/

/** Internal Array of Nucleus Values */
typedef struct {
    int count;
    int capacity;
    nuc_Particle* values;
} nuc_ParticleArr;

/**
 * Initialises a particle array.
 * @param arr                   Particle array to initialise.
 */
void particleArr_init(nuc_ParticleArr* arr) {
    arr->values = NULL;
    arr->capacity = 0;
    arr->count = 0;
}

/**
 * Writes a particle to a particle array.
 * @param arr                   Particle array to write to.
 * @param value                 Value to write.
 */
void particleArr_write(nuc_ParticleArr* arr, nuc_Particle value) {
    NUC_GROW_ARR_IF(nuc_Particle, arr, values, GROW_DYNAMIC);  // grow the array if needed
    arr->values[arr->count] = value;                           // and save the new value
    arr->count++;                                              // increment array count
}

/**
 * Frees a particle array from memory.
 * @param arr                   Particle array to free.
 */
void particleArr_free(nuc_ParticleArr* arr) {
    NUC_FREE_ARR(nuc_Particle, arr->values, arr->capacity);  // free the array
    particleArr_init(arr);                                   // and re-initialise
}

#endif