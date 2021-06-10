#ifndef NUC_COMPILER_FLAGS_H
#define NUC_COMPILER_FLAGS_H

// Nucleus Headers
#include "../../common.h"

/*********************
 *  AVAILABLE FLAGS  *
 *********************/

#define NUC_CFLAGS_NONE (uint32_t)0               // no flags set
#define NUC_CFLAG_MUTATE_NEXT (uint32_t)(1 << 0)  // denotes the next variable as mutating

/******************
 *  FLAG METHODS  *
 ******************/

#define NUC_SET_CFLAG(flag) current->flags |= (flag)
#define NUC_UNSET_CFLAG(flag) current->flags &= ~(flag)
#define NUC_RESET_CFLAGS fuser->flags = NUC_CFLAGS_NONE
#define NUC_CHECK_CFLAG(flag) ((current->flags & (flag)) != 0)

#endif