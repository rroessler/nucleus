#ifndef NUC_LOOP_STATEMENT_H
#define NUC_LOOP_STATEMENT_H

// forward declarations

/** Compiles a Nucleus 'repif' (while) statement. */
static void whileStatement() {
    int loopStart = currentChunk()->count;
    consume(T_LEFT_PAREN, "Expected '(' after 'repif' keyword.");
    expression();
    consume(T_RIGHT_PAREN, "Expected ')' after 'repif' condition.");

    int exitJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    statement();
    emitLoop(loopStart);

    patchJump(exitJump);
    emitByte(OP_POP);
}

/** Essentially the same as the while loop, but preemptively runs the statement. */
static void doStatement() {
    int loopStart = currentChunk()->count;
    statement();

    // and now consume the condition
    consume(T_REPEAT_IF, "Expected 'repif' keyword after 'do' statement.");
    consume(T_LEFT_PAREN, "Expected '(' after 'while' keyword.");
    expression();
    consume(T_RIGHT_PAREN, "Expected ')' after 'repif' condition.");
    consume(T_SEMICOLON, "Expected ';' after 'repif' condition.");

    int exitJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    emitLoop(loopStart);

    patchJump(exitJump);
    emitByte(OP_POP);
}

/** Eats an expected for loop initialiser, and returns the inclusivity operator. */
static TokenType eatForInitialiser() {
    // parse the variable name
    uint8_t initialiser = parseVariable("Expected a variable name.");

    // consume a loop inclusivity operator
    TokenType inclusivity = consumeInclusivityOperator();
    if (inclusivity == T_ERROR) return inclusivity;  // immediately return an error if needed

    // and NOW eat the desired initialiser expression
    defineVariable(initialiser);
    return inclusivity;
}

/** Parses and Compiles a Nucleus For Statement */
static void forStatement() {
    fuser_beginScope();  // to ensure loop variable is encapsulated
    consume(T_LEFT_PAREN, "Expected '(' after 'for' keyword.");

    // if we don't have a nucleus type, then chuck a wobbly
    if (!match(T_LET)) errorAtCurrent("Expected a loop variable declaration.");

    // parse the loop variable name
    Token loopVariable = parser.current;
    TokenType inclusivity = eatForInitialiser();
    if (inclusivity == T_ERROR) return;  // do not continue

    // and now set the variable
    expression();

    // now our loop ACTUALLY starts
    int loopStart = currentChunk()->count;
    consume(T_COMMA, "Expected a ',' to deliminate the loop initialiser and loop exitor.");

    // want to read the loop comparator (if there)
    TokenType comparator = T_BANG_EQUAL;
    if (match(T_LEFT_BRACE)) {
        comparator = parser.current.type;
        advance();
        consume(T_RIGHT_BRACE, "Expected '}' after specifying loop comparator.");
    }

    // after the loop comparator should be the exitor value
    namedVariable(loopVariable, false, true);  // force a GET of the variable
    expression();                              // then the EXPRESSION
    forceOperation(comparator);                // and FORCE a comparison

    // and coordinate the condition jump
    int exitJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);

    // lastly want to coordinate an incrementor
    bool useDefaultInc = true;  // denote whether to use default incrementor
    TokenType binify = T_PLUS;  // and set the default binary operator to use

    // if we do NOT have a default value
    if (!check(T_RIGHT_PAREN)) {
        consume(T_COMMA, "Expected a ',' to deliminate the loop exitor and loop incrementor.");

        // check if user is specifying a custom operator
        if (match(T_LEFT_BRACE)) {
            binify = parser.current.type;  // save the operator
            advance();                     // and eat the operator
            consume(T_RIGHT_BRACE, "Expected '}' after specifying incrementor operator.");
        }
    }

    // if not using default expression, then denote NOW
    if (!check(T_RIGHT_PAREN)) useDefaultInc = false;

    // finally can compile the incrementor operation
    int bodyJump = emitJump(OP_JUMP);
    int incrementStart = currentChunk()->count;

    // update the variable value
    if (useDefaultInc) {  // emit either the default constant
        emitConstant(NUC_NUMBER(1));
    } else {  // or the given expression
        expression();
    }
    forceOperation(binify);  // force the binary operation incrementor

    // and patch some final items for the incrementor
    consume(T_RIGHT_PAREN, "Expected ')' after 'for' clauses.");
    emitLoop(loopStart);
    loopStart = incrementStart;
    patchJump(bodyJump);

    // eat the for loop body
    statement();
    emitLoop(loopStart);

    // patch the exit condition jump
    patchJump(exitJump);
    emitByte(OP_POP);
    fuser_endScope();  // and end the for loop scope
}

#endif