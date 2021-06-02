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
#include "print.h"
#include "time.h"

// pre-defined size of stdlib
#define NUC_STDLIB_REACTIONS_LEN 3
#define NUC_STDLIB_PROPERTIES_LEN 0

/** Pre-defined array of Nucleus Native Reactions. */
NucleusNativeReaction nuc_nativeReactions[] = {
    {"std.print", nuc_std__print},

    // time based reactions
    {"std.time.now", nuc_std__time_now},
    {"std.time.clock", nuc_std__time_clock},
};

/** Pre-defined array of Nucleus Native Properties */
NucleusNativeProperty nuc_nativeProperties[] = {

};

#endif