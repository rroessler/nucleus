#ifndef NUC_LOOP_STATEMENT_H
#define NUC_LOOP_STATEMENT_H

// Nucleus Includes
#include "../rules/operator.h"

/** Compiles a Nucleus 'repif' (while) statement. */
static void fuser_whileStatement() {
    int loopStart = fuser_currentChunk()->count;
    CONSUME(T_LEFT_PAREN, "Expected '(' after 'repif' keyword.");
    EXPRESSION;
    CONSUME(T_RIGHT_PAREN, "Expected ')' after 'repif' condition.");

    int exitJump = EMIT_JUMP(OP_JUMP_IF_FALSE);
    EMIT_BYTE(OP_POP);
    nuc_statement();
    EMIT_LOOP(loopStart);

    PATCH_JUMP(exitJump);
    EMIT_BYTE(OP_POP);
}

/** Essentially the same as the while loop, but preemptively runs the statement. */
static void fuser_doStatement() {
    size_t loopStart = fuser_currentChunk()->count;
    nuc_statement();

    // and now consume the condition
    CONSUME(T_REPEAT_IF, "Expected 'repif' keyword after 'do' statement.");
    CONSUME(T_LEFT_PAREN, "Expected '(' after 'while' keyword.");
    EXPRESSION;
    CONSUME(T_RIGHT_PAREN, "Expected ')' after 'repif' condition.");
    CONSUME(T_SEMICOLON, "Expected ';' after 'repif' condition.");

    size_t exitJump = EMIT_JUMP(OP_JUMP_IF_FALSE);
    EMIT_BYTE(OP_POP);
    EMIT_LOOP(loopStart);

    PATCH_JUMP(exitJump);
    EMIT_BYTE(OP_POP);
}

/** Eats an expected for loop initialiser, and returns the inclusivity operator. */
static TokenType fuser_eatForInitialiser() {
    // parse the variable name
    uint16_t initialiser = fuser_parseVariable("Expected a variable name.", NUC_MUTABLE);

    // consume a loop inclusivity operator
    TokenType inclusivity = fuser_consumeInclusivityOperator();
    if (inclusivity == T_ERROR) return inclusivity;  // immediately return an error if needed

    // and NOW eat the desired initialiser expression
    fuser_defineVariable(initialiser);
    return inclusivity;
}

/** Parses and Compiles a Nucleus For Statement */
static void fuser_forStatement() {
    fuser_beginScope();  // to ensure loop variable is encapsulated
    CONSUME(T_LEFT_PAREN, "Expected '(' after 'for' keyword.");

    // if we don't have a nucleus type, then chuck a wobbly
    if (!MATCH(T_LET)) PARSER_ERROR_AT_CURRENT("Expected a loop variable declaration.");

    // parse the loop variable name
    Token loopVariable = parser.current;
    TokenType inclusivity = fuser_eatForInitialiser();
    if (inclusivity == T_ERROR) return;  // do not continue

    // and now set the variable
    EXPRESSION;

    // now our loop ACTUALLY starts
    int loopStart = fuser_currentChunk()->count;
    CONSUME(T_COMMA, "Expected a ',' to deliminate the loop initialiser and loop exitor.");

    // want to read the loop comparator (if there)
    TokenType comparator = T_BANG_EQUAL;
    if (MATCH(T_LEFT_BRACE)) {
        comparator = parser.current.type;
        ADVANCE;
        CONSUME(T_RIGHT_BRACE, "Expected '}' after specifying loop comparator.");
    }

    // after the loop comparator should be the exitor value
    fuser_namedVariable(loopVariable, false, true);  // force a GET of the variable
    EXPRESSION;                                      // then the EXPRESSION
    parser_forceOperation(comparator);               // and FORCE a comparison

    // and coordinate the condition jump
    int exitJump = EMIT_JUMP(OP_JUMP_IF_FALSE);
    EMIT_BYTE(OP_POP);

    // lastly want to coordinate an incrementor
    bool useDefaultInc = true;  // denote whether to use default incrementor
    TokenType binify = T_PLUS;  // and set the default binary operator to use

    // if we do NOT have a default value
    if (!CHECK(T_RIGHT_PAREN)) {
        CONSUME(T_COMMA, "Expected a ',' to deliminate the loop exitor and loop incrementor.");

        // check if user is specifying a custom operator
        if (MATCH(T_LEFT_BRACE)) {
            binify = parser.current.type;  // save the operator
            ADVANCE;                       // and eat the operator
            CONSUME(T_RIGHT_BRACE, "Expected '}' after specifying incrementor operator.");
        }
    }

    // if not using default expression, then denote NOW
    if (!CHECK(T_RIGHT_PAREN)) useDefaultInc = false;

    // finally can compile the incrementor operation
    int bodyJump = EMIT_JUMP(OP_JUMP);
    int incrementStart = fuser_currentChunk()->count;

    // update the variable value
    if (useDefaultInc) {  // emit either the default constant
        EMIT_CONST(NUC_NUM(1));
    } else {  // or the given expression
        EXPRESSION;
    }
    parser_forceOperation(binify);  // force the binary operation incrementor

    // and patch some final items for the incrementor
    CONSUME(T_RIGHT_PAREN, "Expected ')' after 'for' clauses.");
    EMIT_LOOP(loopStart);
    loopStart = incrementStart;
    PATCH_JUMP(bodyJump);

    // eat the for loop body
    nuc_statement();
    EMIT_LOOP(loopStart);

    // patch the exit condition jump
    PATCH_JUMP(exitJump);
    EMIT_BYTE(OP_POP);
    fuser_endScope();  // and end the for loop scope
}

#endif