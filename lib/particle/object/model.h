#ifndef NUC_MODEL_H
#define NUC_MODEL_H

// Nucleus Headers
#include "../table.h"

/** Nucleus Model Object Structure */
typedef struct {
    Obj obj;
    bool unstable;
    Table methods;
    Table defaults;
    ObjString* name;
} ObjModel;

/** Nucleus Model Instance Structure */
typedef struct {
    Obj obj;
    ObjModel* model;
    Table fields;
} ObjInstance;

/** Structure to BIND Model methods when referenced outside of a Model Instance. */
typedef struct {
    Obj obj;
    Particle receiver;
    ObjClosure* method;
} ObjBoundMethod;

// defines for denoting model stability
#define NUC_MODEL_STABLE false
#define NUC_MODEL_UNSTABLE true

/**
 * Constructs a new base model object.
 * @param name              Name of model being defined.
 * @param unstable          Whether a model's instances can be altered.
 */
ObjModel* model_new(ObjString* name, bool unstable) {
    ObjModel* model = ALLOCATE_OBJ(ObjModel, OBJ_MODEL);
    model->name = name;
    table_init(&model->methods);
    table_init(&model->defaults);
    model->unstable = unstable;
    return model;
}

/**
 * Constructs a new model instance.
 * @param model             Base model to derive from.
 */
ObjInstance* model_newInstance(ObjModel* model) {
    ObjInstance* inst = ALLOCATE_OBJ(ObjInstance, OBJ_INSTANCE);
    inst->model = model;
    inst->obj.mutable = model->unstable;  // and set mutability based on instability of parent
    table_init(&inst->fields);
    return inst;
}

/**
 * Constructs a new bound model method.
 * @param receiver          Particle receiving reference to bound method.
 * @param method            Model method to bind.
 */
ObjBoundMethod* model_newBoundMethod(Particle receiver, ObjClosure* method) {
    ObjBoundMethod* bound = ALLOCATE_OBJ(ObjBoundMethod, OBJ_BOUND_METHOD);
    bound->receiver = receiver;
    bound->method = method;
    return bound;
}

#endif