#ifndef NUC_GARBAGE_COLLECTION_H
#define NUC_GARBAGE_COLLECTION_H

// C Standard Library
#ifdef NUC_DEBUG_GC
    #include <stdio.h>
#endif

// Nucleus Headers
#include "../../common.h"
#include "../../compiler/core/mark.h"
#include "../../particle/table.h"
#include "../../particle/value.h"

// garbage collection growth factor
#define NUC_GC_HEAP_GROWTH_FACTOR 2

/****************
 *  GC MARKING  *
 ****************/

/**
 * Marks an object to NOT be garbage collected.
 * @param object            Object to be marked.
 */
void gc_markObject(nuc_Obj* object) {
    if (object == NULL) return;
    if (object->isMarked) return;

#ifdef NUC_DEBUG_GC  // garbage collection log
    printf("[\x1b[2;31mGC\x1b[0m] ");
    printf("\x1b[35m%p\x1b[0m: Marked ", (void*)object);
    particle_print(NUC_OBJ(object), true);
    printf("\n");
#endif

    // mark the object
    object->isMarked = true;

    // and now continue add to the grayed values
    if (atomizer.grayCapacity < atomizer.grayCount + 1) {
        atomizer.grayCapacity = NUC_CAP_GROW_FAST(atomizer.grayCapacity);
        atomizer.grayStack = (nuc_Obj**)realloc(atomizer.grayStack, sizeof(nuc_Obj*) * atomizer.grayCapacity);
        if (atomizer.grayStack == NULL) nuc_immediateExit(NUC_EXIT_MEM, "Could not reallocate space for garbage collection.");  // bad memory reallocation
    }

    // item has been grayed
    atomizer.grayStack[atomizer.grayCount++] = object;
}

/**
 * Marks a particle value to not be garbage collected.
 * @param value             Particle to be marked.
 */
void gc_markValue(nuc_Particle value) {
    if (IS_OBJ(value)) gc_markObject(AS_OBJ(value));
}

/**
 * Marks a particle array from being gc'd.
 * @param arr               Array not to be garbage collected.
 */
void gc_markArray(nuc_ParticleArr* arr) {
    for (int i = 0; i < arr->count; i++) gc_markValue(arr->values[i]);
}

/**
 * Marks a table from garbage collection.
 * @param table             Table to be marked.
 */
void gc_markTable(nuc_Table* table) {
    for (int i = 0; i < table->capacity; i++) {
        nuc_Entry* entry = &table->entries[i];
        gc_markObject((nuc_Obj*)entry->key);
        gc_markValue(entry->value);
    }
}

/** Marks roots of all items NOT to be Garbage Collected. */
static void gc_markRoots() {
    for (nuc_Particle* slot = atomizer.stack; slot < atomizer.top; slot++) gc_markValue(*slot);

    // want to mark all closures and upvalues
    for (int i = 0; i < atomizer.frameCount; i++) gc_markObject((nuc_Obj*)atomizer.frames[i].closure);
    for (nuc_ObjUpvalue* uv = atomizer.openUVs; uv != NULL; uv = uv->next) gc_markObject((nuc_Obj*)uv);

    // now want to mark tables / roots of atomizer
    gc_markTable(&atomizer.globals);
    gc_markObject((nuc_Obj*)atomizer.constructor);
    gc_markCompilerRoots();
}

/************************
 *  GC TRACING / SWEEP  *
 ************************/

/** Blackens an object making it safe from GC. */
static void gc_blackenObject(nuc_Obj* object) {
#ifdef NUC_DEBUG_GC
    printf("[\x1b[2;31mGC\x1b[0m] ");
    printf("\x1b[35m%p\x1b[0m: Blackened ", (void*)object);
    particle_print(NUC_OBJ(object), true);
    printf("\n");
#endif

    switch (object->type) {
        case OBJ_CLOSURE: {
            nuc_ObjClosure* closure = (nuc_ObjClosure*)object;
            gc_markObject((nuc_Obj*)closure->reaction);
            for (int i = 0; i < closure->uvCount; i++) gc_markObject((nuc_Obj*)closure->upvalues[i]);
        } break;
        case OBJ_REACTION: {
            nuc_ObjReaction* reaction = (nuc_ObjReaction*)object;
            gc_markObject((nuc_Obj*)reaction->name);
            gc_markArray(&reaction->chunk.constants);
        } break;
        case OBJ_UPVALUE:
            gc_markValue(((nuc_ObjUpvalue*)object)->closed);
            break;
        case OBJ_MODEL: {
            nuc_ObjModel* model = (nuc_ObjModel*)object;
            gc_markObject((nuc_Obj*)model->name);
            gc_markTable(&model->methods);
            gc_markTable(&model->defaults);
        } break;
        case OBJ_INSTANCE: {
            nuc_ObjInstance* inst = (nuc_ObjInstance*)object;
            gc_markObject((nuc_Obj*)inst->model);
            gc_markTable(&inst->fields);
        } break;
        case OBJ_BOUND_METHOD: {
            nuc_ObjBoundMethod* bound = (nuc_ObjBoundMethod*)object;
            gc_markValue(bound->receiver);
            gc_markObject((nuc_Obj*)bound->method);
        } break;
        case OBJ_ARRAY:
        case OBJ_NATIVE:  // these items are coordinated through intersn / globals
        case OBJ_STRING:  // so no need to worry
            break;
    }
}

/** Traces the GC references to blacken. */
static void gc_traceRefs() {
    while (atomizer.grayCount > 0) {
        nuc_Obj* object = atomizer.grayStack[--atomizer.grayCount];
        gc_blackenObject(object);
    }
}

/** Removes all white references that are weakly linked. */
static void gc_tableRemoveWhite(nuc_Table* table) {
    for (int i = 0; i < table->capacity; i++) {
        nuc_Entry* entry = &table->entries[i];
        if (entry->key != NULL && !entry->key->obj.isMarked) table_delete(table, entry->key);
    }
}

/** Sweeps up the leftovers for garbage collection. */
static void gc_sweep() {
    nuc_Obj* prev = NULL;
    nuc_Obj* object = atomizer.objects;

    // iterate over the singly linked list
    while (object != NULL) {
        if (object->isMarked) {        // move onto next item
            object->isMarked = false;  // unmark for next time
            prev = object;
            object = object->next;
        } else {  // found an unreachable item
            nuc_Obj* unreached = object;
            object = object->next;
            if (prev != NULL) {
                prev->next = object;
            } else {
                atomizer.objects = object;
            }

            // and free the unreached object
            obj_free(unreached);
        }
    }
}

/*****************
 *  GC MAIN API  *
 *****************/

/** Collects Garbage :) */
void gc_collect() {
#ifdef NUC_DEBUG_LOG_GC
    printf("\x1b[2m[\x1b[0m\x1b[31mGC\x1b[0m\x1b[2m[\x1b[0m");
    printf(" Started\n");

    // grab the before size
    size_t before = atomizer.bytesAllocated;
#endif

    gc_markRoots();
    gc_traceRefs();
    gc_tableRemoveWhite(&atomizer.interns);
    gc_sweep();

    // and update the GC frequency
    atomizer.nextGC = atomizer.bytesAlloc * NUC_GC_HEAP_GROWTH_FACTOR;

#ifdef NUC_DEBUG_LOG_GC
    printf("\x1b[2m[\x1b[0m\x1b[31mGC\x1b[0m\x1b[2m[\x1b[0m");
    printf(" Ended\n");
    printf("\x1b[2m[\x1b[0m\x1b[31mGC\x1b[0m\x1b[2m[\x1b[0m");
    printf(" Collected \x1b[33m%zu\x1b[0m bytes (from \x1b[33m%zu\x1b[0m to \x1b[33m%zu\x1b[0m), next GC @ \x1b[33m%zu\x1b[0m bytes.\n",
           before - atomizer.bytesAllocated,
           before, atomizer.bytesAllocated,
           atomizer.nextGC);
#endif
}

/** Coordinates actually RUNNING a gc. */
static void atomizer_gc(size_t old_, size_t new_) {
    atomizer.bytesAlloc += new_ - old_;

    // want to coordinate some garbage collection if desired
    if (atomizer.bytesAlloc > atomizer.nextGC) {
        gc_collect();
    }
}

#endif