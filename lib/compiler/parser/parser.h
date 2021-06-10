#ifndef NUC_PARSER_H
#define NUC_PARSER_H

// C Standard Library
#include <stdio.h>
#include <stdlib.h>

// Nucleus Headers
#include "../lexer/lexer.h"
#include "../lexer/token.h"

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

/**
 * Displays an 'errorAt' error with given error message.
 * @param token                 Token error occured on.
 * @param message               Error message.
 */
static void parser_errorAt(Token* token, const char* message) {
    if (parser.panicMode) return;  // do not continueif already in panic mode
    parser.panicMode = true;       // set into panic mode

    // and display the error
    fprintf(stderr, "\n\x1b[2m[line\x1b[0m \x1b[33m%d\x1b[0m\x1b[2m]\x1b[0m \x1b[1;31mSyntax Error\x1b[0m", token->line);
    if (token->type == T_EOF) {
        fprintf(stderr, " found at end of the script.");
    } else if (token->type == T_ERROR) {
        /** ignore as already set */
    } else {
        fprintf(stderr, " found at '%.*s'", token->length, token->start);
    }

    // and finish display
    fprintf(stderr, ": %s\n", message);

    // and display WHERE the error occured
    const char* line = lexer_getLine(token->line);
    fprintf(stderr, "%s\n", line);
    for (int i = 0; i < token->col; i++) fprintf(stderr, " ");
    fprintf(stderr, "%s\n", "\x1b[3;31m^~~~ Here!\x1b[0m\n");

    // and lastly denote about the parser error
    parser.hadError = true;
}

/**
 * Throws an error from the previously parsed token.
 * @param message               Error message.
 */
#define PARSER_ERROR_AT(msg) parser_errorAt(&parser.previous, msg)

/**
 * Throws an error from the current token.
 * @param message               Error message.
 */
#define PARSER_ERROR_AT_CURRENT(msg) parser_errorAt(&parser.current, msg)

/****************
 *  PARSER API  *
 ****************/

#define ADVANCE parser_advance()
#define CONSUME(type, msg) parser_consume(type, msg)
#define CHECK(type) parser_check(type)
#define CHECK_PREV(type) parser_checkPrevious(type)
#define MATCH(type) parser_match(type)

/** Advances the parsers currently retrieved tokens. */
static void parser_advance() {
    // update the previous token
    parser.previous = parser.current;

    // iterate infinitely to ensure errors can be plowed through
    for (;;) {
        parser.current = lexer_scan();
        if (parser.current.type != T_ERROR) break;

        // bad token so display an error
        PARSER_ERROR_AT_CURRENT(parser.current.start);
    }
}

/**
 * Consumes a current token IFF the type matches. Otherwise throws an error
 * with the given message.
 * @param type                      Expected type of token to consume.
 * @param message                   Error message.
 */
static void parser_consume(TokenType type, const char* message) {
    if (parser.current.type == type) {
        parser_advance();
        return;
    }

    // throw an error as type failed
    PARSER_ERROR_AT_CURRENT(message);
}

/** 
 * Checks if the current token matches the given type. 
 * @param type              Expected type.
 */
static bool parser_check(TokenType type) { return parser.current.type == type; }

/**
 * Advances the parser if a match to a token type occurs.
 * @param type              Type to match.
 */
static bool parser_match(TokenType type) {
    if (!parser_check(type)) return false;
    parser_advance();
    return true;
}

/*******************
 *  MISCELLANEOUS  *
 *******************/

/** Consumes a valid loop inclusivity operator and returns it. Otherwise throws an error. */
static TokenType fuser_consumeInclusivityOperator() {
    if (MATCH(T_COLON)) return T_COLON;
    PARSER_ERROR_AT_CURRENT("Expected a 'for' loop inclusivity operator.");
    return T_ERROR;
}

#endif