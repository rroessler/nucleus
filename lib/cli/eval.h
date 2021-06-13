#ifndef NUC_CLI_EVAL_H
#define NUC_CLI_EVAL_H

// Nucleus Headers
#include "../vm/atomizer.h"

/**
 * Evaluates a given input as Nucleus Source code.
 * @param input             Input to evaluate.
 */
void nuc_eval(const char* input) {
    atomizer_init();
    nuc_atomize(input);
}

#endif