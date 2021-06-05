#ifndef NUC_REACTION_DECLARATION_H
#define NUC_REACTION_DECLARATION_H

// Nucleus Includes
#include "../emit.h"
#include "../parser.h"
#include "variable.h"

/** 
 * Parses and Compiles a Nucleus Reaction.
 * @param type          Reaction type.
 */
static void reaction(ReactionType type) {
    // begin compilation of the reaction in an enclosed fuser
    Fuser fuser;
    fuser_init(&fuser, type);
    fuser_beginScope();

    // and then actually parse some items
    consume(T_LEFT_PAREN, "Expected '(' after reaction name.");

    // want to eat all the reaction arguments definitions
    if (!check(T_RIGHT_PAREN)) {
        do {
            current->reac->arity++;
            if (current->reac->arity > UINT8_MAX) errorAtCurrent("Cannot have more than 255 parameters for reactions.");
            uint8_t constant = parseVariable("Expected a parameter name.");
            defineVariable(constant, false);  // not mutable so can be changed

            // and to set defaults, want to coordinate defaulting NON-defaulted arguments
            if (match(T_EQUAL)) {
                size_t myLocal = current->localCount - 1;
                EMIT_SHORT(OP_GET_LOCAL, myLocal);
                EMIT_SHORT(OP_NULL, OP_EQUAL);

                int jumpIndex = emitJump(OP_JUMP_IF_FALSE_OR_POP);
                fuser_beginScope();
                expression();
                emitByte(OP_MUTATE);
                EMIT_SHORT(OP_SET_LOCAL, myLocal);
                fuser_endScope();

                patchJump(jumpIndex);
                current->reac->defaults++;
            } else if (current->reac->defaults) {
                error("Non-defaulted reaction parameter specified after a defaulted reaction parameter.");
                break;
            }

        } while (match(T_COMMA));
    }

    // parse the reaction body now
    consume(T_RIGHT_PAREN, "Expected ')' after reaction parameters.");
    consume(T_LEFT_BRACE, "Expected '{' before reaction body.");
    block();

    // and finally clean up the compilation and emit to parent compilation
    ObjReaction* reac = fuser_end();
    EMIT_SHORT(OP_CLOSURE, makeConstant(NUC_OBJ(reac)));

    // and emit the closure upvalues
    for (int i = 0; i < reac->uvCount; i++) {
        EMIT_SHORT(fuser.upvalues[i].isLocal ? 1 : 0, fuser.upvalues[i].index);
    }
}

/** Compiles a Reaction Declaration. */
static void reactionDeclaration() {
    uint8_t global = parseVariable("Expected a reaction name.");
    fuser_markInitialised();
    reaction(RT_REACTION);
    defineVariable(global, true);  // set reaction name as immutable
}

#endif