#ifndef NUC_UTIL_MEMORY_H
#define NUC_UTIL_MEMORY_H

// Nucleus Headers
#include "../common.h"
#include "../vm/garbage/realloc.h"

/********************
 *  MEMORY HELPERS  *
 ********************/

/**
 * Frees a Nucleus pointer from memory.
 * @param type              Type of pointer.
 * @param ptr               Pointer to free.
 */
#define NUC_FREE(type, ptr) (type*)nuc_realloc(ptr, sizeof(type), 0)

/**
 * Frees a Nucleus array from memory.
 * @param type              Type of array pointer.
 * @param ptr               Array pointer to free.
 * @param prev              Previous size.
 */
#define NUC_FREE_ARR(type, ptr, prev) (type*)nuc_realloc(ptr, sizeof(type) * (prev), 0)

/**
 * Allocates memory for a Nucleus item.
 * @param type              Type of item.
 * @param count             Size count.
 */
#define NUC_ALLOC(type, count) (type*)nuc_realloc(NULL, 0, sizeof(type) * (count))

/**
 * Grows the capacity of an array to a default of 8, then multiples of 2. This uses
 * the QT based memory grow scheme. It increments in memory from 0 -> 20 by additions
 * of 4, then increases by (cap * 2) + 12 until 4084. After then, increments in 2048
 * blocks at a time.
 * 
 * Supposedly by NOT incrementing directly with powers of 2, this will act better than
 * direct powers of 2. Additionally, there is no need to keep continuing with this as 
 * most modern operating systems DO NOT copy the entire data when reallocating.
 * @param cap               Previous capacity.
 */
#define NUC_CAP_GROW_DYNAMIC(cap) ((cap) < 20           \
                                       ? ((cap) + 4)    \
                                   : (cap) < 4084       \
                                       ? ((cap)*2 + 12) \
                                       : ((cap) + 2048))

/**
 * Conversely a fast growth algorithm can also be utilised for KNOWN items that
 * will initially grow fast, but slow down afterwards.
 * @param cap               Previous capacity.
 */
#define NUC_CAP_GROW_FAST(cap) ((cap) < 8 ? 8 : (cap)*2)

/**
 * Grows an array pointer.
 * @param type              Type of array.
 * @param ptr               Pointer to array.
 * @param prev              Previous size.
 * @param now               New size.
 */
#define NUC_GROW_ARR(type, ptr, prev, now) (type*)nuc_realloc(ptr, sizeof(type) * (prev), sizeof(type) * (now))

/**
 * Grows an array IFF its new count will exceed its capacity.
 * @param type              Type of array.
 * @param arr               Array to grow.
 * @param accessor          Accesor to array.
 */
#define NUC_GROW_ARR_IF(type, arr, accessor, METHOD)                            \
    if (arr->capacity < arr->count + 1) {                                       \
        int prev = arr->capacity;                                               \
        arr->capacity = NUC_CAP_##METHOD(prev);                                 \
        arr->accessor = NUC_GROW_ARR(type, arr->accessor, prev, arr->capacity); \
    }

/**
 * Grows two arrays IFF the capacity is exceeded by the count.
 * @param typeA             A array type.
 * @param typeB             B array type.
 * @param arr               Array to grow.
 * @param a                 Accessor A.
 * @param b                 Accessor B.
 */
#define NUC_GROW_ARR_IF_MULT(typeA, typeB, arr, a, b, METHOD)      \
    if (arr->capacity < arr->count + 1) {                          \
        int prev = arr->capacity;                                  \
        arr->capacity = NUC_CAP_##METHOD(prev);                    \
        arr->a = NUC_GROW_ARR(typeA, arr->a, prev, arr->capacity); \
        arr->b = NUC_GROW_ARR(typeB, arr->b, prev, arr->capacity); \
    }

#endif