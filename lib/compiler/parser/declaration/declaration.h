#ifndef NUC_DECLARATION_H
#define NUC_DECLARATION_H

// Nucleus Headers
#include "../parser.h"
#include "../statement/statement.h"
#include "../synchronise.h"
#include "directive.h"
#include "model.h"
#include "reaction.h"
#include "variable.h"

/** Parses and Compiles Nucleus Declarations */
static void nuc_declaration() {
    if (MATCH(T_LET)) {
        fuser_variableDeclaration(NUC_MUTABLE);
    } else if (MATCH(T_CONST)) {
        fuser_variableDeclaration(NUC_IMMUTABLE);
    } else if (MATCH(T_MODEL)) {
        fuser_modelDeclaration();
    } else if (MATCH(T_REACTION)) {
        fuser_reactionDeclaration();
    } else if (MATCH(T_MUTATE)) {
        fuser_directiveDeclaration(&parser.previous);
    } else {
        nuc_statement();
    }

    // and synchronise the compilation if panicking
    if (parser.panicMode) parser_synchronise();
}

#endif