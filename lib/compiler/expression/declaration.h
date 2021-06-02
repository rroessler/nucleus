#ifndef NUC_DECLARATION_H
#define NUC_DECLARATION_H

// Nucleus Headers
#include "../statement/statement.h"
#include "../synchronise.h"
#include "reaction.h"
#include "variable.h"

/** Compiles a Nucleus Declaration */
static void declaration() {
    if (match(T_LET)) {
        variableDeclaration();
    } else if (match(T_REACTION)) {
        reactionDeclaration();
    } else {
        statement();
    }

    // and synchronise the compilation if panicing
    if (parser.panicMode) synchronise();
}

#endif