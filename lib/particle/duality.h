#ifndef NUC_PARTICLE_DUALITY_H
#define NUC_PARTICLE_DUALITY_H

// Nucleus Includes
#include "object.h"
#include "value.h"

/*********************************
 *  API COMMON TO ALL PARTICLES  *
 *********************************/

/** 
 * Coerces a particle as a false value. 
 * @param value             Value to check if falsey
 */
static inline bool particle_isFalsey(Particle value) {
    return IS_NUMBER(value)
               ? (AS_NUMBER(value) == 0)
               : (IS_NULL(value) || (IS_BOOL(value) && !AS_BOOL(value)));
}

/** 
 * Checks if two partciles are equal.
 * @param a                 Particle A.
 * @param b                 Particle B.
 */
bool particle_isEqual(Particle a, Particle b) {
#ifdef NUC_NAN_BOXING
    if (IS_NUMBER(a) && IS_NUMBER(b)) return AS_NUMBER(a) == AS_NUMBER(b);
    return a == b;
#else
    if (a.type != b.type) return false;  // not the same type
    switch (a.type) {
        case TYPE_BOOL:
            return AS_BOOL(a) == AS_BOOL(b);
        case TYPE_NULL:  // always true
            return true;
        case TYPE_NUMBER:
            return AS_NUMBER(a) == AS_NUMBER(b);
        case TYPE_OBJ:
            return AS_OBJ(a) == AS_OBJ(b);
        default:  // unreachable
            return false;
    }
#endif
}

#endif