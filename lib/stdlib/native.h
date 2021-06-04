#ifndef NUC_STDLIB_EXPORT_H
#define NUC_STDLIB_EXPORT_H

// Nucleus Headers
#include "../particle/object/native.h"

/** Standard Library Reaction Structure */
typedef struct {
    const char* name;
    NativeRn native;
} NucleusNativeReaction;

/** Standard Library Property Structure */
typedef struct {
    const char* name;
    Particle value;
} NucleusNativeProperty;

// Nucleus Standard Library
#include "nu-math.h"
#include "print.h"
#include "time.h"

// pre-defined size of stdlib
#define NUC_STDLIB_REACTIONS_LEN 14
#define NUC_STDLIB_PROPERTIES_LEN 0

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
};

/** Pre-defined array of Nucleus Native Properties */
NucleusNativeProperty nuc_nativeProperties[] = {

};

#endif