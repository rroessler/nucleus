#ifndef NUC_PARSE_RULES_H
#define NUC_PARSE_RULES_H

// Nucleus Headers
#include "../declaration/model.h"
#include "../declaration/variable.h"
#include "../statement/logical.h"
#include "array.h"
#include "base.h"
#include "call.h"
#include "grouping.h"
#include "literals.h"
#include "native.h"
#include "operator.h"
#include "precedence.h"

/** Globally Accessible Parse Rules */
nuc_ParseRule rules[] = {
    // punctuation
    [T_LEFT_PAREN] = {rule_grouping, rule_call, P_CALL},
    [T_RIGHT_PAREN] = {NULL, NULL, P_NONE},
    [T_LEFT_BRACE] = {rule_baseModel, NULL, P_NONE},
    [T_RIGHT_BRACE] = {NULL, NULL, P_NONE},
    [T_LEFT_BRACK] = {rule_arrayLiteral, rule_member, P_CALL},
    [T_RIGHT_BRACK] = {NULL, NULL, P_NONE},
    [T_COMMA] = {NULL, NULL, P_NONE},
    [T_PERIOD] = {NULL, rule_accessor, P_CALL},
    [T_SEMICOLON] = {NULL, NULL, P_NONE},
    [T_COLON] = {NULL, NULL, P_NONE},

    // operators
    [T_MINUS] = {rule_unary, rule_binary, P_TERM},
    [T_PLUS] = {NULL, rule_binary, P_TERM},
    [T_SLASH] = {NULL, rule_binary, P_FACTOR},
    [T_STAR] = {NULL, rule_binary, P_FACTOR},
    [T_PERCENTAGE] = {NULL, rule_binary, P_FACTOR},
    [T_POW] = {NULL, rule_binary, P_EXP},
    [T_AND] = {NULL, rule_logical_and, P_AND},
    [T_OR] = {NULL, rule_logical_or, P_OR},
    [T_BITW_AND] = {NULL, rule_binary, P_BITW_AND},
    [T_BITW_OR] = {NULL, rule_binary, P_BITW_OR},
    [T_BITW_NOT] = {rule_unary, NULL, P_NONE},
    [T_ROL] = {NULL, rule_binary, P_SHIFT},
    [T_ROR] = {NULL, rule_binary, P_SHIFT},
    [T_HAT] = {NULL, rule_binary, P_BITW_XOR},

    // comparisons / equalities
    [T_BANG] = {rule_unary, NULL, P_NONE},
    [T_BANG_EQUAL] = {NULL, rule_binary, P_EQUALITY},
    [T_EQUAL] = {NULL, NULL, P_ASSIGNMENT},
    [T_EQUAL_EQUAL] = {NULL, rule_binary, P_EQUALITY},
    [T_GREATER] = {NULL, rule_binary, P_COMPARISON},
    [T_GREATER_EQUAL] = {NULL, rule_binary, P_COMPARISON},
    [T_LESS] = {NULL, rule_binary, P_COMPARISON},
    [T_LESS_EQUAL] = {NULL, rule_binary, P_COMPARISON},

    // constants / literals
    [T_IDENTIFIER] = {rule_variable, NULL, P_NONE},
    [T_LIT_STRING] = {rule_string, NULL, P_NONE},
    [T_LIT_NUMBER] = {rule_number, NULL, P_NONE},
    [T_LIT_HEX] = {rule_nhex, NULL, P_NONE},
    [T_LIT_OCT] = {rule_noct, NULL, P_NONE},
    [T_LIT_BIN] = {rule_nbin, NULL, P_NONE},
    [T_NULL] = {rule_literal, NULL, P_NONE},
    [T_TRUE] = {rule_literal, NULL, P_NONE},
    [T_FALSE] = {rule_literal, NULL, P_NONE},

    // type based tokens
    [T_LET] = {NULL, NULL, P_NONE},
    [T_MODEL] = {NULL, NULL, P_NONE},
    [T_REACTION] = {rule_inlineReaction, NULL, P_NONE},

    // model based
    [T_NEW] = {NULL, NULL, P_NONE},
    [T_THIS] = {rule_modelThis, NULL, P_NONE},
    [T_SUPER] = {rule_modelSuper, NULL, P_NONE},

    // library accessors
    [T_STDLIB] = {rule_native, NULL, P_NONE},
    [T_MATH] = {rule_native, NULL, P_NONE},

    // directives
    [T_MUTATE] = {NULL, NULL, P_NONE},

    // control tokens
    [T_IF] = {NULL, NULL, P_NONE},
    [T_ELSE] = {NULL, NULL, P_NONE},
    [T_FOR] = {NULL, NULL, P_NONE},
    [T_DO] = {NULL, NULL, P_NONE},
    [T_REPEAT_IF] = {NULL, NULL, P_NONE},
    [T_RETURN] = {NULL, NULL, P_NONE},
    [T_TRY] = {NULL, NULL, P_NONE},
    [T_CATCH] = {NULL, NULL, P_NONE},

    // miscellaneous
    [T_ERROR] = {NULL, NULL, P_NONE},
    [T_EOF] = {NULL, NULL, P_NONE},
};

#endif