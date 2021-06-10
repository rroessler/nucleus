#ifndef NUC_CALL_FRAME_H
#define NUC_CALL_FRAME_H

// Nucleus Headers
#include "../../particle/objects/closure.h"

/** Atomizer Call Frame Structure */
typedef struct {
    nuc_ObjClosure* closure;  // pointer to associated closure reaction
    uint8_t* ip;              // frame instruction pointer
    nuc_Particle* slots;      // internal local slots
} nuc_CallFrame;

#endif