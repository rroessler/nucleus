#ifndef NUC_STDLIB_THROW_H
#define NUC_STDLIB_THROW_H

#ifdef NUC_NTVDEF_THROW_DISRUPTION

    // Nucleus Headers
    #include "../../vm/disruptions/disruption.h"
    #include "../helpers.h"

/************************
 *  THROW MAIN HANDLER  *
 ************************/

NUC_NATIVE_WRAPPER(
    std,
    throw,
    NUC_STDLIB_EXPECT_ONE_ARG("std.throw");

    /** If we have a disruption instance, then want to throw that. */
    if (IS_INSTANCE(args[0])) {
        atomizer_thrownDisruption(AS_INSTANCE(args[0]));
        return NUC_NULL;
    }

    /** Otherwise handle as normal. */
    NUC_STDLIB_EXPECT_STRING(args[0], "std.throw");
    if (argCount >= 2) NUC_STDLIB_EXPECT_NUM(args[1], "std.throw");
    atomizer_catchableError(argCount > 1 ? AS_NUMBER(args[1]) : NUC_EXIT_FAILURE, AS_CSTRING(args[0]));

    /** return null as error handler will coordinate catching */
    return NUC_NULL);

    /*************
     *  EXPORTS  *
     *************/

    // exporting the throw methods
    #define NUC_STDLIB__THROW_DISP \
        {"std.throw", nuc_std__throw},

#endif

#endif