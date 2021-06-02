#ifndef NUC_UTIL_STRINGS_H
#define NUC_UTIL_STRINGS_H

// C Standard Library
#include <ctype.h>
#include <string.h>

/**
 * Appends a character to a given character array.
 * @param arr                   Character array to append character to.
 * @param c                     Character to append.
 */
void strings_appendChar(char* arr, char c) {
    strncat(arr, &c, 1);
}

/**
 * Returns whether a character is a valid alpha character.
 * @param c                 Character to check.
 */
static bool strings_isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

/**
 * Determines if a character is a digit character 0->9.
 * @param c                 Character to check.
 */
static bool strings_isDigit(char c) { return c >= '0' && c <= '9'; }

/**
 * Determines if a character is a hex character 0->F case insensitive.
 * @param c                 Character to check.
 */
static bool strings_isHex(char c) { return strings_isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }

/**
 * Coordinates a string hashing algorithm to get a 32-bit unsigned hash.
 * @param key           String to hash.
 * @param length        Length of string.
 */
static uint32_t strings_hash(const char* key, int length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

#endif