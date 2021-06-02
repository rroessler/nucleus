#ifndef NUC_GROUPING_H
#define NUC_GROUPING_H

// Nucleus Headers
#include "../parser.h"

// forward declaraion
static void expression();

/** Parses a grouped expression. */
static void grouping(bool canAssign) {
    expression();
    consume(T_RIGHT_PAREN, "Expected ')' after grouped expression.");
}

#endif