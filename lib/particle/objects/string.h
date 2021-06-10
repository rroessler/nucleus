#ifndef NUC_OBJ_STRING_H
#define NUC_OBJ_STRING_H

// C Standard Library
#include <string.h>

// Nucleus Headers
#include "../../utils/hash.h"
#include "../../utils/memory.h"
#include "../../vm/atomizer.h"
#include "../../vm/disruptions/immediate.h"

/***************************
 *  OBJECT STRING METHODS  *
 ***************************/

// forward declaration of allocation item
nuc_Obj* obj_alloc(size_t size, nuc_ObjType type);
static void atomizer_setIntern(nuc_ObjString* string);
static inline nuc_ObjString* atomizer_getIntern(const char* chars, int length, uint32_t hash);

/**
 * Allocates a string object into memory.
 * @param chars                 String for string object to own.
 * @param length                Length of string input.
 * @param hash                  String hash.
 */
static nuc_ObjString* objString_alloc(char* chars, int length, uint32_t hash) {
    nuc_ObjString* string = NUC_ALLOC_OBJ(nuc_ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    atomizer_setIntern(string);
    return string;
}

/**
 * Unescapes a given input string.
 * @param chars             String to unescape.
 * @param length            Length of input string.
 */
static char* objString_unescape(const char* chars, int* length) {
    // initially check if the '\' character even exists
    if (strchr(chars, '\\') == NULL) return (char*)chars;

// switch case helper
#define CASE_UNESCAPE(c, hex)       \
    case c:                         \
        strncat(unescaped, hex, 1); \
        newLen++;                   \
        break;

    char* unescaped = NUC_ALLOC(char, *length + 1);  // preempt a size and alloc
    int newLen = 0;                                  // and set a new length
    for (int i = 0; i < *length; i++) {
        if (chars[i] != '\\') {                // if not '\'
            strncat(unescaped, &chars[i], 1);  // then concat
            newLen++;                          // then increment the new length
            continue;                          // and continue
        }

        // otherwise handle string escape sequences
        switch (chars[++i]) {
            CASE_UNESCAPE('a', "\a");
            CASE_UNESCAPE('b', "\b");
            CASE_UNESCAPE('f', "\f");
            CASE_UNESCAPE('n', "\n");
            CASE_UNESCAPE('r', "\r");
            CASE_UNESCAPE('t', "\t");
            CASE_UNESCAPE('v', "\v");
            CASE_UNESCAPE('\\', "\\");
            CASE_UNESCAPE('\'', "'");
            CASE_UNESCAPE('"', "\"");
            CASE_UNESCAPE('?', "\?");  // single char escape sequences

            case 'x': {  // byte escape sequence
                char hex[2];
                hex[0] = chars[++i];
                hex[1] = chars[++i];
                long byte = strtol(hex, NULL, 16);
                strncat(unescaped, (char*)&byte, 1);
                newLen++;  // and increment the length by 1                           // waste a char as expect two
            } break;

            default:  // want to error out on other escapes
                nuc_immediateExit(NUC_EXIT_SYNTAX, "Found an invalid escape sequence character.");
                /** TODO: ATOMIZER_ERROR */
                return (char*)"";
        }
    }

    // if now same length, then can ignore change
    if (newLen == *length) return (char*)chars;

    // otherwise need to modify the length
    length = &newLen;
    unescaped[newLen] = '\0';                           // terminate
    unescaped = (char*)realloc(unescaped, newLen + 1);  // and realloc size
    return unescaped;

// undefining the switch case
#undef CASE_UNSESCAPE
}

/**
 * Creates a string object from given series of characters.
 * @param input             Chars to convert to string object.
 * @param inLen             Length of input string.
 */
nuc_ObjString* objString_copy(const char* input, int inLen) {
    // want to initially UNESCAPE an input string
    int length = inLen;
    const char* chars = objString_unescape(input, &length);
    uint32_t hash = hash_generic(chars, length);

    // if a string intern is found, return it instead
    nuc_ObjString* interned = atomizer_getIntern(chars, length, hash);
    if (interned != NULL) return interned;

    char* heapChars = NUC_ALLOC(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return objString_alloc(heapChars, length, hash);
}

/**
 * Claims ownership of a string to a string object.
 * @param chars             Chars to claim ownership of.
 * @param length            Length of string.
 */
nuc_ObjString* objString_take(char* chars, int length) {
    uint32_t hash = hash_generic(chars, length);

    // if a string intern is found, claim ownership
    nuc_ObjString* interned = atomizer_getIntern(chars, length, hash);
    if (interned != NULL) {
        NUC_FREE_ARR(char, chars, length + 1);
        return interned;
    }

    // and return the allocation
    return objString_alloc(chars, length, hash);
}

#endif