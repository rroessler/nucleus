#ifndef NUC_RETURN_STATEMENT_H
#define NUC_RETURN_STATEMENT_H

/** Compiles a return statement. */
static void returnStatement() {
    if (current->type == RT_SCRIPT) error("Cannot return from top-level code.");

    if (match(T_SEMICOLON)) {
        emitReturn();
    } else {
        expression();
        consume(T_SEMICOLON, "Expected ';' after return value.");
        emitByte(OP_RETURN);
    }
}

#endif