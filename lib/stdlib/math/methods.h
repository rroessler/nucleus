#ifndef NUC_STDLIB_MATH_H
#define NUC_STDLIB_MATH_H

#ifdef NUC_NTVDEF_MATH

    // C Standard Library
    #include <math.h>

    // Nucleus Headers
    #include "../helpers.h"

    /******************
     *  MATH HELPERS  *
     ******************/

    /**
     * Defines a single argument math method.
     * @param METHOD                Native math method.
     */
    #define NUC_MATH_ONE_ARG(METHOD)                \
        NUC_NATIVE_WRAPPER(                         \
            math,                                   \
            METHOD,                                 \
            NUC_STDLIB_EXPECT_ONE_ARG(METHOD);      \
            NUC_STDLIB_EXPECT_NUM(args[0], METHOD); \
            return NUC_NUM(METHOD(AS_NUMBER(args[0]))))

    /**
     * Defines a two argument math methods.
     * @param METHOD                Native math method.
     */
    #define NUC_MATH_TWO_ARGS(METHOD)               \
        NUC_NATIVE_WRAPPER(                         \
            math,                                   \
            METHOD,                                 \
            NUC_STDLIB_EXPECT_ARGS(2, METHOD);      \
            NUC_STDLIB_EXPECT_NUM(args[0], METHOD); \
            NUC_STDLIB_EXPECT_NUM(args[1], METHOD); \
            return NUC_NUM(METHOD(AS_NUMBER(args[0]), AS_NUMBER(args[1]))))

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

/******************
 *  MATH NATIVES  *
 ******************/

// macro abuse to build arguments
NUC_MATH_ONE_ARG(acos)
NUC_MATH_ONE_ARG(acosh)
NUC_MATH_ONE_ARG(asin)
NUC_MATH_ONE_ARG(asinh)
NUC_MATH_ONE_ARG(atan)
NUC_MATH_ONE_ARG(atanh)
NUC_MATH_ONE_ARG(cbrt)
NUC_MATH_ONE_ARG(ceil)
NUC_MATH_ONE_ARG(clz32)
NUC_MATH_ONE_ARG(cos)
NUC_MATH_ONE_ARG(cosh)
NUC_MATH_ONE_ARG(exp)
NUC_MATH_ONE_ARG(expm1)
NUC_MATH_ONE_ARG(floor)
NUC_MATH_ONE_ARG(log)
NUC_MATH_ONE_ARG(log1p)
NUC_MATH_ONE_ARG(log10)
NUC_MATH_ONE_ARG(log2)
NUC_MATH_ONE_ARG(round)
NUC_MATH_ONE_ARG(roundf)
NUC_MATH_ONE_ARG(sin)
NUC_MATH_ONE_ARG(sinh)
NUC_MATH_ONE_ARG(sqrt)
NUC_MATH_ONE_ARG(tan)
NUC_MATH_ONE_ARG(tanh)
NUC_MATH_ONE_ARG(trunc)

NUC_MATH_TWO_ARGS(atan2)
NUC_MATH_TWO_ARGS(pow)

/** Black Sheep Methods */

// Absolute value requires "fabs"
NUC_NATIVE_WRAPPER(
    math,
    abs,
    NUC_STDLIB_EXPECT_ONE_ARG("abs");
    NUC_STDLIB_EXPECT_NUM(args[0], METHOD);
    return NUC_NUM(fabs(AS_NUMBER(args[0]))))

// "hypot" takes in numerous arguments
NUC_NATIVE_WRAPPER(
    math,
    hypot,
    NUC_STDLIB_EXPECT_ALL_NUM("hypot");
    if (argCount == 0) return NUC_NUM(0);
    else if (argCount == 1) return args[0];

    double sos = 0; /** set a placeholder sum of squares */
    for (int i = 0; i < argCount; i++) {
        double tmp = AS_NUMBER(args[i]);
        sos += tmp * tmp;
    }

    /** Return the SQUARE ROOT of the SUM of SQUARES */
    return NUC_NUM(sqrt(sos)))

// need to cast to integers for integer multiplicaltion
NUC_NATIVE_WRAPPER(
    math,
    imul,
    NUC_STDLIB_EXPECT_ARGS(2, "imul");
    NUC_STDLIB_EXPECT_NUM(args[0], "imul");
    NUC_STDLIB_EXPECT_NUM(args[1], "imul");
    return NUC_NUM((int)AS_NUMBER(args[0]) * (int)AS_NUMBER(args[1])))

// coordinates "signum" method which will be -1, 0, or 1
NUC_NATIVE_WRAPPER(
    math,
    signum,
    NUC_STDLIB_EXPECT_ONE_ARG("signum");
    NUC_STDLIB_EXPECT_NUM(args[0], "signum");
    double tmp = AS_NUMBER(args[0]);
    return NUC_NUM((tmp > 0) - (tmp < 0)))

    /*************
     *  EXPORTS  *
     *************/

    // exporting the MATH NATIVE methods
    #define NUC_STDLIB__MATH_NATIVES           \
        {"math.abs", nuc_math__abs},           \
            {"math.acos", nuc_math__acos},     \
            {"math.acosh", nuc_math__acosh},   \
            {"math.asin", nuc_math__asin},     \
            {"math.asinh", nuc_math__asinh},   \
            {"math.atan", nuc_math__atan},     \
            {"math.atanh", nuc_math__atanh},   \
            {"math.atan2", nuc_math__atan2},   \
            {"math.cbrt", nuc_math__cbrt},     \
            {"math.ceil", nuc_math__ceil},     \
            {"math.clz32", nuc_math__clz32},   \
            {"math.cos", nuc_math__cos},       \
            {"math.cosh", nuc_math__cosh},     \
            {"math.exp", nuc_math__exp},       \
            {"math.expm1", nuc_math__expm1},   \
            {"math.floor", nuc_math__floor},   \
            {"math.hypot", nuc_math__hypot},   \
            {"math.imul", nuc_math__imul},     \
            {"math.log1p", nuc_math__log},     \
            {"math.log10", nuc_math__log10},   \
            {"math.log2", nuc_math__log2},     \
            {"math.pow", nuc_math__pow},       \
            {"math.round", nuc_math__round},   \
            {"math.roundf", nuc_math__roundf}, \
            {"math.signum", nuc_math__signum}, \
            {"math.sin", nuc_math__sin},       \
            {"math.sinh", nuc_math__sinh},     \
            {"math.sqrt", nuc_math__sqrt},     \
            {"math.tan", nuc_math__tan},       \
            {"math.tanh", nuc_math__tanh},     \
        { "math.trunc", nuc_math__trunc }

    /***************
     *  UNDEFINES  *
     ***************/

    #undef NUC_MATH_ONE_ARG
    #undef NUC_MATH_TWO_ARGS

#endif

#endif