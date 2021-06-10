#ifndef NUC_UTIL_STRINGS_H
#define NUC_UTIL_STRINGS_H

// C Standard Library
#include <ctype.h>
#include <string.h>

// Nucleus Headers
#include "../common.h"

/**
 * Compares two string constants that A is less than B.
 * @param a                 String A.
 * @param b                 String B.
 */
static inline bool strings_isLess(const char* a, const char* b) {
    return strcmp(a, b) < 0;
}

/**
 * Returns whether a character is a valid alpha character.
 * @param c                 Character to check.
 */
static inline bool strings_isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

/**
 * Determines if a character is a digit character 0->9.
 * @param c                 Character to check.
 */
static inline bool strings_isDigit(char c) { return c >= '0' && c <= '9'; }

/**
 * Determines if a character is a hex character 0->F case insensitive.
 * @param c                 Character to check.
 */
static inline bool strings_isHex(char c) { return strings_isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }

#endif