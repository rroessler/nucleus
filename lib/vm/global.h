#ifndef NUC_ATOMIZER_GLOBALS_H
#define NUC_ATOMIZER_GLOBALS_H

// Nucleus Headers
#include "../bytecode/chunk.h"
#include "../common.h"
#include "../particle/particle.h"
#include "../particle/table.h"
#include "core/frame.h"
#include "primatives.h"

/*************************
 *  GLOBAL DECLARATIONS  *
 *************************/

/** Atomizer Virtual Machine Structure */
typedef struct {
    nuc_Chunk* chunk;  // current bytecode chunk
    uint8_t* ip;       // instruction pointer

    nuc_CallFrame frames[FRAMES_MAX];  // available call frames
    int frameCount;
    nuc_ObjUpvalue* openUVs;  // pointer list of open upvalues

    // stack variables
    nuc_Particle stack[STACK_MAX];  // stack items
    nuc_Particle* top;              // pointer to top of stack

    // global variables
    nuc_Obj* objects;            // global objects list
    nuc_ObjString* constructor;  // constructor string
    nuc_Table globals;           // script globals
    nuc_Table interns;           // global string interns
    nuc_Table natives;           // native methods
    nuc_Primatives primatives;   // primative particle methods

    // garbage collection
    int grayCount;     // grayed particles
    int grayCapacity;  // current gray capacity
    nuc_Obj** grayStack;
    size_t bytesAlloc;  // bytes allocated on last GC
    size_t nextGC;      // next GC bytes size

    // atomizer flags
    uint32_t flags;
    uint8_t exitCode;
} Atomizer;

/** Global Atomizer Instance */
Atomizer atomizer;

#endif