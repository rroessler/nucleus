#ifndef NUC_QUANTISE_OPERATORS_H
#define NUC_QUANTISE_OPERATORS_H

// buffer length for concatenating numbers and strings
#define NUC_CONCAT_BUFF_LEN 50
#define NUC_REPEAT_SN true
#define NUC_REPEAT_NS false

/**
 * Casts a CONCATENATION argument to desired `nuc_ObjString` format.
 * @param dest                  Destination of `nuc_ObjString`.
 * @param argNum                Stack argument number.
 */
#define NUC_CONCAT_CAST(dest, argNum)                                                             \
    if (IS_NUMBER(PEEK(argNum))) {                                                                \
        char buffer[NUC_CONCAT_BUFF_LEN];     /** buffer for result */                            \
        double num = AS_NUMBER(PEEK(argNum)); /** get value as double */                          \
                                                                                                  \
        /** convert to string as required */                                                      \
        if (ceil(num) == num) {                                                                   \
            snprintf(buffer, NUC_CONCAT_BUFF_LEN, "%lld", (int64_t)num);                          \
        } else {                                                                                  \
            snprintf(buffer, NUC_CONCAT_BUFF_LEN, "%lf", num);                                    \
        }                                                                                         \
                                                                                                  \
        /** save copy to destination */                                                           \
        dest = objString_copy(buffer, (int)strlen(buffer));                                       \
    } else if (IS_STRING(PEEK(argNum))) {                                                         \
        dest = AS_STRING(PEEK(argNum));                                                           \
    } else { /** Otherwise results in a type error */                                             \
        atomizer_catchableError(NUC_EXIT_TYPE, "Only strings and numerics can be concatenated."); \
        return false;                                                                             \
    }

/** Concatenates two strings, or a string and number together. */
static inline bool quantise_concat() {
    nuc_ObjString* b;
    nuc_ObjString* a;  // set placeholders

    // allocate the above strings
    NUC_CONCAT_CAST(b, 0);
    NUC_CONCAT_CAST(a, 1);

    // and now can continue with concatenation
    int length = a->length + b->length;
    char* chars = NUC_ALLOC(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    // and PUSH the result back onto the stack
    nuc_ObjString* res = objString_take(chars, length);
    POP_DOUBLE();  // pop the args
    PUSH(NUC_OBJ(res));
    return true;  // denote success
}

/**
 * Helper method for `quantise_repeat` to optimize what would have
 * been iterated "strcat" methods.
 * @param s                     String to concat.
 * @param a                     Item to concatenate.
 */
static inline char* quatise_repeat__strcat(char* s, char* a) {
    while (*s != '\0') s++;
    while (*a != '\0') *s++ = *a++;
    *s = '\0';
    return s;
}

/**
 * Repeats a string by a certain number of times.
 * @param dir                   Direction of either NS or SN
 */
static inline void quantise_repeat(bool dir) {
    int count = (int)AS_NUMBER(PEEK(dir ? 1 : 0));
    nuc_ObjString* string = AS_STRING(PEEK(dir ? 0 : 1));

    // handle negative / zero counts
    if (count <= 0) {
        POP_DOUBLE();
        PUSH(NUC_OBJ(objString_copy("", 0)));
        return;
    } else if (count == 1) {  // if repeats once, then return original
        POP_DOUBLE();
        PUSH(NUC_OBJ(string));
        return;
    }

    // otherwise want to coordinate repeating the string
    int length = (string->length) * count + count;
    char* chars = NUC_ALLOC(char, length);
    *chars = '\0';
    char* tmp = quatise_repeat__strcat(chars, string->chars);  // need to sub 1 below, since preempt ONCE
    for (int i = 0; i < count - 1; i++) tmp = quatise_repeat__strcat(tmp, string->chars);

    // and now POP and PUSH the new result
    nuc_ObjString* res = objString_take(chars, length);
    POP_DOUBLE();
    PUSH(NUC_OBJ(res));
}

/********************************
 *  QUANTISER PARTICLE METHODS  *
 ********************************/

/**
 * Checks if a given particle is "falsey".
 * @param value             Particle to check.
 */
static inline bool quantise_isFalsey(nuc_Particle value) {
    return IS_NUMBER(value)
               ? (AS_NUMBER(value) == 0)
               : (IS_OBJ(value)
                      ? obj_isEmpty(AS_OBJ(value))
                      : (IS_NULL(value) ||
                         (IS_BOOL(value) && !AS_BOOL(value))));
}

/**
 * Checks if two particles are the the same either by value (numbers) or
 * by given pointer locations.
 * @param a                             Particle A to compare.
 * @param b                             Particle B to compare.
 */
static inline bool quantise_isEqual(nuc_Particle a, nuc_Particle b) {
    if (IS_NUMBER(a) && IS_NUMBER(b)) return AS_NUMBER(a) == AS_NUMBER(b);
    return a == b;  // since NAN boxing, can check pointers
}

/**
 * Checks if a particle is less than another particle.
 * @param a                             Particle A to compare.
 * @param b                             Particle B to compare.
 */
static inline bool quantise_isLess(nuc_Particle a, nuc_Particle b) {
    if (IS_NUMBER(a) && IS_NUMBER(b)) {
        return AS_NUMBER(a) < AS_NUMBER(b);
    } else if (IS_STRING(a) && IS_STRING(b)) {
        return strcmp(AS_STRING(a)->chars, AS_STRING(b)->chars) < 0;
    }

    // otherwise will result in a catchable runtime error
    atomizer_catchableError(NUC_EXIT_TYPE, "Cannot compare operands with pointer comparisons if both are not of type string or numeric.");
    return false;
}

#endif