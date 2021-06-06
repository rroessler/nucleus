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
 * @param prettify                  Prettifies output.
 */
void particle_print(Particle value, bool prettify) {
#ifdef NUC_NAN_BOXING
    if (IS_BOOL(value)) {
        if (prettify) printf("\x1b[35m");
        printf(AS_BOOL(value) ? "true" : "false");
        if (prettify) printf("\x1b[0m");
    } else if (IS_NULL(value)) {
        if (prettify) printf("\x1b[1m");
        printf("\x1b[1mnull\x1b[0m");
        if (prettify) printf("\x1b[0m");
    } else if (IS_NUMBER(value)) {
        double num = AS_NUMBER(value);
        if (prettify) printf("\x1b[33m");
        if (ceil(num) == num) {  // want to print as an integer
            printf("%lld", (int64_t)num);
        } else {  // otherwise a float
            printf("%lf", num);
        }
        if (prettify) printf("\x1b[0m");
    } else if (IS_OBJ(value)) {
        obj_print(value, prettify);
    }
#else
    switch (value.type) {
        case TYPE_BOOL:
            if (prettify) printf("\x1b[35m");
            printf(AS_BOOL(value) ? "true" : "false");
            if (prettify) printf("\x1b[0m");
            break;
        case TYPE_NULL:
            if (prettify) printf("\x1b[1m");
            printf("\x1b[1mnull\x1b[0m");
            if (prettify) printf("\x1b[0m");
            break;
        case TYPE_NUMBER: {
            double num = AS_NUMBER(value);
            if (prettify) printf("\x1b[33m");
            if (ceil(num) == num) {  // want to print as an integer
                printf("%lld", (int64_t)num);
            } else {  // otherwise a float
                printf("%lf", num);
            }
            if (prettify) printf("\x1b[0m");
        } break;
        case TYPE_OBJ:
            obj_print(value, prettify);
            break;
    }
#endif
}

#endif