#ifndef NUC_STDLIB_MATH_H
#define NUC_STDLIB_MATH_H

// C Standard Library
#include <math.h>

// Nucleus Headers
#include "../particle/value.h"

// forward declaration
static void atomizer_runtimeError(const char* format, ...);

/************************
 *  MATH HELPER MACROS  *
 ************************/

// macro for confirming correct number of argument
#define NUC_MATH_EXPECT_ARGS(expected, METHOD)                               \
    if (argCount < expected) {                                               \
        atomizer_runtimeError(#METHOD " expected " #expected " arguments."); \
        return NUC_NULL;                                                     \
    }

// macro for individually arguments
#define NUC_MATH_EXPECT_ONE_ARG(METHOD)                         \
    if (argCount < 1) {                                         \
        atomizer_runtimeError(#METHOD " expected 1 argument."); \
        return NUC_NULL;                                        \
    }

// macro for confirming an item is a numeric
#define NUC_MATH_EXPECT_NUMERIC(arg, METHOD)                            \
    if (!IS_NUMBER(arg)) {                                              \
        atomizer_runtimeError(#METHOD " expected a numeric argument."); \
        return NUC_NULL;                                                \
    }

// macro for checking a set number of items as numerics
#define NUC_MATH_CHECK_NUMERICS(numArgs, METHOD) \
    for (int i = 0; i < numArgs; i++) NUC_MATH_EXPECT_NUMERIC(args[i], METHOD)

// validates singular arguments
#define NUC_MATH_VALIDATE_ONE_ARG(METHOD) \
    NUC_MATH_EXPECT_ONE_ARG(METHOD);      \
    NUC_MATH_EXPECT_NUMERIC(args[0], METHOD)

// validates multiple arguments
#define NUC_MATH_VALIDATE_ARGS(expected, METHOD) \
    NUC_MATH_EXPECT_ARGS(expected, METHOD);      \
    NUC_MATH_CHECK_NUMERICS(expected, METHOD)

/**
 * Custom __builtin_clz method.
 * @param num           Number to compute. 
 */
static double clz32(double num) {
    // handle some edge cases
    if (num < 0) {
        return 0;
    } else if (num == 0) {
        return 32;
    }

    // cast double to uint32_t
    uint32_t x = (uint32_t)num;

    // and compute clz32
    static const char debruijn32[32] = {
        0, 31, 9, 30, 3, 8, 13, 29, 2, 5, 7, 21, 12, 24, 28, 19,
        1, 10, 4, 14, 6, 22, 25, 20, 11, 15, 23, 26, 16, 27, 17, 18};
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return debruijn32[x * 0x076be629 >> 27];
}

/**************
 *  MATH API  *
 **************/

/** Conducts the "abs" math operation. */
static inline Particle nuc_math__abs(int argCount, Particle* args) {
    NUC_MATH_VALIDATE_ONE_ARG("abs");  // validate the method
    return NUC_NUMBER(fabs(AS_NUMBER(args[0])));
}

/** Conducts the "acos" math operation. */
static inline Particle nuc_math__acos(int argCount, Particle* args) {
    NUC_MATH_VALIDATE_ONE_ARG("acos");  // validate the method
    return NUC_NUMBER(acos(AS_NUMBER(args[0])));
}

/** Conducts the "acosh" math operation. */
static inline Particle nuc_math__acosh(int argCount, Particle* args) {
    NUC_MATH_VALIDATE_ONE_ARG("acosh");  // validate the method
    return NUC_NUMBER(acosh(AS_NUMBER(args[0])));
}

/** Conducts the "asin" math operation. */
static inline Particle nuc_math__asin(int argCount, Particle* args) {
    NUC_MATH_VALIDATE_ONE_ARG("asin");  // validate the method
    return NUC_NUMBER(asin(AS_NUMBER(args[0])));
}

/** Conducts the "asinh" math operation. */
static inline Particle nuc_math__asinh(int argCount, Particle* args) {
    NUC_MATH_VALIDATE_ONE_ARG("asinh");  // validate the method
    return NUC_NUMBER(asinh(AS_NUMBER(args[0])));
}

/** Conducts the "atan" math operation. */
static inline Particle nuc_math__atan(int argCount, Particle* args) {
    NUC_MATH_VALIDATE_ONE_ARG("atan");  // validate the method
    return NUC_NUMBER(atan(AS_NUMBER(args[0])));
}

/** Conducts the "atanh" math operation. */
static inline Particle nuc_math__atanh(int argCount, Particle* args) {
    NUC_MATH_VALIDATE_ONE_ARG("atanh");  // validate the method
    return NUC_NUMBER(atanh(AS_NUMBER(args[0])));
}

/** Conducts the "atan2" math operation. */
static inline Particle nuc_math__atan2(int argCount, Particle* args) {
    NUC_MATH_VALIDATE_ARGS(2, "atanh");
    return NUC_NUMBER(atan2(AS_NUMBER(args[0]), AS_NUMBER(args[1])));
}

/** Conducts the "cbrt" math operation. */
static inline Particle nuc_math__cbrt(int argCount, Particle* args) {
    NUC_MATH_VALIDATE_ONE_ARG("cbrt");
    return NUC_NUMBER(cbrt(AS_NUMBER(args[0])));
}

/** Conducts the "ceil" math operation. */
static inline Particle nuc_math__ceil(int argCount, Particle* args) {
    NUC_MATH_VALIDATE_ONE_ARG("ceil");
    return NUC_NUMBER(ceil(AS_NUMBER(args[0])));
}

/** Conducts the "clz32" math operation. */
static inline Particle nuc_math__clz32(int argCount, Particle* args) {
    NUC_MATH_VALIDATE_ONE_ARG("clz32");
    return NUC_NUMBER(clz32(AS_NUMBER(args[0])));
}

#endif