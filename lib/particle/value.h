#ifndef NUC_PARTICLE_H
#define NUC_PARTICLE_H

// C Standard Library
#include <string.h>

// Nucleus Headers
#include "../common.h"
#include "../memory.h"

// forward declaration of object type
typedef struct Obj Obj;
typedef struct ObjString ObjString;

#ifdef NUC_NAN_BOXING

// particle type definition
typedef uint64_t Particle;
    #define QNAN ((uint64_t)0x7ffc000000000000)      // queries if Not-a-Number
    #define SIGN_BIT ((uint64_t)0x8000000000000000)  // 64-bit sign

    // type tags
    #define TAG_NULL 1
    #define TAG_FALSE 2
    #define TAG_TRUE 3

    // creation
    #define NUC_NUMBER(num) numToParticle(num)
    #define NUC_NULL ((Particle)(uint64_t)(QNAN | TAG_NULL))
    #define NUC_FALSE ((Particle)(uint64_t)(QNAN | TAG_FALSE))
    #define NUC_TRUE ((Particle)(uint64_t)(QNAN | TAG_TRUE))
    #define NUC_BOOL(b) ((b) ? NUC_TRUE : NUC_FALSE)
    #define NUC_OBJ(obj) \
        (Particle)(SIGN_BIT | QNAN | (uint64_t)(uintptr_t)(obj))

    // casts
    #define AS_NUMBER(value) particleToNum(value)
    #define AS_BOOL(value) ((value) == NUC_TRUE)
    #define AS_OBJ(value) \
        ((Obj*)(uintptr_t)((value) & ~(SIGN_BIT | QNAN)))

    // checks
    #define IS_NUMBER(value) (((value)&QNAN) != QNAN)
    #define IS_NULL(value) ((value) == NUC_NULL)
    #define IS_BOOL(value) (((value) | 1) == NUC_TRUE)
    #define IS_OBJ(value) \
        (((value) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))

    #define IS_MUTABLE(value) \
        (IS_NUMBER(value) || IS_NULL(value) || IS_BOOL(value) || AS_OBJ(value)->mutable)

/**
 * Converts a double to a particle numeric.
 * @param num           Double to convert.
 */
static inline Particle numToParticle(double num) {
    Particle value;
    memcpy(&value, &num, sizeof(double));
    return value;
}

/**
 * Converts a particle to a double.
 * @param value             Particle to convert.
 */
static inline double particleToNum(Particle value) {
    double num;
    memcpy(&num, &value, sizeof(Particle));
    return num;
}

#else

/** Particle Moment - Type of Nucleus Value */
typedef enum {
    TYPE_NULL,
    TYPE_BOOL,
    TYPE_NUMBER,
    TYPE_OBJ,
} Moment;

/** Particle - Generic Nucleus Value */
typedef struct {
    Moment type;
    bool mutable;
    union {
        bool boolean;
        double number;
        Obj* obj;
    } as;
} Particle;

    // creation
    #define NUC_BOOL(value) ((Particle){TYPE_BOOL, true, {.boolean = value}})
    #define NUC_NULL ((Particle){TYPE_NULL, true, {.number = 0}})
    #define NUC_NUMBER(value) ((Particle){TYPE_NUMBER, true, {.number = value}})
    #define NUC_OBJ(value) ((Particle){TYPE_OBJ, false, {.obj = (Obj*)value}})
    #define NUC_OBJ_MUTABLE(value) ((Particle){TYPE_OBJ, true, {.obj = (Obj*)value}})

    // cast as
    #define AS_BOOL(value) ((value).as.boolean)
    #define AS_NUMBER(value) ((value).as.number)
    #define AS_OBJ(value) ((value).as.obj)

    // checks
    #define IS_BOOL(value) ((value).type == TYPE_BOOL)
    #define IS_NULL(value) ((value).type == TYPE_NULL)
    #define IS_NUMBER(value) ((value).type == TYPE_NUMBER)
    #define IS_OBJ(value) ((value).type == TYPE_OBJ)
    #define IS_MUTABLE(value) ((value).mutable)

#endif

/** Array of Nucleus Values */
typedef struct {
    int capacity;
    int count;
    Particle* values;
} ParticleArray;

/**
 * Initialises a particle array.
 * @param arr                   Particle array to initialise.
 */
void particleArray_init(ParticleArray* arr) {
    arr->values = NULL;
    arr->capacity = 0;
    arr->count = 0;
}

/**
 * Writes a particle to a particle array.
 * @param arr                   Particle array to write to.
 * @param value                 Value to write.
 */
void particleArray_write(ParticleArray* arr, Particle value) {
    // grow the array if needed
    GROW_ARRAY_IF(Particle, arr, values);

    // and append to the array
    arr->values[arr->count] = value;
    arr->count++;
}

/**
 * Frees a particle array from memory.
 * @param arr                   Particle array to free.
 */
void particleArray_free(ParticleArray* arr) {
    FREE_ARRAY(Particle, arr->values, arr->capacity);
    particleArray_init(arr);
}

#endif