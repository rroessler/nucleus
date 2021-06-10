#ifndef NUC_RULE_NATIVE_H
#define NUC_RULE_NATIVE_H

// Nucleus Headers
#include "../declaration/variable.h"
#include "../parser.h"

/** Parses a known Native as a GLOBAL identifier. */
static void rule_native(bool canAssign) {
    int constant = fuser_identifierConstant(&parser.previous);
    EMIT_BYTE(OP_GET_NATIVE);
    EMIT_UINT16(constant);
}

#endif