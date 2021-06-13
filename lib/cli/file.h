#ifndef NUC_RUN_FILE_H
#define NUC_RUN_FILE_H

// C Standard Library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Nucleus Headers
#include "../vm/atomizer.h"
#include "../vm/disruptions/codes.h"
#include "../vm/disruptions/immediate.h"

/**
 * Reads a file from a given path into memory.
 * @param path              File path to read file of.
 */
char* nuc_readFile(const char* path) {
    FILE* file = fopen(path, "rb");  // open in binary read mode
    if (file == NULL) nuc_immediateExit(NUC_EXIT_IO, "Could not open file \"%s\".", path);

    // determine the file size
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    // read the file into memory
    char* buffer = (char*)malloc(fileSize + 1);

    // ensure buffer was read
    if (buffer == NULL) nuc_immediateExit(NUC_EXIT_MEM, "Not enough memory available to read \"%s\".", path);

    // read the bytes
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);

    // and check for read errors
    if (bytesRead < fileSize) nuc_immediateExit(NUC_EXIT_IO, "Could not read file \"%s\".", path);

    // terminate and return the buffer
    buffer[bytesRead] = '\0';
    fclose(file);
    return buffer;
}

/**
 * Runs a given file in the atomizer.
 * @param path              Path to run.
 */
void nuc_runFile(const char* path) {
    char* source = nuc_readFile(path);
    atomizer_init();
    nuc_atomize(source);
    free(source);  // free the allocated source
}

#endif