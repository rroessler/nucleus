#ifndef NUC_ATOMIZER_H
#define NUC_ATOMIZER_H

// Nucleus Headers
#include "../compiler/fuser.h"
#include "../particle/particle.h"
#include "core/call.h"
#include "core/core.h"
#include "core/flags.h"
#include "core/invoke.h"
#include "core/model.h"
#include "core/upvalue.h"
#include "garbage/collection.h"
#include "global.h"
#include "quantise/quantise.h"
#include "stdlib.h"

/*************************
 *  ATOMIZATION METHODS  *
 *************************/

/** Initialises an Atomizer Instance. */
void atomizer_init() {
    atomizer_resetStack();

    // init all globals
    atomizer.objects = NULL;
    atomizer.openUVs = NULL;
    table_init(&atomizer.globals);
    table_init(&atomizer.interns);
    table_init(&atomizer.natives);
    atomizer_initPrimatives(&atomizer.primatives);

    // create and intern the common strings
    atomizer.constructor = objString_copy("@construct", 10);
    atomizer.disruption = objString_copy("Disruption", 10);
    atomizer.modelLiteral = objString_copy("Model", 5);

    // initialise garbage collection variables
    atomizer.grayCount = 0;
    atomizer.grayCapacity = 0;
    atomizer.grayStack = NULL;
    atomizer.bytesAlloc = 0;
    atomizer.nextGC = 1024 * 1024;

    // initialise the atomizer flags
    NUC_RESET_AFLAGS;
    atomizer.exitCode = NUC_EXIT_SUCCESS;  // safe exit code

    // finally load in some available pre-defined "MODELS"
    nuc_defineModelLibrary();
}

/** Frees an Atomizer instance. */
void atomizer_free() {
    // free all global items and objects
    table_free(&atomizer.globals);
    table_free(&atomizer.interns);
    table_free(&atomizer.natives);
    atomizer_freePrimatives(&atomizer.primatives);

    // free the common strings
    atomizer.constructor = NULL;
    atomizer.disruption = NULL;
    atomizer.modelLiteral = NULL;

    // and free the grayed stack from memory
    free(atomizer.grayStack);
}

/**
 * Callable method that coordinates compiling and running
 * a Nucleus atomization.
 * @param source                    Nucleus source code.
 */
uint8_t nuc_atomize(const char* source) {
    // initially compile the source code
    nuc_ObjReaction* reaction = nuc_fuse(source);
    if (reaction == NULL) return NUC_EXIT_SYNTAX;

    // push the compiled script as a reaction global
    PUSH(NUC_OBJ(reaction));
    nuc_ObjClosure* closure = closure_new(reaction);
    POP();
    PUSH(NUC_OBJ(closure));
    atomizer_call(closure, 0);

    // quatises the atomization process
    atomizer_quantise();
    return atomizer.exitCode;
}

#endif