#ifndef NUC_REALLOC_H
#define NUC_REALLOC_H

// C Standard Library
#include <stdio.h>
#include <stdlib.h>

// Nucleus Headers
#include "../../common.h"
#include "../disruptions/codes.h"
#include "../disruptions/immediate.h"

// FORWARD DECLARATION
static void atomizer_gc(size_t prev, size_t now);

/**
 * Reallocates a pointer with a new size.
 * @param ptr                   Pointer to reallocate.
 * @param prev                  Old size of pointer.
 * @param now                   New size of pointer.
 */
void* nuc_realloc(void* ptr, size_t prev, size_t now) {
    atomizer_gc(prev, now);  // collect garbarge

    // if the new size is zero, then want to free
    if (now == 0) {
        free(ptr);
        return NULL;
    }

    // otherwise reallocate as necessary
    void* res = realloc(ptr, now);
    if (res == NULL) nuc_immediateExit(NUC_EXIT_MEM, "Could not reallocate memory.");
    return res;
}

#endif