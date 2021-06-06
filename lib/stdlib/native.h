#ifndef NUC_STDLIB_EXPORT_H
#define NUC_STDLIB_EXPORT_H

// Nucleus Headers
#include "../particle/object/native.h"

/** Standard Library Reaction Structure */
typedef struct {
    const char* name;
    NativeRn native;
} NucleusNativeReaction;

// enumeration for defining property constants
typedef enum {
    NATIVE_NUM,
    NATIVE_STR,
    NATIVE_BOOL
} PropertyType;

/** Standard Library Property Structure */
typedef struct {
    const char* name;
    PropertyType type;
    union {
        double num;
        bool boolean;
        const char* str;
    } as;
} NucleusNativeProperty;

// Nucleus Standard Library
#include "_math.h"
#include "print.h"
#include "rand.h"
#include "time.h"

// pre-defined size of stdlib
#define NUC_STDLIB_REACTIONS_LEN 35
#define NUC_STDLIB_PROPERTIES_LEN 10

/** Pre-defined array of Nucleus Native Reactions. */
NucleusNativeReaction nuc_nativeReactions[] = {
    {"std.print", nuc_std__print},

    // time based reactions
    {"std.time.now", nuc_std__time_now},
    {"std.time.clock", nuc_std__time_clock},

    // math reactions
    {"math.abs", nuc_math__abs},
    {"math.acos", nuc_math__acos},
    {"math.acosh", nuc_math__acosh},
    {"math.asin", nuc_math__asin},
    {"math.asinh", nuc_math__asinh},
    {"math.atan", nuc_math__atan},
    {"math.atanh", nuc_math__atanh},
    {"math.atan2", nuc_math__atan2},
    {"math.cbrt", nuc_math__cbrt},
    {"math.ceil", nuc_math__ceil},
    {"math.clz32", nuc_math__clz32},
    {"math.cos", nuc_math__cos},
    {"math.cosh", nuc_math__cosh},
    {"math.exp", nuc_math__exp},
    {"math.expm1", nuc_math__expm1},
    {"math.hypot", nuc_math__hypot},
    {"math.floor", nuc_math__floor},
    {"math.log1p", nuc_math__log},
    {"math.log10", nuc_math__log10},
    {"math.log2", nuc_math__log2},
    {"math.pow", nuc_math__pow},
    {"math.round", nuc_math__round},
    {"math.roundf", nuc_math__roundf},
    {"math.sin", nuc_math__sin},
    {"math.sinh", nuc_math__sinh},
    {"math.sqrt", nuc_math__sqrt},
    {"math.tan", nuc_math__tan},
    {"math.tanh", nuc_math__tanh},
    {"math.trunc", nuc_math__trunc},

    // random reactions
    {"std.rand.seed", nuc_rand__seed},
    {"std.rand.float", nuc_rand__float},
    {"std.rand", nuc_rand},
};

#define NUC_NATIVE_PROPERTY(a, b, c) ((NucleusNativeProperty){a, b, c})

/** Pre-defined array of Nucleus Native Properties */
NucleusNativeProperty nuc_nativeProperties[] = {
    NUC_NATIVE_PROPERTY("math.E", NATIVE_NUM, {.num = M_E}),
    NUC_NATIVE_PROPERTY("math.LN_2", NATIVE_NUM, {.num = M_LN2}),
    NUC_NATIVE_PROPERTY("math.LN_10", NATIVE_NUM, {.num = M_LN10}),
    NUC_NATIVE_PROPERTY("math.LOG_2E", NATIVE_NUM, {.num = M_LOG2E}),
    NUC_NATIVE_PROPERTY("math.LOG_10E", NATIVE_NUM, {.num = M_LOG10E}),
    NUC_NATIVE_PROPERTY("math.PI", NATIVE_NUM, {.num = M_PI}),
    NUC_NATIVE_PROPERTY("math.PI_2", NATIVE_NUM, {.num = M_PI_2}),
    NUC_NATIVE_PROPERTY("math.PI_4", NATIVE_NUM, {.num = M_PI_4}),
    NUC_NATIVE_PROPERTY("math.SQRT_2", NATIVE_NUM, {.num = M_SQRT2}),
    NUC_NATIVE_PROPERTY("math.SQRT1_2", NATIVE_NUM, {.num = M_SQRT1_2}),
};

#endif