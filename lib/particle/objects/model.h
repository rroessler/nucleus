#ifndef NUC_OBJ_MODEL_H
#define NUC_OBJ_MODEL_H

// Nucleus Headers
#include "../table.h"
#include "type.h"

/***********************
 *  MODEL DEFINITIONS  *
 ***********************/

/** Nucleus Model Object Structure */
typedef struct {
    nuc_Obj obj;
    nuc_Table methods;
    nuc_Table defaults;
    nuc_ObjString* name;
} nuc_ObjModel;

/** Nucleus Model Instance Structure */
typedef struct {
    nuc_Obj obj;
    nuc_ObjModel* model;
    nuc_Table fields;
} nuc_ObjInstance;

/** Structure to BIND Model methods when referenced outside of a Model Instance. */
typedef struct {
    nuc_Obj obj;
    nuc_Particle receiver;
    nuc_ObjClosure* method;
} nuc_ObjBoundMethod;

// defines for denoting model stability
#define NUC_MODEL_STABLE true
#define NUC_MODEL_UNSTABLE false

/*******************
 *  MODEL METHODS  *
 *******************/

/**
 * Constructs a new base model object.
 * @param name              Name of model being defined.
 */
nuc_ObjModel* model_new(nuc_ObjString* name) {
    nuc_ObjModel* model = NUC_ALLOC_OBJ(nuc_ObjModel, OBJ_MODEL);
    model->name = name;
    table_init(&model->methods);
    table_init(&model->defaults);
    return model;
}

/**
 * Constructs a new model instance.
 * @param model             Base model to derive from.
 */
nuc_ObjInstance* model_newInstance(nuc_ObjModel* model) {
    nuc_ObjInstance* inst = NUC_ALLOC_OBJ(nuc_ObjInstance, OBJ_INSTANCE);
    inst->model = model;
    table_init(&inst->fields);
    return inst;
}

/**
 * Constructs a new bound model method.
 * @param receiver          Particle receiving reference to bound method.
 * @param method            Model method to bind.
 */
nuc_ObjBoundMethod* model_newBoundMethod(nuc_Particle receiver, nuc_ObjClosure* method) {
    nuc_ObjBoundMethod* bound = NUC_ALLOC_OBJ(nuc_ObjBoundMethod, OBJ_BOUND_METHOD);
    bound->receiver = receiver;
    bound->method = method;
    return bound;
}

#endif