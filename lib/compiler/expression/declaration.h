#ifndef NUC_DECLARATION_H
#define NUC_DECLARATION_H

// Nucleus Headers
#include "../statement/statement.h"
#include "../synchronise.h"
#include "directive.h"
#include "model.h"
#include "reaction.h"
#include "variable.h"

/** Compiles a Nucleus Declaration */
static void declaration() {
    if (match(T_LET)) {
        variableDeclaration(false);  // set as mutable
    } else if (match(T_CONST)) {
        variableDeclaration(true);  // set as immutable
    } else if (match(T_MODEL)) {
        modelDeclaration();
    } else if (match(T_REACTION)) {
        reactionDeclaration();
    } else if (match(T_MUTATE)) {
        directiveDeclaration(&parser.previous);
    } else {
        statement();
    }

    // and synchronise the compilation if panicing
    if (parser.panicMode) synchronise();
}

#endif