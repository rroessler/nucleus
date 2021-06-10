#ifndef NUC_DIRECTIVE_DECLARATION_H
#define NUC_DIRECTIVE_DECLARATION_H

// Nucleus Headers
#include "../../emit.h"

// FORWARD DECLARATIONS
static void nuc_declaration();

/**
 * Coordinates a directive declaration.
 * @param directive             Directive to coordinate.
 */
static void fuser_directiveDeclaration(Token* directive) {
    switch (directive->type) {
        case T_MUTATE:
            NUC_SET_CFLAG(NUC_CFLAG_MUTATE_NEXT);
            nuc_declaration();  // expect FIRST const to be set to be ignored
            break;

        // ignore invalid items
        default:
            return;
    }
}

#endif