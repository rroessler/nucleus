#ifndef NUC_PARTICLE_PRINT_H
#define NUC_PARTICLE_PRINT_H

// C Standard Library
#include <math.h>
#include <stdio.h>

// Nucleus Headers
#include "object.h"
#include "value.h"

/**
 * Prints a Nucleus Particle Value.
 * @param value                     Value to print.
 * @param prettify                  Prettify output.
 */
void particle_print(nuc_Particle value, bool prettify) {
    if (IS_BOOL(value)) {
        NUC_PRETTIFY_WRAP(35, printf(AS_BOOL(value) ? "true" : "false"));
    } else if (IS_NULL(value)) {
        NUC_PRETTIFY_WRAP(1, printf("null"));
    } else if (IS_NUMBER(value)) {
        NUC_PRETTIFY_WRAP(
            33,
            double num = AS_NUMBER(value);
            if (ceil(num) == num) {
                printf("%lld", (int64_t)num);
            } else {
                printf("%lf", num);
            });
    } else if (IS_OBJ(value)) {
        obj_print(value, prettify);
    }
}

#endif