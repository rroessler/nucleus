#ifndef NUC_STDLIB_HELPERS_H
#define NUC_STDLIB_HELPERS_H

// Nucleus Headers
#include "../particle/object.h"
#include "../particle/value.h"

/******************************
 *  NATIVE FUNCTION WRAPPING  *
 ******************************/

/**
 * Wraps native method code to allow for not CODING out the same method handle
 * continually.
 * @param METHOD                Method name.
 */
#define NUC_NATIVE_WRAPPER(PARENT, METHOD, ...)                                             \
    static inline nuc_Particle nuc_##PARENT##__##METHOD(int argCount, nuc_Particle* args) { \
        __VA_ARGS__;                                                                        \
    }

/**********************
 *  EXPECTING MACROS  *
 **********************/

/**
 * Checks if the argument count is valid.
 * @param EXPECTED          Number of arguments expected.
 * @param METHOD            Method being completed.
 */
#define NUC_STDLIB_EXPECT_ARGS(EXPECTED, METHOD)                                            \
    if (argCount < EXPECTED) {                                                              \
        atomizer_catchableError(NUC_EXIT_ARG, #METHOD " expected " #EXPECTED " argumets."); \
        return NUC_NULL;                                                                    \
    }

/**
 * Checks for only 1 argument.
 * @param METHOD            Method being completed.
 */
#define NUC_STDLIB_EXPECT_ONE_ARG(METHOD)                                                \
    if (argCount < 1) {                                                                  \
        atomizer_catchableError(NUC_EXIT_ARG, #METHOD " expected at least 1 argument."); \
        return NUC_NULL;                                                                 \
    }

/**
 * Checks for only 0 arguments.
 * @param METHOD            Method being completed.
 */
#define NUC_STDLIB_EXPECT_NO_ARGS(METHOD)                                         \
    if (argCount != 0) {                                                          \
        atomizer_catchableError(NUC_EXIT_ARG, #METHOD " expected no arguments."); \
        return NUC_NULL;                                                          \
    }

/**
 * Expects a type for a particularly typed argument.
 * @param arg               Argument to type check.
 * @param isMethod          Check method.
 * @param TYPE              TYPE of argument.
 * @param METHOD            Method in which expectation occurs.
 */
#define NUC_STDLIB_EXPECT_TYPE(arg, isMethod, TYPE, METHOD)                                         \
    if (!isMethod(arg)) {                                                                           \
        atomizer_catchableError(NUC_EXIT_TYPE, #METHOD " expected a \"" #TYPE "\" argument type."); \
        return NUC_NULL;                                                                            \
    }

/**
 * Expects a type for an object typed argument.
 * @param arg               Argument to type check.
 * @param isMethod          Check method.
 * @param TYPE              TYPE of argument.
 * @param METHOD            Method in which expectation occurs.
 */
#define NUC_STDLIB_EXPECT_OBJ_TYPE(arg, isMethod, TYPE, METHOD)                                     \
    if (!IS_OBJ(arg) || !isMethod(arg)) {                                                           \
        atomizer_catchableError(NUC_EXIT_TYPE, #METHOD " expected a \"" #TYPE "\" argument type."); \
        return NUC_NULL;                                                                            \
    }

/** TYPE CHECKING MACROS */

#define NUC_STDLIB_EXPECT_NUM(arg, METHOD) NUC_STDLIB_EXPECT_TYPE(arg, IS_NUMBER, numeric, METHOD)
#define NUC_STDLIB_EXPECT_BOOL(arg, METHOD) NUC_STDLIB_EXPECT_TYPE(arg, IS_BOOL, boolean, METHOD)
#define NUC_STDLIB_EXPECT_NULL(arg, METHOD) NUC_STDLIB_EXPECT_TYPE(arg, IS_NULL, null, METHOD)
#define NUC_STDLIB_EXPECT_OBJ(arg, METHOD) NUC_STDLIB_EXPECT_TYPE(arg, IS_OBJ, model, METHOD)
#define NUC_STDLIB_EXPECT_STRING(arg, METHOD) NUC_STDLIB_EXPECT_OBJ_TYPE(arg, IS_STRING, string, METHOD)
#define NUC_STDLIB_EXPECT_REACTION(arg, METHOD) NUC_STDLIB_EXPECT_OBJ_TYPE(arg, IS_REACTION, reaction, METHOD)
#define NUC_STDLIB_EXPECT_MODEL(arg, METHOD) NUC_STDLIB_EXPECT_OBJ_TYPE(arg, IS_MODEL, model, METHOD)

/** ITERATING TYPE CHECKS */

#define NUC_STDLIB_EXPECT_ALL_NUM(METHOD) \
    for (int i = 0; i < argCount; i++) NUC_STDLIB_EXPECT_NUM(args[i], METHOD)

#define NUC_STDLIB_EXPECT_ALL_STRINGS(METHOD) \
    for (int i = 0; i < argCount; i++) NUC_STDLIB_EXPECT_STRING(args[i], METHOD)

#endif