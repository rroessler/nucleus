#ifndef NUC_OBJECT_TYPE
#define NUC_OBJECT_TYPE

// Nucleus Headers
#include "../value.h"

/** Object Type Enumeration */
typedef enum {
    OBJ_CLOSURE,
    OBJ_UPVALUE,
    OBJ_REACTION,

    OBJ_MODEL,
    OBJ_INSTANCE,
    OBJ_BOUND_METHOD,

    OBJ_STRING,
    OBJ_NATIVE,
    OBJ_ARRAY,
} nuc_ObjType;

/** Generic Object Structure */
typedef struct nuc_Obj {
    nuc_ObjType type;      // type of object
    bool isMarked;         // if an object is marked safe from gc
    struct nuc_Obj* next;  // pointer to next object on heap
} nuc_Obj;

/** Nucleus String Object */
typedef struct {
    nuc_Obj obj;
    int length;
    char* chars;
    uint32_t hash;
} nuc_ObjString;  // this one needs to be declared here

/****************************
 *  GENERIC OBJECT HELPERS  *
 ****************************/

/**
 * Checks if a particle have is of a specific internal object type.
 * @param value             Particle to check.
 * @param type              Type to check against.
 */
static inline bool nuc_isObjType(nuc_Particle value, nuc_ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

/** Macro for getting the Object Type */
#define OBJ_TYPE(value) (AS_OBJ(value)->type)

// Object Checks
#define IS_CLOSURE(value) nuc_isObjType(value, OBJ_CLOSURE)
#define IS_MODEL(value) nuc_isObjType(value, OBJ_MODEL)
#define IS_BOUND_METHOD(value) nuc_isObjType(value, OBJ_BOUND_METHOD)
#define IS_INSTANCE(value) nuc_isObjType(value, OBJ_INSTANCE)
#define IS_REACTION(value) nuc_isObjType(value, OBJ_REACTION)
#define IS_NATIVE(value) nuc_isObjType(value, OBJ_NATIVE)
#define IS_STRING(value) nuc_isObjType(value, OBJ_STRING)

// Object Casts
#define AS_CLOSURE(value) ((nuc_ObjClosure*)AS_OBJ(value))
#define AS_MODEL(value) ((nuc_ObjModel*)AS_OBJ(value))
#define AS_BOUND_METHOD(value) ((nuc_ObjBoundMethod*)AS_OBJ(value))
#define AS_INSTANCE(value) ((nuc_ObjInstance*)AS_OBJ(value))
#define AS_REACTION(value) ((nuc_ObjReaction*)AS_OBJ(value))
#define AS_NATIVE(value) (((nuc_ObjNative*)AS_OBJ(value))->reaction)
#define AS_STRING(value) ((nuc_ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((nuc_ObjString*)AS_OBJ(value))->chars)

/**
 * Allocates a Nucleus object to memory.
 * @param type              Enum Type of Nucleus object.
 * @param objType           Object Type for object.
 */
#define NUC_ALLOC_OBJ(type, objType) (type*)obj_alloc(sizeof(type), objType)

#endif