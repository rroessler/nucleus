#ifndef NUC_DIRECTIVE_DECLARATION_H
#define NUC_DIRECTIVE_DECLARATION_H

/**
 * Coordinates a directive declaration.
 * @param directive             Directive to coordinate.
 */
static void directiveDeclaration(Token* directive) {
    switch (directive->type) {
        case T_MUTATE:
            emitByte(OP_MUTATE);
            declaration();
            break;

        // ignore invalid items
        default:
            return;
    }
}

#endif