#ifndef NUC_TOKEN_H
#define NUC_TOKEN_H

/** Enumeration of Nucleus Tokens */
typedef enum {
    // punctuation
    T_LEFT_PAREN,
    T_RIGHT_PAREN,
    T_LEFT_BRACE,
    T_RIGHT_BRACE,
    T_COMMA,
    T_PERIOD,
    T_SEMICOLON,
    T_COLON,

    // operators
    T_MINUS,
    T_PLUS,
    T_SLASH,
    T_STAR,
    T_AND,
    T_OR,
    T_BITW_AND,
    T_BITW_OR,

    // comparisons / equalities
    T_BANG,
    T_BANG_EQUAL,
    T_EQUAL,
    T_EQUAL_EQUAL,
    T_GREATER,
    T_GREATER_EQUAL,
    T_LESS,
    T_LESS_EQUAL,

    // constants / literals
    T_IDENTIFIER,
    T_LIT_STRING,
    T_LIT_NUMBER,
    T_LIT_HEX,
    T_LIT_OCT,
    T_LIT_BIN,
    T_NULL,
    T_TRUE,
    T_FALSE,

    // type based tokens
    T_LET,
    T_CONST,
    T_MODEL,
    T_DERIVES,
    T_REACTION,

    // model tokens
    T_THIS,
    T_SUPER,

    // library accessors
    T_STDLIB,
    T_MATH,
    T_DEBUG,

    // directives
    T_MUTATE,

    // control tokens
    T_IF,
    T_ELSE,
    T_FOR,
    T_DO,
    T_REPEAT_IF,
    T_RETURN,

    // miscellaneous
    T_ERROR,
    T_EOF,
} TokenType;

/** Nucleus Lexical Token */
typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
    int col;
} Token;

/**
 * Generates a synthetic token at runtime.
 * @param type              Type of token.
 * @param word              String to give token.
 */
Token syntheticToken(TokenType type, const char* word) {
    Token token;
    token.type = type;
    token.start = word;
    token.length = (int)strlen(word);
    token.line = -1;
    token.col = -1;  // denotes column start number
    return token;
}

#endif