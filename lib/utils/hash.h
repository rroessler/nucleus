#ifndef NUC_UTIL_HASH_H
#define NUC_UTIL_HASH_H

// Nucleus Headers
#include "../common.h"

/**
 * Coordinates a string hashing algorithm to get a 32-bit unsigned hash.
 * @param key           String to hash.
 * @param length        Length of string.
 */
static uint32_t hash_generic(const char* key, int length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

#endif