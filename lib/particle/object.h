#ifndef NUC_OBJECT_H
#define NUC_OBJECT_H

// Nucleus Headers
#include "../common.h"
#include "../utils/memory.h"
#include "../vm/global.h"
#include "objects/type.h"
#include "value.h"

/***********************
 *  AVAILABLE OBJECTS  *
 ***********************/

#include "objects/array.h"
#include "objects/closure.h"
#include "objects/model.h"
#include "objects/reaction.h"
#include "objects/string.h"

/****************
 *  OBJECT API  *
 ****************/

/**
 * Allocates an object of given size and type into memory.
 * @param size              Size of object.
 * @param type              Internal object type.
 */
nuc_Obj* obj_alloc(size_t size, nuc_ObjType type) {
    nuc_Obj* obj = (nuc_Obj*)nuc_realloc(NULL, 0, size);
    obj->type = type;
    obj->isMarked = false;

    // set up singley linked objects list
    obj->next = atomizer.objects;
    atomizer.objects = obj;

#ifdef NUC_DEBUG_GC  // display GC allocation
    printf("[\x1b[2;31mGC\x1b[0m] ");
    printf("\x1b[35m%p\x1b[0m: Allocated \x1b[33m%zu\x1b[0m for \x1b[33m%d\x1b[0m.\n", (void*)obj, size, type);
#endif

    // and return the object reference
    return obj;
}

/**
 * Frees an object from memory.
 * @param obj              Object to free from memory.
 */
static void obj_free(nuc_Obj* obj) {
#ifdef NUC_DEBUG_GC  // display GC freeing
    printf("[\x1b[2;31mGC\x1b[0m] ");
    printf("\x1b[35m%p\x1b[0m: Freed type \x1b[33m%d\x1b[0m.\n", (void*)obj, obj->type);
#endif

    // depending on the type
    switch (obj->type) {
        case OBJ_STRING: {
            nuc_ObjString* string = (nuc_ObjString*)obj;
            NUC_FREE_ARR(char, string->chars, string->length + 1);
            NUC_FREE(nuc_ObjString, obj);
        } break;
        case OBJ_REACTION: {
            nuc_ObjReaction* reac = (nuc_ObjReaction*)obj;
            chunk_free(&reac->chunk);
            NUC_FREE(nuc_ObjReaction, reac);
        } break;
        case OBJ_NATIVE: {
            NUC_FREE(nuc_ObjNative, obj);
        } break;
        case OBJ_CLOSURE: {
            nuc_ObjClosure* closure = (nuc_ObjClosure*)obj;
            NUC_FREE_ARR(nuc_ObjUpvalue*, closure->upvalues, closure->uvCount);
            NUC_FREE(nuc_ObjClosure, obj);
        } break;
        case OBJ_UPVALUE: {
            NUC_FREE(nuc_ObjUpvalue, obj);
        } break;
        case OBJ_MODEL: {
            nuc_ObjModel* model = (nuc_ObjModel*)obj;
            table_free(&model->methods);
            table_free(&model->defaults);
            NUC_FREE(nuc_ObjModel, obj);
        } break;
        case OBJ_INSTANCE: {
            nuc_ObjInstance* inst = (nuc_ObjInstance*)obj;
            table_free(&inst->fields);
            NUC_FREE(nuc_ObjInstance, obj);
        } break;
        case OBJ_BOUND_METHOD: {
            NUC_FREE(nuc_ObjBoundMethod, obj);
        } break;
        case OBJ_ARRAY: {
            nuc_ObjArr* arr = (nuc_ObjArr*)obj;
            NUC_FREE_ARR(nuc_ObjArr*, arr->values, arr->capacity);
            NUC_FREE(nuc_ObjArr, obj);
        } break;
    }
}

/**
 * Checks if a given object is currently empty.
 * @param obj                   Object particle to check.
 */
static bool obj_isEmpty(nuc_Obj* obj) {
    switch (obj->type) {
        case OBJ_STRING: {  // just to confirm length
            nuc_ObjString* string = (nuc_ObjString*)obj;
            return string->length == 0;
        }
        case OBJ_INSTANCE: {  // can only be empty if derives from model literal
            nuc_ObjInstance* inst = (nuc_ObjInstance*)obj;
            return inst->fields.count == 0 &&
                   memcmp(inst->model->name->chars, "Model", 5);
        }
        case OBJ_ARRAY: {
            nuc_ObjArr* arr = (nuc_ObjArr*)obj;
            return arr->count == 0;
        }
        default:  // all otherwise will ALWAYS return false
            return false;
    }
}

/**
 * Allows wrapping particle print blocks in a printf CHALK.
 * @param chalk             ANSI colour/style modifer code.
 */
#define NUC_PRETTIFY_WRAP(chalk, ...)         \
    if (prettify) printf("\x1b[" #chalk "m"); \
    { __VA_ARGS__; }                          \
    if (prettify) printf("\x1b[0m");

/**
 * Prints an internal object value.
 * @param value             Object Particle to print.
 * @param prettify          Whether to prettify the output.
 */
void obj_print(nuc_Particle value, bool prettify) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            NUC_PRETTIFY_WRAP(36, printf(prettify ? "\"%s\"" : "%s", AS_CSTRING(value)));
            break;
        case OBJ_REACTION:
            NUC_PRETTIFY_WRAP(32, reaction_print(AS_REACTION(value)));
            break;
        case OBJ_NATIVE:
            NUC_PRETTIFY_WRAP(32, printf("<reaction: native>"));
            break;
        case OBJ_CLOSURE:
            NUC_PRETTIFY_WRAP(32, reaction_print(AS_CLOSURE(value)->reaction));
            break;
        case OBJ_UPVALUE:
            NUC_PRETTIFY_WRAP(34, printf("<upvalue>"));
            break;
        case OBJ_MODEL:
            NUC_PRETTIFY_WRAP(33, printf("<model: %s>", AS_MODEL(value)->name->chars));
            break;
        case OBJ_INSTANCE:
            NUC_PRETTIFY_WRAP(33, printf("<instance: %s>", AS_INSTANCE(value)->model->name->chars));
            break;
        case OBJ_BOUND_METHOD:
            NUC_PRETTIFY_WRAP(32, reaction_print(AS_BOUND_METHOD(value)->method->reaction));
            break;
        case OBJ_ARRAY:
            NUC_PRETTIFY_WRAP(34, printf("<array: %lu>", AS_ARRAY(value)->count));
            break;
    }
}

#endif