#ifndef NUC_KEYWORDS_H
#define NUC_KEYWORDS_H

// Nucleus Headers
#include "token.h"

// defines for denoting TOTAL items
#define NUC_TOTAL_KEYWORDS 22
#define NUC_TOTAL_DIRECTIVES 2

/** Nucleus Keywords */
const char* __keywords[] = {
    // control
    "for",
    "do",
    "repif",
    "if",
    "else",
    "return",
    "try",
    "catch",

    // constants
    "false",
    "true",
    "null",

    // lib
    "std",
    "math",

    // models
    "new",
    "this",
    "super",

    // types
    "let",
    "const",
    "model",
    "derives",
    "rn",
    "reaction",
};

/** Associate Keyword Tokens */
const TokenType __keywordTokens[] = {
    T_FOR,
    T_DO,
    T_REPEAT_IF,
    T_IF,
    T_ELSE,
    T_RETURN,
    T_TRY,
    T_CATCH,

    T_FALSE,
    T_TRUE,
    T_NULL,

    T_STDLIB,
    T_MATH,

    T_NEW,
    T_THIS,
    T_SUPER,

    T_LET,
    T_CONST,
    T_MODEL,
    T_DERIVES,
    T_REACTION,
    T_REACTION,
};

// available directives
const char* __directives[] = {
    "@mutate",
    "@construct",
};

// and their associated tokens
const TokenType __directiveTokens[] = {
    T_MUTATE,
    T_IDENTIFIER,
};

#endif