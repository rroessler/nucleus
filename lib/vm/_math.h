#ifndef NUC_OPERATOR_MATH_H
#define NUC_OPERATOR_MATH_H

/** Concatenates two particles on the top of the atomizer stack. */
static void atomizer_concat() {
    ObjString* b = AS_STRING(atomizer_peek(0));
    ObjString* a = AS_STRING(atomizer_peek(1));

    // concatenate both strings
    int length = a->length + b->length;
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    // and push the result to the stack
    ObjString* result = objString_take(chars, length);
    POP();
    POP();
    PUSH(NUC_OBJ(result));
}

#endif