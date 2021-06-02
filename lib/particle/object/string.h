#ifndef NUC_STRING_H
#define NUC_STRING_H

// C Standard Library
#include <stdio.h>
#include <string.h>

// Nucleus Headers
#include "../../memory.h"
#include "../../utils/strings.h"
#include "../object.h"

/** Nucleus String Object */
struct ObjString {
    Obj obj;
    int length;
    char* chars;
    uint32_t hash;
};

// forward declaration of items since these all required the atomizer
static ObjString* objString_alloc(char* chars, int length, uint32_t hash);
ObjString* objString_copy(const char* chars, int length);
ObjString* objString_take(char* chars, int length);

#endif