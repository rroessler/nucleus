#ifndef NUC_LEXER_H
#define NUC_LEXER_H

// C Standard Library
#include <stdlib.h>

// Nucleus Headers
#include "../../common.h"
#include "../../utils/strings.h"
#include "../../vm/disruptions/immediate.h"
#include "keywords.h"
#include "token.h"

/*********************
 *  LEXER STRUCTURE  *
 *********************/

/** Lexer Type Definition */
typedef struct {
    const char *base;  // allows find LINES within the lexer
    const char *start;
    const char *current;
    int line;  // line number
    int col;   // column number
} Lexer;

/** Create a global lexer. */
Lexer lexer;

/**
 * Initialises the global lexer with a current source.
 * @param source            Source to initialise with.
 */
void lexer_init(const char *source) {
    lexer.base = source;
    lexer.start = source;
    lexer.current = source;
    lexer.line = 1;
    lexer.col = 0;
}

/*******************
 *  LEXER METHODS  *
 *******************/

/**
 * Gets a line from the source code.
 * @param line              Line to retrieve from source memory.
 */
const char *lexer_getLine(int line) {
    // set up some iterables
    const char *curLine = lexer.base;
    int counter = 1;

    // and iterate until we find the current token line
    while (curLine) {
        const char *nextLine = strchr(curLine, '\n');
        int curLineLen = nextLine ? (nextLine - curLine) : strlen(curLine);

        // only copy the line when we reach the line
        if (counter == line) {
            char *tempStr = (char *)malloc(curLineLen + 1);
            if (!tempStr) nuc_immediateExit(NUC_EXIT_MEM, "Could not retrieve source line.");
            memcpy(tempStr, curLine, curLineLen);
            tempStr[curLineLen] = '\0';
            return tempStr;
        }

        curLine = nextLine ? (nextLine + 1) : NULL;
        counter++;
    }

    // bad result so just return empty string
    return "";
}

/** Returns if the lexer is at it's end. */
static inline bool lexer_isAtEnd() { return *lexer.current == '\0'; }

/** Increments the current lexer positioning. */
static inline void lexer_incPos() {
    lexer.current++;
    lexer.col++;
}

/** Advances the lexer one character forward. */
static inline char lexer_advance() {
    lexer_incPos();            // advance the actual position
    return lexer.current[-1];  // and display the previous char
}

/** Peeks the current lexer character. */
static inline char lexer_peek() { return *lexer.current; }

/** Peeks the next available lexer character. */
static inline char lexer_peekNext() {
    if (lexer_isAtEnd()) return '\0';
    return lexer.current[1];
}

/**
 * Checks to see if the given character is what is current at the top of the current stack. Advances the
 * current pointer if so.
 * @param expected              Character to expect.
 */
static inline bool lexer_match(char expected) {
    if (lexer_isAtEnd()) return false;
    if (*lexer.current != expected) return false;
    lexer_incPos();
    return true;
}

/**
 * Creates an error token with a given message.
 * @param message               Error message to set.
 */
static Token lexer_error(const char *message) {
    Token token;
    token.type = T_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = lexer.line;
    token.col = lexer.col - (int)(lexer.current - lexer.start);  // denotes column start number
    return token;
}

/**
 * Tokenizes the currently found string.
 * @param type                  Type of token to create.
 */
static Token lexer_tokenize(TokenType type) {
    Token token;
    token.type = type;
    token.start = lexer.start;
    token.length = (int)(lexer.current - lexer.start);
    token.line = lexer.line;
    token.col = lexer.col - token.length;  // denotes column start number
    return token;
}

/***********************
 *  PARTICLE SCANNING  *
 ***********************/

/** Skips all whitespace */
static void lexer_skipWS() {
    for (;;) {  // iterate indefinitely
        char c = lexer_peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':  // general whitespace
                lexer_advance();
                break;

            case '\n':  // found a NEW LINE
                lexer.line++;
                lexer.col = -1;  // set the column to -1 as new line
                lexer_advance();
                break;

            case '#':  // comment declarations
                if (lexer_peekNext() == '!') {
                    // found a match for multi-line comment
                    while (!lexer_isAtEnd()) {
                        // if we match a '!' and can peek a '#' then exit block comment
                        if (lexer_match('!') && lexer_peek() == '#') {
                            lexer_advance();
                            break;
                        }

                        // anytime a newline is encountered, fix the line / column
                        if (lexer_peek() == '\n') {
                            lexer.line++;
                            lexer.col = -1;
                        }

                        // otherwise advance as normal
                        lexer_advance();
                    }

                    // if currently at the end, then return EOF
                    if (lexer_isAtEnd()) return;  // want to exit
                    break;
                }

                // single line comments
                while (lexer_peek() != '\n' && !lexer_isAtEnd()) lexer_advance();
                break;  // just need to consume until next line

            default:  // break for all other items
                return;
        }
    }
}

/**
 * Creates a based number token given a range base for numeric characters.
 * @param high              High digit available.
 * @param type              Token type to return.
 */
static Token lexer_basedNumber(const char high, TokenType type) {
    char c = lexer_peek();                              // retrieve an initial character
    while (c >= '0' && c <= high) c = lexer_advance();  // and continue advancing
    return lexer_tokenize(type);
}

/** Tokenizes a number. */
static Token lexer_number() {
    if (lexer_peek() == '0') {  // potentially a octal, hex or binary value
        switch (lexer_peekNext()) {
            case 'x':
            case 'X':  // HEXADECIMAL
                while (strings_isHex(lexer_peek())) lexer_advance();
                return lexer_tokenize(T_LIT_HEX);
            case 'b':
            case 'B':  // BINARY
                return lexer_basedNumber('1', T_LIT_BIN);
            case 'o':
            case 'O':  // OCTAL
                return lexer_basedNumber('7', T_LIT_OCT);
            default:  // just return as a single value
                break;
        }
    }

    // whilst we can see digits continue
    while (strings_isDigit(lexer_peek())) lexer_advance();

    // parsing fractional part
    if (lexer_peek() == '.' && strings_isDigit(lexer_peekNext())) {
        lexer_advance();  // eat the period
        while (strings_isDigit(lexer_peek())) lexer_advance();
    }

    // generic number token to return
    return lexer_tokenize(T_LIT_NUMBER);
}

/** Coordinates tokenizing a string. */
static Token lexer_string() {
    while (!lexer_isAtEnd()) {
        // break immediately when possible
        if (lexer_peek() == '\"') break;

        // skip over quotation escapes
        if (lexer_match('\\') && lexer_peek() == '\"') {
            lexer_advance();
            continue;  // need to CONTINUE immediately
        }

        // EOL => unterminated string
        if (lexer_peek() == '\n') return lexer_error("Unterminated string literal.");
        lexer_advance();  // otherwise advance as normal
    }

    // check if at end, and if so then string is unterminated
    if (lexer_isAtEnd()) return lexer_error("Unterminated string literal.");

    lexer_advance();  // eat the closing quote
    return lexer_tokenize(T_LIT_STRING);
}

/**
 * Checks if a the currently found string matches a given one.
 * @param matcher                   String to match.
 * @param length                    Length of string to match.
 */
static bool lexer_matchString(const char *matcher, int length) {
    if (lexer.current - lexer.start != length) return false;
    return memcmp(lexer.start, matcher, length) == 0;
}

/** Tokenizes a given directive */
static Token lexer_directive() {
    lexer_advance();  // eat the @
    while (strings_isAlpha(lexer_peek()) || strings_isDigit(lexer_peek())) lexer_advance();

    // iterate over the directives
    for (int i = 0; i < NUC_TOTAL_DIRECTIVES; i++) {
        // and return the appropriate item
        if (lexer_matchString(__directives[i], strlen(__directives[i]))) return lexer_tokenize(__directiveTokens[i]);
    }

    // otherwise we couldn't match a valid directive
    return lexer_error("Unknown directive declared.");
}

/** Converts a given token to a native instruction.*/
static Token lexer_stdlib(const char *message, TokenType type) {
    // got the LIB or DEBUG token, so we want to check the next character is a period
    if (lexer_peek() != '.') return lexer_error(message);

    do {  // can iterate ad-finitum as we eat accessors here
          // eat the current comma
        lexer_advance();

        // and make sure we have a valid identifier
        if (!strings_isAlpha(lexer_peek())) return lexer_error("Expected a property/method after standard library declaration.");
        while (strings_isAlpha(lexer_peek()) || strings_isDigit(lexer_peek())) lexer_advance();
    } while (lexer_peek() == '.');

    // and return as the decided type
    return lexer_tokenize(type);
}

/** Coerces a keyword from the currently parsed identifier. */
static Token lexer_coerceKeyword() {
    // iterate over the keywords
    for (int i = 0; i < NUC_TOTAL_KEYWORDS; i++) {
        // check for a valid match
        if (lexer_matchString(__keywords[i], strlen(__keywords[i]))) {
            if (__keywordTokens[i] == T_STDLIB) {  // return as a native reaction / property
                return lexer_stdlib("Expected a period after reserved keyword \"std\".", T_STDLIB);
            } else if (__keywordTokens[i] == T_MATH) {
                return lexer_stdlib("Expected a period after reserved keyword \"math\".", T_MATH);
            }
            return lexer_tokenize(__keywordTokens[i]);
        }
    }

    // otherwise is a generic token
    return lexer_tokenize(T_IDENTIFIER);
}

/** Tokenizes a given identifier. */
static Token lexer_identifier() {
    while (strings_isAlpha(lexer_peek()) || strings_isDigit(lexer_peek())) lexer_advance();
    return lexer_coerceKeyword();
}

/********************
 *  TOKEN SCANNING  *
 ********************/

/** Coordinates scanning for the next available token. */
Token lexer_scan() {
    lexer_skipWS();  // ignore whitespace

    // move the starting character pointer
    lexer.start = lexer.current;

    // if we reach the end then tokenize
    if (lexer_isAtEnd()) return lexer_tokenize(T_EOF);

    // grab the initial char
    char c = lexer_advance();

    // if we have some common types, then tokenize
    if (strings_isAlpha(c)) return lexer_identifier();
    if (strings_isDigit(c)) return lexer_number();

    switch (c) {
        // single-character tokens
        case '(':
            return lexer_tokenize(T_LEFT_PAREN);
        case ')':
            return lexer_tokenize(T_RIGHT_PAREN);
        case '{':
            return lexer_tokenize(T_LEFT_BRACE);
        case '}':
            return lexer_tokenize(T_RIGHT_BRACE);
        case ';':
            return lexer_tokenize(T_SEMICOLON);
        case ':':
            return lexer_tokenize(T_COLON);
        case '.':
            return lexer_tokenize(T_PERIOD);
        case ',':
            return lexer_tokenize(T_COMMA);
        case '-':
            return lexer_tokenize(T_MINUS);
        case '+':
            return lexer_tokenize(T_PLUS);
        case '/':
            return lexer_tokenize(T_SLASH);
        case '*':
            return lexer_tokenize(T_STAR);
        case '%':
            return lexer_tokenize(T_PERCENTAGE);
        case '^':
            return lexer_tokenize(T_HAT);
        case '~':
            return lexer_tokenize(T_BITW_NOT);

        // multi-character operators
        case '!':
            return lexer_tokenize(lexer_match('=') ? T_BANG_EQUAL : T_BANG);
        case '=':
            return lexer_tokenize(lexer_match('=') ? T_EQUAL_EQUAL : T_EQUAL);
        case '<':
            return lexer_tokenize(
                lexer_match('=')
                    ? T_LESS_EQUAL
                    : (lexer_match('<') ? T_ROL : T_LESS));
        case '>':
            return lexer_tokenize(
                lexer_match('=')
                    ? T_GREATER_EQUAL
                    : (lexer_match('<') ? T_ROR : T_GREATER));
        case '&':
            return lexer_tokenize(lexer_match('&') ? T_AND : T_BITW_AND);
        case '|':
            return lexer_tokenize(lexer_match('|') ? T_OR : T_BITW_OR);

        // strings
        case '"':
            return lexer_string();
        // directives
        case '@':
            return lexer_directive();
    }

    // otherwise return an error token
    return lexer_error("Found an unexpected character.");
}

#endif