#ifndef NUC_LITERALS_H
#define NUC_LITERALS_H

// Nucleus Headers
#include "../../../particle/value.h"
#include "../../emit.h"
#include "../parser.h"

// // forward declaration
// static void modelThis(bool canAssign);
// static void modelSuper(bool canAssign);
// static void reaction(ReactionType type);

/*********************
 *  NUMBER LITERALS  *
 *********************/

/** Parses a "number" constant. */
static void rule_number(bool canAssign) {
    double value = strtod(parser.previous.start, NULL);
    chunk_emitConstant(NUC_NUM(value));
}

/*********************
 *  STRING LITERALS  *
 *********************/

/** Parses a "string" constant. */
static void rule_string(bool canAssign) {
    chunk_emitConstant(NUC_OBJ(objString_copy(parser.previous.start + 1, parser.previous.length - 2)));
}

/***********************
 *  CONSTANT LITERALS  *
 ***********************/

/** Parses a "literal" constant. */
static void rule_literal(bool canAssign) {
    switch (parser.previous.type) {
        CASE_EMIT(T_FALSE, OP_FALSE);
        CASE_EMIT(T_TRUE, OP_TRUE);
        CASE_EMIT(T_NULL, OP_NULL);
        default:  // unreachable
            return;
    }
}

/***********************
 *  REACTION LITERALS  *
 ***********************/

/** Parses and compiles an prefix reaction. */
static void rule_inlineReaction(bool canAssign) { nuc_reaction(RT_REACTION); }

#endif