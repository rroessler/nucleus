#ifndef NUC_ATOMIZER_CORE_H
#define NUC_ATOMIZER_CORE_H

// Nucleus Headers
#include "../../particle/value.h"
#include "../global.h"

/****************************
 *  ATOMIZER HELPER MACROS  *
 ****************************/

/** 
 * Macro to help push a value onto the Atomizer. 
 * @param val           Particle value to push.
 */
#define PUSH(val) atomizer_push(val)

/** Macro to help pop a value from the Atomizer. */
#define POP() atomizer_pop()

/** Coordinates a double POP. */
#define POP_DOUBLE() \
    POP();           \
    POP()

/**
 * Peeks a set distance onto the atomizer stack.
 * @param dist              Distance to peek.
 */
#define PEEK(dist) atomizer_peek(dist)

/***************************
 *  ATOMIZER CORE METHODS  *
 ***************************/

/** Resets the current atomizers stack. */
inline static void atomizer_resetStack() {
    atomizer.top = atomizer.stack;
    atomizer.frameCount = 0;
}

/**
 * Pushes a particle value to the Atomizer stack.
 * @param value             Particle to push.
 */
static inline void atomizer_push(nuc_Particle value) {
    *atomizer.top = value;
    atomizer.top++;
}

/** Pops the top particle value from the Atomizer stack. */
static inline nuc_Particle atomizer_pop() {
    atomizer.top--;
    return *atomizer.top;
}

/**
 * Peeks a value at a given distance on the stack.
 * @param distance          Distance of item from top of stack.
 */
static inline nuc_Particle atomizer_peek(size_t dist) { return atomizer.top[-1 - dist]; }

/**
 * Interns a given string to the Atomizer.
 * @param string            String to intern.
 */
static void atomizer_setIntern(nuc_ObjString* string) {
    PUSH(NUC_OBJ(string));
    table_set(&atomizer.interns, string, NUC_NULL);
    POP();
}

/**
 * Finds an interned string within the Atomizer.
 * @param chars                 String to get intern of.
 * @param length                Length of string input.
 * @param hash                  String hash.
 */
static inline nuc_ObjString* atomizer_getIntern(const char* chars, int length, uint32_t hash) {
    return table_findString(&atomizer.interns, chars, length, hash);
}

#endif