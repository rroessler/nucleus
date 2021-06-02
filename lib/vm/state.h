#ifndef NUC_STATE_H
#define NUC_STATE_H

/** Atomizer State Enumeration */
typedef enum {
    ASTATE_STABLE,
    ASTATE_COMPILE_UNSTABLE,
    ASTATE_RUNTIME_UNSTABLE,
} ATOMIC_STATE;

#endif