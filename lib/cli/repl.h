#ifndef NUC_REPL_H
#define NUC_REPL_H

// C Standard Library
#include <stdio.h>
#include <stdlib.h>

// Nucleus Headers
#include "../vm/atomizer.h"

/** Read-Eval-Print Loop */
void nuc_repl() {
    char line[1024];
    for (;;) {
        printf("\x1b[3;32mnucleus\x1b[0m -> ");  // prompt
        if (!fgets(line, sizeof(line), stdin)) break;

        // patch some items here before atomizing

        nuc_atomize(line);  // and complete
    }
}

#endif