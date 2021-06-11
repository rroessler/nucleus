#ifndef NUC_QUANTISE_MEMBER_H
#define NUC_QUANTISE_MEMBER_H

/**
 * Accesses an array member using a given accessor and parent array.
 * @param accessor              Array member accessor.
 * @param arr                   Array to access item from.
 */
static inline bool quantise_getArrayMember(nuc_Particle accessor, nuc_ObjArr* arr) {
    if (!IS_NUMBER(accessor)) {  // ensure only numerics can access array members
        atomizer_catchableError(NUC_EXIT_TYPE, "Only numerics can be used to access array members.");
        return false;
    }

    // cast the numeric to a double and check if NOT an integer
    double index = AS_NUMBER(accessor);
    if (ceil(index) != index) {
        atomizer_catchableError(NUC_EXIT_RANGE, "A floating point numeric was used to access an array member.");
        return false;
    }

    // pop the array and check range access
    if (index < 0 || (int)index > arr->count - 1) {
        atomizer_catchableError(NUC_EXIT_RANGE, "Array accessor out of feasible range.");
        return false;
    }

    // NOW that all the checks are complete, push onto the stack the item
    PUSH(arr->values[(int)accessor]);
    return true;  // denote success
}

/**
 * Accesses an model member using a given accessor and parent model.
 * @param accessor              Model member accessor.
 * @param instance              Model instance to access item from.
 */
static inline bool quantise_getModelMember(nuc_Particle accessor, nuc_ObjInstance* instance) {
    if (!IS_STRING(accessor)) {  // make sure have a string based accessor
        atomizer_catchableError(NUC_EXIT_REF, "Only strings can be used to access model members.");
        return false;
    }

    // can now simply get the accessor name
    nuc_ObjString* name = AS_STRING(accessor);
    nuc_Particle value;  // item being accessed

    // and attempt to access
    if (table_get(&instance->model->defaults, name, &value)) {
        PUSH(value);
        return true;
    } else if (table_get(&instance->fields, name, &value)) {
        PUSH(value);
        return true;
    }

    // otherwise is a bad access
    atomizer_catchableError(NUC_EXIT_REF, "Could not access undefined model member.");
    return false;
}

/**
 * Sets an array member with given value and accessor.
 * @param value                     Value to assign.
 * @param accessor                  Accessor to use.
 * @param arr                       Array to assign to.
 */
static inline bool quantise_setArrayMember(nuc_Particle value, nuc_Particle accessor, nuc_ObjArr* arr) {
    if (!IS_NUMBER(accessor)) {  // ensure only numerics can access array members
        atomizer_catchableError(NUC_EXIT_TYPE, "Only numerics can be used to access array members.");
        return false;
    }

    // cast the numeric to a double and check if NOT an integer
    double index = AS_NUMBER(accessor);
    if (ceil(index) != index) {
        atomizer_catchableError(NUC_EXIT_RANGE, "A floating point numeric was used to access an array member.");
        return false;
    }

    // pop the array and check range access
    if (index < 0 || (int)index > arr->count - 1) {
        atomizer_catchableError(NUC_EXIT_RANGE, "Array accessor out of feasible range.");
        return false;
    }

    arr->values[(int)accessor] = value;
    PUSH(NUC_OBJ(arr));
    return true;
}

/**
 * Sets an model member with given value and accessor.
 * @param value                     Value to assign.
 * @param accessor                  Accessor to use.
 * @param instance                  Model instance to assign to.
 */
static inline bool quantise_setModelMember(nuc_Particle value, nuc_Particle accessor, nuc_ObjInstance* instance) {
    if (!IS_STRING(accessor)) {  // make sure have a string based accessor
        atomizer_catchableError(NUC_EXIT_REF, "Only strings can be used to access model members.");
        return false;
    }

    // can now simply get the accessor name
    nuc_ObjString* name = AS_STRING(accessor);
    if (!table_set(&instance->model->defaults, name, value)) {
        table_set(&instance->fields, name, value);
    }

    // and push the value back onto the stack
    PUSH(NUC_OBJ(instance));
    return true;
}

#endif