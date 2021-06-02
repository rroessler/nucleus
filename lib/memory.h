#ifndef NUC_MEMORY_H
#define NUC_MEMORY_H

// C Standard Library
#include <stdlib.h>

// Nucleus Headers
#include "common.h"

// forward declaration
void gc_collect();
static void atomizer_gc();

/** Frees a pointer from memory. */
#define FREE(type, ptr) (type*)nuc_realloc(ptr, sizeof(type), 0)

/** Allocates a size to memory. */
#define ALLOCATE(type, count) (type*)nuc_realloc(NULL, 0, sizeof(type) * (count))

/** Grows a given capacity value. */
#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity * 2))

/** Grows an array pointer to a new size. */
#define GROW_ARRAY(type, ptr, old_, new_) (type*)nuc_realloc(ptr, sizeof(type) * (old_), sizeof(type) * (new_))

/** Grows an array only if is required. */
#define GROW_ARRAY_IF(type, arr, accessor)                                   \
    if (arr->capacity < arr->count + 1) {                                    \
        int old = arr->capacity;                                             \
        arr->capacity = GROW_CAPACITY(old);                                  \
        arr->accessor = GROW_ARRAY(type, arr->accessor, old, arr->capacity); \
    }

/** Grows an array if needed for TWO accessors. */
#define GROW_ARRAY_IF_MULT(typeA, typeB, arr, a, b)             \
    if (arr->capacity < arr->count + 1) {                       \
        int old = arr->capacity;                                \
        arr->capacity = GROW_CAPACITY(old);                     \
        arr->a = GROW_ARRAY(typeA, arr->a, old, arr->capacity); \
        arr->b = GROW_ARRAY(typeB, arr->b, old, arr->capacity); \
    }

/** Frees an array pointer by reallocating with a size of zero. */
#define FREE_ARRAY(type, ptr, old) (type*)nuc_realloc(ptr, sizeof(type) * (old), 0)

/**
 * Reallocates a pointer with a new size.
 * @param ptr                   Pointer to reallocate.
 * @param old_                  Old size of pointer.
 * @param new_                  New size of pointer.
 */
void* nuc_realloc(void* ptr, size_t old_, size_t new_) {
    atomizer_gc(old_, new_);

    // if the new size if zero, then want to free the pointer
    if (new_ == 0) {
        free(ptr);
        return NULL;
    }

    // and reallocate as necessary
    void* res = realloc(ptr, new_);
    if (res == NULL) exit(1); /** TODO: Implement Error Handler */
    return res;
}

#endif