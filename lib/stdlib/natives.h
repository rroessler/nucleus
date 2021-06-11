#ifndef NUC_STDLIB_NATIVES_H
#define NUC_STDLIB_NATIVES_H

#include "../particle/objects/reaction.h"

/*******************
 *  NATIVE GUARDS  *
 *******************/

// Here certain DEFINES can be specified to ALTER the available standard library. The reasoning behind this
// is that if Nucleus is to be potentially compilable for smaller embedded devices, then a restriction of
// natives will allow for minimizing application size. Specifically if many natives are unused, then there is no
// need to have them included. SOME ITEMS HOWEVER WILL BE RESERVED. This is because debug printing, etc... is expected
// to be needed on most items. In these cases, I'm sure if someone REALLY desired to remove them they can do so.
//
// On top of this, there are also native restrictions, which will further specify "MINIMUM" builds. An example of
// this is removing more specialized "math" natives.

#define NUC_NTVDEF_TIME
#define NUC_NTVDEF_MATH

#define NUC_NTVDEF_DISRUPTIONS
#define NUC_NTVDEF_THROW_DISRUPTION

// defines for available reference array sizes
#define NUC_NATIVE_REACTIONS_LEN 34  // THESE MUST BE CORRECT
#define NUC_NATIVE_PROPS_LEN 0       // VALUES OR ELSE ITEMS MAY BE MISSED

/**********************
 *  TYPE DEFINITIONS  *
 **********************/

/** Structure for Native Reaction References */
typedef struct {
    const char* name;
    nuc_NativeReaction native;
} nuc_NativeReactionReference;

/*********************
 *  LIBRARY HEADERS  *
 *********************/

#include "disruption/throw.h"
#include "math/constants.h"
#include "math/methods.h"
#include "print.h"
#include "time/time.h"

/**********************
 *  NATTIE REACTIONS  *
 **********************/

nuc_NativeReactionReference nuc_nativeReactionRefs[] = {
    // miscellaneous
    {"std.print", nuc_std__print},

#ifdef NUC_NTVDEF_TIME  // time natives
    NUC_STDLIB__TIME_NATIVES,
#endif

#ifdef NUC_NTVDEF_MATH  // math natives
    NUC_STDLIB__MATH_NATIVES,
#endif

#ifdef NUC_NTVDEF_DISRUPTIONS           // disruption methods
    #ifdef NUC_NTVDEF_THROW_DISRUPTION  // throw methods
    NUC_STDLIB__THROW_DISP
    #endif
#endif
};

#endif