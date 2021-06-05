#ifndef NUC_GARBAGE_COLLECTION_H
#define NUC_GARBAGE_COLLECTION_H

// need to forward some items
#include "../common.h"  // allows access to <stdio.h> below
void gc_markCompilerRoots();

// C Standard Library
#ifdef NUC_DEBUG_LOG_GC
    #include <stdio.h>
#endif

// GC growth factor
#define NUC_GC_HEAP_GROW_FACTOR 2

/****************
 *  GC MARKING  *
 ****************/

/**
 * Marks an object to NOT be garbage collected.
 * @param object            Object to be marked.
 */
void gc_markObject(Obj* object) {
    if (object == NULL) return;
    if (object->isMarked) return;

#ifdef NUC_DEBUG_LOG_GC
    printf("%p mark ", (void*)object);
    particle_print(NUC_OBJ(object), true);
    printf("\n");
#endif

    // mark the object
    object->isMarked = true;

    // and now continue add to the grayed values
    if (atomizer.grayCapacity < atomizer.grayCount + 1) {
        atomizer.grayCapacity = GROW_CAPACITY(atomizer.grayCapacity);
        atomizer.grayStack = (Obj**)realloc(atomizer.grayStack, sizeof(Obj*) * atomizer.grayCapacity);
        if (atomizer.grayStack == NULL) exit(1);  // bad memory reallocation
    }

    // item has been grayed
    atomizer.grayStack[atomizer.grayCount++] = object;
}

/**
 * Marks a particle value to not be garbage collected.
 * @param value             Particle to be marked.
 */
void gc_markValue(Particle value) {
    if (IS_OBJ(value)) gc_markObject(AS_OBJ(value));
}

/**
 * Marks a particle array from being gc'd.
 * @param arr               Array not to be garbage collected.
 */
void gc_markArray(ParticleArray* arr) {
    for (int i = 0; i < arr->count; i++) gc_markValue(arr->values[i]);
}

/**
 * Marks a table from garbage collection.
 * @param table             Table to be marked.
 */
void gc_markTable(Table* table) {
    for (int i = 0; i < table->capacity; i++) {
        Entry* entry = &table->entries[i];
        gc_markObject((Obj*)entry->key);
        gc_markValue(entry->value);
    }
}

/** Marks roots of all items NOT to be Garbage Collected. */
static void gv_markRoots() {
    for (Particle* slot = atomizer.stack; slot < atomizer.stackTop; slot++) gc_markValue(*slot);

    // want to mark all closures and upvalues
    for (int i = 0; i < atomizer.frameCount; i++) gc_markObject((Obj*)atomizer.frames[i].closure);
    for (ObjUpvalue* uv = atomizer.openUpvalues; uv != NULL; uv = uv->next) gc_markObject((Obj*)uv);

    // now want to mark tables / roots of atomizer
    gc_markTable(&atomizer.globals);
    gc_markObject((Obj*)atomizer.initString);
}

/************************
 *  GC TRACING / SWEEP  *
 ************************/

/** Blackens an object making it safe from GC. */
static void gc_blackenObject(Obj* object) {
#ifdef NUC_DEBUG_LOG_GC
    printf("%p blacken ", (void*)object);
    particle_print(NUC_OBJ(object), true);
    printf("\n");
#endif

    switch (object->type) {
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*)object;
            gc_markObject((Obj*)closure->reac);
            for (int i = 0; i < closure->uvCount; i++) gc_markObject((Obj*)closure->upvalues[i]);
        } break;
        case OBJ_REACTION: {
            ObjReaction* reac = (ObjReaction*)object;
            gc_markObject((Obj*)reac->name);
            gc_markArray(&reac->chunk.constants);
        } break;
        case OBJ_UPVALUE:
            gc_markValue(((ObjUpvalue*)object)->closed);
            break;
        case OBJ_MODEL: {
            ObjModel* model = (ObjModel*)object;
            gc_markObject((Obj*)model->name);
            gc_markTable(&model->methods);
            gc_markTable(&model->defaults);
        } break;
        case OBJ_INSTANCE: {
            ObjInstance* inst = (ObjInstance*)object;
            gc_markObject((Obj*)inst->model);
            gc_markTable(&inst->fields);
        } break;
        case OBJ_BOUND_METHOD: {
            ObjBoundMethod* bound = (ObjBoundMethod*)object;
            gc_markValue(bound->receiver);
            gc_markObject((Obj*)bound->method);
        } break;
        case OBJ_NATIVE:
        case OBJ_STRING:
            break;
    }
}

/** Traces the GC references to blacken. */
static void gc_traceRefs() {
    while (atomizer.grayCount > 0) {
        Obj* object = atomizer.grayStack[--atomizer.grayCount];
        gc_blackenObject(object);
    }
}

/** Removes all white references that are weakly linked. */
static void gc_tableRemoveWhite(Table* table) {
    for (int i = 0; i < table->capacity; i++) {
        Entry* entry = &table->entries[i];
        if (entry->key != NULL && !entry->key->obj.isMarked) table_delete(table, entry->key);
    }
}

/** Sweeps up the leftovers for garbage collection. */
static void gc_sweep() {
    Obj* prev = NULL;
    Obj* object = atomizer.objects;

    // iterate over the singly linked list
    while (object != NULL) {
        if (object->isMarked) {        // move onto next item
            object->isMarked = false;  // unmark for next time
            prev = object;
            object = object->next;
        } else {  // found an unreachable item
            Obj* unreached = object;
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
    printf("--- gc begin\n");

    // grab the before size
    size_t before = atomizer.bytesAllocated;
#endif

    gc_traceRefs();
    gc_tableRemoveWhite(&atomizer.strings);
    gc_sweep();

    // and update the GC frequency
    atomizer.nextGC = atomizer.bytesAllocated * NUC_GC_HEAP_GROW_FACTOR;

#ifdef NUC_DEBUG_LOG_GC
    printf("--- gc end\n");
    printf("    collected %zu bytes (from %zu to %zu), next at %zu\n",
           before - atomizer.bytesAllocated,
           before, atomizer.bytesAllocated,
           atomizer.nextGC);
#endif
}

/** Coordinates actually RUNNING a gc. */
static void atomizer_gc(size_t old_, size_t new_) {
    atomizer.bytesAllocated += new_ - old_;

    // want to coordinate some garbage collection if desired
    if (atomizer.bytesAllocated > atomizer.nextGC) {
        gc_collect();
    }
}

#endif