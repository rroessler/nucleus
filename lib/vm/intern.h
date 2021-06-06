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
 * Unescapes a given input string.
 * @param chars             String to unescape.
 * @param length            Length of input string.
 */
static const char* objString_unescape(const char* chars, int* length) {
    // switch case helper
#define CASE_UNESCAPE(c, hex)       \
    case c:                         \
        strncat(unescaped, hex, 1); \
        newLen++;                   \
        break

    char* unescaped = ALLOCATE(char, *length + 1);  // set a placeholder for the output
    int newLen = 0;                                 // and also a new length
    for (int i = 0; i < *length; i++) {
        if (chars[i] != '\\') {
            strncat(unescaped, &chars[i], 1);
            newLen++;
            continue;
        }

        // otherwise found a string escape sequence
        switch (chars[++i]) {
            CASE_UNESCAPE('a', "\a");
            CASE_UNESCAPE('b', "\b");
            CASE_UNESCAPE('e', "\e");
            CASE_UNESCAPE('f', "\f");
            CASE_UNESCAPE('n', "\n");
            CASE_UNESCAPE('r', "\r");
            CASE_UNESCAPE('t', "\t");
            CASE_UNESCAPE('v', "\v");
            CASE_UNESCAPE('\\', "\\");
            CASE_UNESCAPE('\'', "\'");
            CASE_UNESCAPE('"', "\"");
            CASE_UNESCAPE('?', "\?");  // single char escape sequences

            case 'x': {  // byte escape
                const char* start = &chars[++i];
                i++;  // waste a char for 1-byte width
                long byte = strtol(start, NULL, 16);
                strncat(unescaped, (char*)&byte, 1);
                newLen++;  // and increment the length by 1
            } break;

            default:  // ignore the other items for now
                error("Found an invalid escape sequence character.");
                break;
        }
    }

    // if same length, ignore change
    if (newLen == *length) return chars;

    length = &newLen;  // modify the length value
    unescaped = (char*)realloc(unescaped, newLen + 1);
    unescaped[newLen] = '\0';
    return unescaped;  // and return the unescaped string
}

/**
 * Creates a string object from given series of characters.
 * @param input             Chars to convert to string object.
 * @param inLen             Length of input string.
 */
ObjString* objString_copy(const char* input, int inLen) {
    // want to initially UNESCAPE an input string
    int length = inLen;
    const char* chars = objString_unescape(input, &length);
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