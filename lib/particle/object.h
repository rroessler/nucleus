#ifndef NUC_OBJECT_H
#define NUC_OBJECT_H

// Nucleus Headers
#include "../common.h"
#include "../memory.h"
#include "value.h"

/** Object Type Enumeration */
typedef enum {
    OBJ_CLOSURE,
    OBJ_UPVALUE,
    OBJ_REACTION,
    OBJ_NATIVE,
    OBJ_STRING,
} ObjType;

/** Nucleus Internal Object Value. */
struct Obj {
    ObjType type;
    bool isMarked;
    struct Obj* next;  // pointer to NEXT object on heap
};

/** Macro for getting the Object Type */
#define OBJ_TYPE(value) (AS_OBJ(value)->type)

// IS defines for objects
#define IS_CLOSURE(value) isObjType(value, OBJ_CLOSURE)
#define IS_REACTION(value) isObjType(value, OBJ_REACTION)
#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)
#define IS_STRING(value) isObjType(value, OBJ_STRING)

// AS defines for objects
#define AS_CLOSURE(value) ((ObjClosure*)AS_OBJ(value))
#define AS_REACTION(value) ((ObjReaction*)AS_OBJ(value))
#define AS_NATIVE(value) (((ObjNative*)AS_OBJ(value))->reac)
#define AS_STRING(value) ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)

/**
 * Checks if a particle have is of a specific internal object type.
 * @param value             Particle to check.
 * @param type              Type to check against.
 */
static inline bool isObjType(Particle value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

/****************
 *  OBJECT API  *
 ****************/

// forward declaraion
static Obj* obj_alloc(size_t size, ObjType type);
void obj_freeAll();

/** Macro to quickly allocate objects. */
#define ALLOCATE_OBJ(type, objectType) (type*)obj_alloc(sizeof(type), objectType)

// includes that will be resolved in compilation
#include "object/closure.h"
#include "object/native.h"
#include "object/reaction.h"
#include "object/string.h"

/**
 * Frees an object from memory.
 * @param object             Object to free from memory.
 */
static void obj_free(Obj* object) {
#ifdef NUC_DEBUG_LOG_GC
    printf("%p free type %d\n", (void*)object, object->type);
#endif

    switch (object->type) {  // need to free based on type
        case OBJ_STRING: {
            ObjString* string = (ObjString*)object;
            FREE_ARRAY(char, string->chars, string->length + 1);
            FREE(ObjString, object);
        } break;
        case OBJ_REACTION: {
            ObjReaction* reac = (ObjReaction*)object;
            chunk_free(&reac->chunk);
            FREE(ObjReaction, reac);
        } break;
        case OBJ_NATIVE: {
            FREE(ObjNative, object);
        } break;
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*)object;
            FREE_ARRAY(ObjUpvalue*, closure->upvalues, closure->uvCount);
            FREE(ObjClosure, object);
        } break;
        case OBJ_UPVALUE: {
            FREE(ObjUpvalue, object);
        } break;
    }
}

/**
 * Prints an internal object value.
 * @param value             Particle to print.
 * @param prettify          Whether to prettify the output.
 */
void obj_print(Particle value, bool prettify) {
    switch (OBJ_TYPE(value)) {  // this will also filter BAD values
        case OBJ_STRING:
            if (prettify) printf("\x1b[36m\"");
            printf("%s", AS_CSTRING(value));
            if (prettify) printf("\"\x1b[0m");
            break;
        case OBJ_REACTION:
            if (prettify) printf("\x1b[32m");
            reaction_print(AS_REACTION(value));
            if (prettify) printf("\x1b[0m");
            break;
        case OBJ_NATIVE:
            if (prettify) printf("\x1b[32m");
            printf("<native rn>");
            if (prettify) printf("\x1b[0m");
            break;
        case OBJ_CLOSURE:
            if (prettify) printf("\x1b[32m");
            reaction_print(AS_CLOSURE(value)->reac);
            if (prettify) printf("\x1b[0m");
            break;
        case OBJ_UPVALUE:
            if (prettify) printf("\x1b[34m");
            printf("upvalue");
            if (prettify) printf("\x1b[0m");
            break;
    }
}

#endif