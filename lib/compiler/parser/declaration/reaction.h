#ifndef NUC_REACTION_DECLARATION_H
#define NUC_REACTION_DECLARATION_H

// Nucleus Headers
#include "../../emit.h"
#include "../parser.h"
#include "../statement/block.h"
#include "variable.h"

// FORWARD DECLARATIONS
static void fuser_init(nuc_Fuser* fuser, nuc_ReactionType type);
static nuc_ObjReaction* fuser_complete();

/**********************
 *  REACTION METHODS  *
 **********************/

/** 
 * Parses and Compiles a Nucleus Reaction.
 * @param type          Reaction type.
 */
static void nuc_reaction(nuc_ReactionType type) {
    // begin compilation of the reaction in an enclosed fuser
    nuc_Fuser fuser;
    fuser_init(&fuser, type);
    fuser_beginScope();

    // and then actually parse some items
    CONSUME(T_LEFT_PAREN, "Expected '(' after reaction name.");

    // want to eat all the reaction arguments definitions
    if (!CHECK(T_RIGHT_PAREN)) {
        do {
            current->reaction->arity++;
            if (current->reaction->arity > UINT8_MAX) PARSER_ERROR_AT_CURRENT("Cannot have more than 255 parameters for reactions.");
            uint16_t constant = fuser_parseVariable("Expected a parameter name.", NUC_MUTABLE);  // want argument to be mutable
            fuser_defineVariable(constant);

            // and to set defaults, want to coordinate defaulting NON-defaulted arguments
            if (MATCH(T_EQUAL)) {
                size_t myLocal = current->localCount - 1;
                EMIT_BYTE(OP_GET_LOCAL);
                EMIT_UINT16(myLocal);
                EMIT_SHORT(OP_NULL, OP_EQUAL);

                int jumpIndex = EMIT_JUMP(OP_JUMP_IF_FALSE_OR_POP);
                fuser_beginScope();
                EXPRESSION;
                EMIT_BYTE(OP_SET_LOCAL);
                EMIT_UINT16(myLocal);
                fuser_endScope();

                PATCH_JUMP(jumpIndex);
                current->reaction->defaults++;
            } else if (current->reaction->defaults) {
                PARSER_ERROR_AT("Non-defaulted reaction parameter specified after a defaulted reaction parameter.");
                break;
            }

        } while (MATCH(T_COMMA));
    }

    // parse the reaction body now
    CONSUME(T_RIGHT_PAREN, "Expected ')' after reaction parameters.");
    CONSUME(T_LEFT_BRACE, "Expected '{' before reaction body.");
    fuser_block();

    // and finally clean up the compilation and emit to parent compilation
    nuc_ObjReaction* reaction = fuser_complete();
    EMIT_BYTE(OP_CLOSURE);
    uint16_t constant = chunk_makeConstant(NUC_OBJ(reaction));
    EMIT_UINT16(constant);

    // and emit the closure upvalues
    for (int i = 0; i < reaction->uvCount; i++) {
        EMIT_BYTE(fuser.upvalues[i].isLocal ? 1 : 0);
        EMIT_UINT16(fuser.upvalues[i].index);
    }
}

/** Compiles a Reaction Declaration. */
static void fuser_reactionDeclaration() {
    uint16_t global = fuser_parseVariable("Expected a reaction name.", NUC_IMMUTABLE);  // reactions are inherently immutable
    fuser_markInitialised();
    nuc_reaction(RT_REACTION);
    fuser_defineVariable(global);  // set reaction name as immutable
}

#endif