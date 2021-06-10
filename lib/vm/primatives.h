#ifndef NUC_PRIMATIVES_DEF_H
#define NUC_PRIMATIVES_DEF_H

#include "../particle/table.h"

/************************
 *  NUCLEUS PRIMATIVES  *
 ************************/

/** Structure to Hold Nucleus Primative Methods */
typedef struct {
    nuc_Table numeric;
    nuc_Table reaction;
    nuc_Table string;
    nuc_Table model;
} nuc_Primatives;

/**
 * Initialises a Primatives structure into memory.
 * @param primatives                    Atomizer primatives to initialise.
 */
static void atomizer_initPrimatives(nuc_Primatives* primatives) {
    table_init(&primatives->model);
    table_init(&primatives->numeric);
    table_init(&primatives->reaction);
    table_init(&primatives->string);
}

/**
 * Fress a Primatives structure from memory.
 * @param primatives                    Atomizer primatives to free.
 */
static void atomizer_freePrimatives(nuc_Primatives* primatives) {
    table_free(&primatives->model);
    table_free(&primatives->numeric);
    table_free(&primatives->reaction);
    table_free(&primatives->string);
}

#endif