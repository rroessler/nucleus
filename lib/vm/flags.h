#ifndef NUC_ATOMIZER_FLAGS_H
#define NUC_ATOMIZER_FLAGS_H

// Nucleus Headers
#include "../common.h"

// defining some global bit flags for the atomizer
#define NUC_AFLAG_RESET (uint32_t)0  // resets all flags

#define NUC_AFLAG_NEXT_SET_MUTABLE (uint32_t)(1 << 3)  // denotes to make next set mutable

#endif