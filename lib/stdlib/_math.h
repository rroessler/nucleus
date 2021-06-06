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

/*******************
 *  METHOD MACROS  *
 *******************/

#define NUC_MATH_ONE_ARG(METHOD)                                              \
    static inline Particle nuc_math__##METHOD(int argCount, Particle* args) { \
        NUC_MATH_VALIDATE_ONE_ARG(#METHOD);                                   \
        return NUC_NUMBER(METHOD(AS_NUMBER(args[0])));                        \
    }

#define NUC_MATH_TWO_ARGS(METHOD)                                             \
    static inline Particle nuc_math__##METHOD(int argCount, Particle* args) { \
        NUC_MATH_VALIDATE_ARGS(2, #METHOD);                                   \
        return NUC_NUMBER(METHOD(AS_NUMBER(args[0]), AS_NUMBER(args[1])));    \
    }

/**************
 *  MATH API  *
 **************/

/** Black Sheeps */

// Absolute value requires "fabs"
static inline Particle nuc_math__abs(int argCount, Particle* args) {
    NUC_MATH_VALIDATE_ONE_ARG("abs");  // validate the method
    return NUC_NUMBER(fabs(AS_NUMBER(args[0])));
}

// Want to allow dyanmic input for "hypot" method
static inline Particle nuc_math__hypot(int argCount, Particle* args) {
    NUC_MATH_CHECK_NUMERICS(argCount, "hypot");  // check all for numerics
    if (argCount == 0) return 0;                 // default
    if (argCount == 1) return args[0];           // no need to calculate

    double sos = 0;  // set a placeholder sum of squares
    for (int i = 0; i < argCount; i++) {
        double tmp = AS_NUMBER(args[i]);
        sos += tmp * tmp;  // quickest way to complete square
    }
    return NUC_NUMBER(sqrt(sos));  // and return the square root of the sos.
}

// compuetes minimum of dynamic range of inputs
static inline Particle nuc_math__min(int argCount, Particle* args) {
    if (argCount == 0) {
        atomizer_runtimeError("\"min\" expected at least 1 argument.");
        return NUC_NULL;
    }

    // check for valid inputs
    NUC_MATH_CHECK_NUMERICS(argCount, "min");
    double res = INFINITY;
    for (int i = 0; i < argCount; i++) res = fmin(AS_NUMBER(args[i]), res);
    return NUC_NUMBER(res);
}

// compuetes maximum of dynamic range of inputs
static inline Particle nuc_math__max(int argCount, Particle* args) {
    if (argCount == 0) {
        atomizer_runtimeError("\"max\" expected at least 1 argument.");
        return NUC_NULL;
    }

    // check for valid inputs
    NUC_MATH_CHECK_NUMERICS(argCount, "max");
    double res = INFINITY;
    for (int i = 0; i < argCount; i++) res = fmax(AS_NUMBER(args[i]), res);
    return NUC_NUMBER(res);
}

// need to cast to integers for integer multiplicaltion
static inline Particle nuc_math__imul(int argCount, Particle* args) {
    NUC_MATH_VALIDATE_ARGS(2, "imul");  // integer multiplication
    return NUC_NUMBER((int)AS_NUMBER(args[0]) * (int)AS_NUMBER(args[1]));
}

// coordinates "signum" method which will be -1, 0, or 1
static inline Particle nuc_math__signum(int argCount, Particle* args) {
    NUC_MATH_VALIDATE_ONE_ARG("signum");
    double tmp = AS_NUMBER(args[0]);
    return NUC_NUMBER((tmp > 0) - (tmp < 0));
}

/** Macro Abuse (one argument) */
NUC_MATH_ONE_ARG(acos);
NUC_MATH_ONE_ARG(acosh);
NUC_MATH_ONE_ARG(asin);
NUC_MATH_ONE_ARG(asinh);
NUC_MATH_ONE_ARG(atan);
NUC_MATH_ONE_ARG(atanh);
NUC_MATH_ONE_ARG(cbrt);
NUC_MATH_ONE_ARG(ceil);
NUC_MATH_ONE_ARG(clz32);
NUC_MATH_ONE_ARG(cos);
NUC_MATH_ONE_ARG(cosh);
NUC_MATH_ONE_ARG(exp);
NUC_MATH_ONE_ARG(expm1);
NUC_MATH_ONE_ARG(floor);
NUC_MATH_ONE_ARG(log);
NUC_MATH_ONE_ARG(log1p);
NUC_MATH_ONE_ARG(log10);
NUC_MATH_ONE_ARG(log2);
NUC_MATH_ONE_ARG(round);
NUC_MATH_ONE_ARG(roundf);
NUC_MATH_ONE_ARG(sin);
NUC_MATH_ONE_ARG(sinh);
NUC_MATH_ONE_ARG(sqrt);
NUC_MATH_ONE_ARG(tan);
NUC_MATH_ONE_ARG(tanh);
NUC_MATH_ONE_ARG(trunc);

/** Macro Abuse (two arguments) */
NUC_MATH_TWO_ARGS(atan2);
NUC_MATH_TWO_ARGS(pow);

#endif