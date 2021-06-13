#ifndef NUC_MARK_COMPILER_H
#define NUC_MARK_COMPILER_H

// Nucleus Includes
#include "../global.h"

// FORWARD DECLARATIONS
void gc_markObject(nuc_Obj* object);

/** Marks the compilers roots as safe from garbage collection. */
void gc_markCompilerRoots() {
    nuc_Fuser* fuser = current;
    while (fuser != NULL) {
        gc_markObject((nuc_Obj*)fuser->reaction);
        fuser = fuser->enclosing;
    }
}

#endif