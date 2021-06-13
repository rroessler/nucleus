#include <nan.h>

// Nucleus Abstraction methods
#include "lib/abstraction.h"

NAN_MODULE_INIT(Initialise) {
    NAN_EXPORT(target, binding_file);
    NAN_EXPORT(target, binding_eval);
}

NODE_MODULE(binding, Initialise);