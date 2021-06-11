#ifndef NUC_OBJ_ARRAY_H
#define NUC_OBJ_ARRAY_H

// Nucleus Headers
#include "../../common.h"

// FORWARD DECLARATION
static void atomizer_catchableError(uint8_t code, const char* format, ...);

/** Array Type Enumeration */
typedef enum {
    ARR_BASIC,
    ARR_SET,
    ARR_STACK,
    ARR_QUEUE,
} nuc_ArrType;

/** Array Type Definition */
typedef struct {
    nuc_Obj obj;
    nuc_ArrType type;
    size_t count;
    size_t capacity;
    nuc_Particle* values;
} nuc_ObjArr;

/****************************
 *  INTERNAL ARRAY METHODS  *
 ****************************/

/**
 * Initialises a new array with given type.
 * @param type              Type of array.
 */
nuc_ObjArr* objArr_new(nuc_ArrType type) {
    nuc_ObjArr* arr = NUC_ALLOC_OBJ(nuc_ObjArr, OBJ_ARRAY);
    arr->count = 0;
    arr->capacity = 0;
    arr->values = NULL;
    arr->type = type;
    return arr;
}

/**
 * Pushes an item onto the end of a dynamic array.
 * @param arr               Array to push item onto.
 * @param value             Value to push.
 */
void objArr_push(nuc_ObjArr* arr, nuc_Particle value) {
    NUC_GROW_ARR_IF(nuc_Particle, arr, values, GROW_DYNAMIC);  // grow the array if needed
    arr->values[arr->count] = value;                           // save the new particle
    arr->count++;
}

/**
 * Decrements the count of an array so that on the next push, an item is fine.
 * @param arr               Array to POP a value from.
 */
nuc_Particle objArr_pop(nuc_ObjArr* arr) {
    // if empty then want to denote an error
    if (arr->count == 0) {
        atomizer_catchableError(NUC_EXIT_RANGE, "Cannot pop an array that is empty.");
        return NUC_NULL;
    }

    nuc_Particle value = arr->values[--arr->count];  // grab the POPPED item
    arr->values[arr->count] = NUC_NULL;              // and cap with null
    return value;
}

#endif