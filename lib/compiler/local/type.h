#ifndef NUC_LOCAL_TYPEDEF_H
#define NUC_LOCAL_TYPEDEF_H

// Nucleus Headers
#include "../../common.h"
#include "../lexer/token.h"

/** Upvalue Compiler Structure */
typedef struct {
    uint16_t index;
    bool isLocal;
    bool immutable;
} nuc_Upvalue;

/** Nucleus Local Variable Structure */
typedef struct {
    Token name;       // associate token
    int depth;        // local depth
    bool isCaptured;  // captured within closure
    bool immutable;
} nuc_Local;

#endif