#ifndef NUC_STRING_INTERNS_H
#define NUC_STRING_INTERNS_H

// Nucleus Headers
#include "../particle/object/string.h"
#include "../particle/table.h"

// forward declaration
static void atomizer_push(Particle value);
static Particle atomizer_pop();

/**
 * Allocates a string object into memory.
 * @param chars                 String for string object to own.
 * @param length                Length of string input.
 */
static ObjString* objString_alloc(char* chars, int length, uint32_t hash) {
    ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;

    atomizer_push(NUC_OBJ(string));
    table_set(&atomizer.strings, string, NUC_NULL);
    atomizer_pop();

    return string;
}

/**
 * Creates a string object from given series of characters.
 * @param chars             Chars to convert to string object.
 * @param length            Length of input string.
 */
ObjString* objString_copy(const char* chars, int length) {
    uint32_t hash = strings_hash(chars, length);

    // if a string intern is found, return it instead
    ObjString* interned = table_findString(&atomizer.strings, chars, length, hash);
    if (interned != NULL) return interned;

    char* heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return objString_alloc(heapChars, length, hash);
}

/**
 * Claims ownership of a string to a string object.
 * @param chars             Chars to claim ownership of.
 * @param length            Length of string.
 */
ObjString* objString_take(char* chars, int length) {
    uint32_t hash = strings_hash(chars, length);

    // if a string intern is found, claim ownership
    ObjString* interned = table_findString(&atomizer.strings, chars, length, hash);
    if (interned != NULL) {
        FREE_ARRAY(char, chars, length + 1);
        return interned;
    }

    // and return the allocation
    return objString_alloc(chars, length, hash);
}

#endif