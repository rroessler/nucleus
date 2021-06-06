#ifndef NUC_PARSE_RULES_H
#define NUC_PARSE_RULES_H

// Nucleus Headers
#include "expression/logical.h"
#include "expression/variable.h"
#include "parsers/binary.h"
#include "parsers/call.h"
#include "parsers/constant.h"
#include "parsers/grouping.h"
#include "parsers/unary.h"
#include "precedence.h"
#include "token.h"

/** Globally accesible parse rules. */
ParseRule rules[] = {
    // punctuation
    [T_LEFT_PAREN] = {grouping, call, P_CALL},
    [T_RIGHT_PAREN] = {NULL, NULL, P_NONE},
    [T_LEFT_BRACE] = {baseModel, NULL, P_NONE},
    [T_RIGHT_BRACE] = {NULL, NULL, P_NONE},
    [T_COMMA] = {NULL, NULL, P_NONE},
    [T_PERIOD] = {NULL, accessor, P_CALL},
    [T_SEMICOLON] = {NULL, NULL, P_NONE},
    [T_COLON] = {NULL, NULL, P_NONE},

    // operators
    [T_MINUS] = {unary, binary, P_TERM},
    [T_PLUS] = {NULL, binary, P_TERM},
    [T_SLASH] = {NULL, binary, P_FACTOR},
    [T_STAR] = {NULL, binary, P_FACTOR},
    [T_AND] = {NULL, logical_and, P_AND},
    [T_OR] = {NULL, logical_or, P_OR},
    [T_BITW_AND] = {NULL, NULL, P_BITW_AND},
    [T_BITW_OR] = {NULL, NULL, P_BITW_OR},

    // comparisons / equalities
    [T_BANG] = {unary, NULL, P_NONE},
    [T_BANG_EQUAL] = {NULL, binary, P_EQUALITY},
    [T_EQUAL] = {NULL, NULL, P_ASSIGNMENT},
    [T_EQUAL_EQUAL] = {NULL, binary, P_EQUALITY},
    [T_GREATER] = {NULL, binary, P_COMPARISON},
    [T_GREATER_EQUAL] = {NULL, binary, P_COMPARISON},
    [T_LESS] = {NULL, binary, P_COMPARISON},
    [T_LESS_EQUAL] = {NULL, binary, P_COMPARISON},

    // constants / literals
    [T_IDENTIFIER] = {variable, NULL, P_NONE},
    [T_LIT_STRING] = {string, NULL, P_NONE},
    [T_LIT_NUMBER] = {number, NULL, P_NONE},
    [T_LIT_HEX] = {NULL, NULL, P_NONE},
    [T_LIT_OCT] = {NULL, NULL, P_NONE},
    [T_LIT_BIN] = {NULL, NULL, P_NONE},
    [T_NULL] = {literal, NULL, P_NONE},
    [T_TRUE] = {literal, NULL, P_NONE},
    [T_FALSE] = {literal, NULL, P_NONE},

    // type based tokens
    [T_LET] = {NULL, NULL, P_NONE},
    [T_MODEL] = {NULL, NULL, P_NONE},
    [T_REACTION] = {inlineReaction, NULL, P_NONE},

    // model based
    [T_THIS] = {modelThis, NULL, P_NONE},
    [T_SUPER] = {modelSuper, NULL, P_NONE},

    // library accessors
    [T_STDLIB] = {NULL, NULL, P_NONE},
    [T_MATH] = {NULL, NULL, P_NONE},
    [T_DEBUG] = {NULL, NULL, P_NONE},

    // directives
    [T_MUTATE] = {NULL, NULL, P_NONE},

    // control tokens
    [T_IF] = {NULL, NULL, P_NONE},
    [T_ELSE] = {NULL, NULL, P_NONE},
    [T_FOR] = {NULL, NULL, P_NONE},
    [T_DO] = {NULL, NULL, P_NONE},
    [T_REPEAT_IF] = {NULL, NULL, P_NONE},
    [T_RETURN] = {NULL, NULL, P_NONE},

    // miscellaneous
    [T_ERROR] = {NULL, NULL, P_NONE},
    [T_EOF] = {NULL, NULL, P_NONE},
};

/**
 * Gets the appropriate parse rule by given token type.
 * @param type              Type of rule to get.
 */
static ParseRule* getRule(TokenType type) { return &rules[type]; }

#endif