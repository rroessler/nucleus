#ifndef NUC_PARSER_H
#define NUC_PARSER_H

// C Standard Library
#include <stdio.h>
#include <stdlib.h>

// Nucleus Headers
#include "lexer.h"
#include "token.h"

/*************
 *  GLOBALS  *
 *************/

/** Parser Structure */
typedef struct {
    Token current;
    Token previous;
    bool panicMode;
    bool hadError;
} Parser;

// globally accesible parser
Parser parser;

/********************
 *  ERROR HANDLING  *
 ********************/

// FORWARD DECLARAION
static void errorAtCurrent(const char* message);
static void error(const char* message);

/**
 * Displays an 'errorAt' error with given error message.
 * @param token                 Token error occured on.
 * @param message               Error message.
 */
static void parser_errorAt(Token* token, const char* message) {
    if (parser.panicMode) return;  // do not continueif already in panic mode
    parser.panicMode = true;       // set into panic mode

    // and display the error
    fprintf(stderr, "\n\x1b[2m[line\x1b[0m \x1b[33m%d\x1b[0m\x1b[2m]\x1b[0m \x1b[1;31mError\x1b[0m", token->line);
    if (token->type == T_EOF) {
        fprintf(stderr, " at end-of-file");
    } else if (token->type == T_ERROR) {
        /** ignore as already set */
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    // and finish display
    fprintf(stderr, ": %s\n", message);

    // and display WHERE the error occured
    const char* line = lexer_getLine(token->line);
    fprintf(stderr, "%s\n", line);
    for (size_t i = 0; i < token->col; i++) fprintf(stderr, " ");
    fprintf(stderr, "%s\n", "\x1b[3;31m^~~~ Here!\x1b[0m\n");

    // and lastly denote about the parser error
    parser.hadError = true;
}

/****************
 *  PARSER API  *
 ****************/

/** Advances the parsers currently retrieved tokens. */
static void advance() {
    // update the previous token
    parser.previous = parser.current;

    // iterate infinitely to ensure errors can be plowed through
    for (;;) {
        parser.current = lexer_scan();
        if (parser.current.type != T_ERROR) break;

        // bad token so display an error
        errorAtCurrent(parser.current.start);
    }
}

/**
 * Consumes a current token IFF the type matches. Otherwise throws an error
 * with the given message.
 * @param type                      Expected type of token to consume.
 * @param message                   Error message.
 */
static void consume(TokenType type, const char* message) {
    if (parser.current.type == type) {
        advance();
        return;
    }

    // throw an error as type failed
    errorAtCurrent(message);
}

/** 
 * Checks if the current token matches the given type. 
 * @param type              Expected type.
 */
static bool check(TokenType type) { return parser.current.type == type; }

/** 
 * Checks if the previous token matches the given type. 
 * @param type              Expected type.
 */
static bool checkPrevious(TokenType type) { return parser.previous.type == type; }

/**
 * Advances the parser if a match to a token type occurs.
 * @param type              Type to match.
 */
static bool match(TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}

// /** Advances the parser if a match to a nucleus token type occurs. */
// static bool matchNucleusType() {
//     if (!isNucleusType(parser.current.type)) return false;
//     advance();
//     return true;
// }

/** Consumes a valid loop inclusivity operator and returns it. Otherwise throws an error. */
static TokenType consumeInclusivityOperator() {
    if (match(T_COLON)) return T_COLON;
    errorAtCurrent("Expected a 'for' loop inclusivity operator.");
    return T_ERROR;
}

#endif