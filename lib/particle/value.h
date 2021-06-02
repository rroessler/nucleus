#ifndef NUC_PARTICLE_H
#define NUC_PARTICLE_H

// Nucleus Headers
#include "../common.h"
#include "../memory.h"

// forward declaration of object type
typedef struct Obj Obj;
typedef struct ObjString ObjString;

/** Particle Moment - Type of Nucleus Value */
typedef enum {
    TYPE_BOOL,
    TYPE_NULL,
    TYPE_NUMBER,
    TYPE_OBJ,
} Moment;

/** Particle - Generic Nucleus Value */
typedef struct {
    Moment type;
    union {
        bool boolean;
        double number;
        Obj* obj;
    } as;
} Particle;

/** Array of Nucleus Values */
typedef struct {
    int capacity;
    int count;
    Particle* values;
} ParticleArray;

#define NUC_BOOL(value) ((Particle){TYPE_BOOL, {.boolean = value}})
#define NUC_NULL ((Particle){TYPE_NULL, {.number = 0}})
#define NUC_NUMBER(value) ((Particle){TYPE_NUMBER, {.number = value}})
#define NUC_OBJ(value) ((Particle){TYPE_OBJ, {.obj = (Obj*)value}})

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define AS_OBJ(value) ((value).as.obj)

#define IS_BOOL(value) ((value).type == TYPE_BOOL)
#define IS_NULL(value) ((value).type == TYPE_NULL)
#define IS_NUMBER(value) ((value).type == TYPE_NUMBER)
#define IS_OBJ(value) ((value).type == TYPE_OBJ)

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