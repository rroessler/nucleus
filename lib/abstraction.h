// NAN Abstraction Header
#include <nan.h>

// Nucleus Headers
extern "C" {
#include "cli/eval.h"
#include "cli/file.h"
}

NAN_METHOD(binding_file) {
    // ensure an argument is given
    if (info.Length() < 1) {
        Nan::ThrowTypeError("Expected a file name.");
        return;
    }

    // and that the argument is a string
    if (!info[0]->IsString()) {
        Nan::ThrowTypeError("Expected a file name as a string.");
        return;
    }

    // get the filename
    v8::String::Utf8Value filename(info.GetIsolate(), info[0]);
    nuc_runFile((const char*)*filename);  // and run the file
}

NAN_METHOD(binding_eval) {
    // ensure an argument is given
    if (info.Length() < 1) {
        Nan::ThrowTypeError("Expected an input to evaluate.");
        return;
    }

    // and that the argument is a string
    if (!info[0]->IsString()) {
        Nan::ThrowTypeError("Expected a string input to evaluate.");
        return;
    }

    // get the input
    v8::String::Utf8Value input(info.GetIsolate(), info[0]);
    nuc_eval((const char*)*input);  // and evaluate
}