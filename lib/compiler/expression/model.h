#ifndef NUC_MODEL_DECLARATION_H
#define NUC_MODEL_DECLARATION_H

// forward declaraiton
static void reaction(ReactionType type);

/** Parses / Compiles Model Method */
static void method() {
    uint8_t constant = identifierConstant(&parser.previous);

    // parse the item as a reaction
    ReactionType type = RT_METHOD;
    if (parser.previous.length == 10 && memcmp(parser.previous.start, "@construct", 10) == 0) type = RT_INITIALISER;
    reaction(type);

    // and emit as a method
    EMIT_SHORT(OP_METHOD, constant);
}

/** Parses / Compiles a Model Field */
static void field() {
    uint8_t constant = identifierConstant(&parser.previous);
    advance();     // eat the colon
    expression();  // eat the expression
    consume(T_SEMICOLON, "Expected ';' after field declaration.");

    // and emit as a field
    EMIT_SHORT(OP_FIELD, constant);
}

/** Parses and Compiles a Model Declaration */
static void modelDeclaration() {
    consume(T_IDENTIFIER, "Expected a model name.");
    Token modelName = parser.previous;
    uint8_t nameConstant = identifierConstant(&modelName);
    declareVariable();

    // now want to emit the model bytes
    EMIT_SHORT(OP_MODEL, nameConstant);
    defineVariable(nameConstant, true);  // and wan to name to be immutable

    // and set as a new model fuser
    ModelFuser modelFuser;
    modelFuser.enclosing = currentModel;
    currentModel = &modelFuser;

    // bring the class to the top of the stack
    namedVariable(modelName, false, true);

    // now need to consume the model body
    consume(T_LEFT_BRACE, "Expected '{' before model body.");
    while (!check(T_RIGHT_BRACE) && !check(T_EOF)) {
        consume(T_IDENTIFIER, "Expecting a method/field name.");
        switch (parser.current.type) {
            case T_LEFT_PAREN:  // method declarations
                method();
                break;

            case T_COLON:  // field declarations
                field();
                break;

            default:  // bad token
                errorAtCurrent("Unknown symbol found in model declaration.");
                break;
        }
    }

    // and cap off the end of the model declaration
    consume(T_RIGHT_BRACE, "Expected '}' after model body.");
    consume(T_SEMICOLON, "Expected ';' after model declaration.");
    emitByte(OP_POP);  // and pop class off the stack

    // and remove top reference of model fuser
    currentModel = currentModel->enclosing;
}

#endif