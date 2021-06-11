#ifndef NUC_PRECEDENCE_H
#define NUC_PRECEDENCE_H

// Nucleus Headers
#include "../../fuser.h"
#include "../../lexer/token.h"

/** Precedence Enumeration (order matters !) */
typedef enum {
    P_NONE,
    P_ASSIGNMENT,  // =
    P_OR,          // ||
    P_AND,         // &&
    P_BITW_OR,     // |
    P_BITW_XOR,    // ^
    P_BITW_AND,    // &
    P_EQUALITY,    // == !=
    P_COMPARISON,  // < > <= >=
    P_SHIFT,       // << >>
    P_TERM,        // + -
    P_FACTOR,      // * / %
    P_EXP,         // **
    P_UNARY,       // ! -
    P_CALL,        // . () []
    P_PRIMARY
} Precedence;

/** Parse Function declaration */
typedef void (*nuc_ParseFunction)(bool canAssign);

/** Parsing Rule for Pratt Parser */
typedef struct {
    nuc_ParseFunction prefix;
    nuc_ParseFunction infix;
    Precedence prec;
} nuc_ParseRule;

// now include the rules
#include "rules.h"

/**
 * Gets the appropriate parse rule by given token type.
 * @param type              Type of rule to get.
 */
static nuc_ParseRule* parser_getRule(TokenType type) { return &rules[type]; }

/**
 * Parses a Nucleus expression based on the given precedence.
 * @param prec                  Current precedence.
 */
static void fuser_parsePrecedence(Precedence prec) {
    ADVANCE;  // initially advance the parser
    nuc_ParseFunction prefixRule = parser_getRule(parser.previous.type)->prefix;
    if (prefixRule == NULL) {
        PARSER_ERROR_AT("Expected an expression.");
        return;
    }

    // set up an assignment boolean to allow assignment
    bool canAssign = prec <= P_ASSIGNMENT;

    // run the prefix rule
    prefixRule(canAssign);

    // and now check for infix
    while (prec <= parser_getRule(parser.current.type)->prec) {
        ADVANCE;
        nuc_ParseFunction infixRule = parser_getRule(parser.previous.type)->infix;
        infixRule(canAssign);
    }

    // check for invalid assignment targets, eg: a * b = 5
    if (canAssign && MATCH(T_EQUAL)) {
        PARSER_ERROR_AT("Invalid assignment target.");
    }
}

#endif