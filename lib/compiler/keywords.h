#ifndef NUC_KEYWORDS_H
#define NUC_KEYWORDS_H

// Nucleus Headers
#include "token.h"

#define TOTAL_KEYWORDS 15

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
    "debug",

    // types
    "let",
    "model",
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
    T_DEBUG,

    T_LET,
    T_MODEL,
    T_REACTION,
    T_REACTION,
};

#endif