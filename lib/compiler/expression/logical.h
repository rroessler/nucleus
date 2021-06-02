#ifndef NUC_LOGICAL_STATEMENT_H
#define NUC_LOGICAL_STATEMENT_H

/** 
 * Parse a logical AND sequence. By doing so as a JUMP, this allows SKIPPING
 * continuous comparisons => speeding up code.
 */
static void logical_and(bool canAssign) {
    // start a jump
    int endJump = emitJump(OP_JUMP_IF_FALSE);

    // and parse the precedence
    emitByte(OP_POP);
    parsePrecedence(P_AND);

    // and finally patch the jump
    patchJump(endJump);
}

/** Same idea as the logical and, however we allow EITHER OR, for both items. */
static void logical_or(bool canAssign) {
    int elseJump = emitJump(OP_JUMP_IF_FALSE);
    int endJump = emitJump(OP_JUMP);

    patchJump(elseJump);
    emitByte(OP_POP);

    parsePrecedence(P_OR);
    patchJump(endJump);
}

#endif