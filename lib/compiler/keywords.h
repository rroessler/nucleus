#ifndef NUC_KEYWORDS_H
#define NUC_KEYWORDS_H

// Nucleus Headers
#include "token.h"

#define TOTAL_KEYWORDS 20

/** Nucleus Keywords */
const char* __keywords[] = {
    // control
    "for",
    "do",
    "repif",
    "if",
    "else",
    "return",

    // constants
    "false",
    "true",
    "null",

    // lib
    "std",
    "math",
    "debug",

    // models
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

    T_FALSE,
    T_TRUE,
    T_NULL,

    T_STDLIB,
    T_MATH,
    T_DEBUG,

    T_THIS,
    T_SUPER,

    T_LET,
    T_CONST,
    T_MODEL,
    T_DERIVES,
    T_REACTION,
    T_REACTION,
};

// total number of directives (for iterating)
#define TOTAL_DIRECTIVES 1

// available directives
const char* __directives[] = {
    "mutate",
};

// and their associated tokens
const TokenType __directiveTokens[] = {
    T_MUTATE,
};

#endif