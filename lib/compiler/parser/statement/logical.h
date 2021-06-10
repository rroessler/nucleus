#ifndef NUC_LOGICAL_STATEMENT_H
#define NUC_LOGICAL_STATEMENT_H

// Nucleus Headers
#include "../../emit.h"

/** 
 * Parse a logical AND sequence. By doing so as a JUMP, this allows SKIPPING
 * continuous comparisons => speeding up code.
 */
static void rule_logical_and(bool canAssign) {
    // start a jump
    int endJump = EMIT_JUMP(OP_JUMP_IF_FALSE);

    // and parse the precedence
    EMIT_BYTE(OP_POP);
    fuser_parsePrecedence(P_AND);

    // and finally patch the jump
    PATCH_JUMP(endJump);
}

/** Same idea as the logical and, however we allow EITHER OR, for both items. */
static void rule_logical_or(bool canAssign) {
    int elseJump = EMIT_JUMP(OP_JUMP_IF_FALSE);
    int endJump = EMIT_JUMP(OP_JUMP);
    PATCH_JUMP(elseJump);
    EMIT_BYTE(OP_POP);
    fuser_parsePrecedence(P_OR);
    EMIT_JUMP(endJump);
}

#endif