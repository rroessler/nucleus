// C Standard Library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Nucleus Headers
#include "vm/atomizer.h"

/** Read, Evaluate, Process, Loop */
static void repl() {
    char line[1024];
    for (;;) {
        printf("\x1b[3;32mnucleus\x1b[0m -> ");  // display prompt
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        atomize(line);
    }
}

/**
 * Reads a file from a given file path.
 * @param path                  File path to read file of.
 */
static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");  // open the file for reading
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    // determine the file size
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    // read the file into memory
    char* buffer = (char*)malloc(fileSize + 1);

    // ensure the buffer was read fully
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory available to read \"%s\".\n", path);
        exit(74);
    }

    // read the bytes
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {  // check for read failure
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }
    buffer[bytesRead] = '\0';  // and cap off the end

    // close the file and return the contents
    fclose(file);
    return buffer;
}

/**
 * Runs a given file in the atomizer.
 * @param path                  Path of file to run.
 */
static void runFile(const char* path) {
    char* source = readFile(path);
    ATOMIC_STATE state = atomize(source);
    free(source);

    // and return as necessary
    if (state == ASTATE_COMPILE_UNSTABLE) exit(65);
    if (state == ASTATE_RUNTIME_UNSTABLE) exit(70);
}

/** Main Nucleus Program */
int main(int argc, const char* argv[]) {
    atomizer_init();

    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        fprintf(stderr, "Usage: nucleus [path]\n");
        exit(64);
    }

    atomizer_free();
    return 0;
}