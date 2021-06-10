#ifndef NUC_GROUPING_H
#define NUC_GROUPING_H

// Nucleus Headers
#include "../expression.h"
#include "../parser.h"

/** Parses a grouped expression. */
static void rule_grouping(bool canAssign) {
    EXPRESSION;
    CONSUME(T_RIGHT_PAREN, "Expected ')' after grouped expression.");
}

#endif