#ifndef NUC_ATOMIZER_FLAGS_H
#define NUC_ATOMIZER_FLAGS_H

// Nucleus Headers
#include "../../common.h"

/*********************
 *  AVAILABLE FLAGS  *
 *********************/

#define NUC_AFLAGS_NONE (uint32_t)0                        // no flags set
#define NUC_AFLAG_DISRUPTION_CATCHABLE (uint32_t)(1 << 0)  // denotes to catch errors
#define NUC_AFLAG_DISRUPTED (uint32_t)(1 << 1)             // denotes a disruption occured without being caught

/******************
 *  FLAG METHODS  *
 ******************/

#define NUC_SET_AFLAG(flag) atomizer.flags |= (flag)
#define NUC_UNSET_AFLAG(flag) atomizer.flags &= ~(flag)
#define NUC_RESET_AFLAGS atomizer.flags = NUC_AFLAGS_NONE
#define NUC_CHECK_AFLAG(flag) ((atomizer.flags & (flag)) != 0)

#endif