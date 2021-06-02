#ifndef NUC_PRECEDENCE_H
#define NUC_PRECEDENCE_H

/** Precedence Enumeration (order matters !) */
typedef enum {
    P_NONE,
    P_ASSIGNMENT,  // =
    P_OR,          // ||
    P_AND,         // &&
    P_BITW_OR,     // |
    P_BITW_AND,    // &
    P_EQUALITY,    // == !=
    P_COMPARISON,  // < > <= >=
    P_SHIFT,       // << >>
    P_TERM,        // + -
    P_FACTOR,      // * / %
    P_UNARY,       // ! -
    P_CALL,        // . ()
    P_PRIMARY
} Precedence;

/** Parse Function declaration */
typedef void (*ParseFn)(bool canAssign);

/** Parsing Rule for Pratt Parser */
typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence prec;
} ParseRule;

// forward declaration
static ParseRule* getRule(TokenType type);

/** Parses a Nucleus expression based on precedence. */
static void parsePrecedence(Precedence prec) {
    advance();  // initially advance the parser
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == NULL) {
        error("Expected an expression.");
        return;
    }

    // set up an assignment boolean to allow assignment
    bool canAssign = prec <= P_ASSIGNMENT;

    // run the prefix rule
    prefixRule(canAssign);

    // and now check for infix
    while (prec <= getRule(parser.current.type)->prec) {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule(canAssign);
    }

    // check for invalid assignment targets, eg: a * b = 5
    if (canAssign && match(T_EQUAL)) {
        error("Invalid assignment target.");
    }
}

#endif