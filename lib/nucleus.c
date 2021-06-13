// C Standard Library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Nucleus Headers
#include "cli/file.h"
#include "cli/repl.h"
#include "vm/disruptions/codes.h"
#include "vm/disruptions/immediate.h"

/** Entry Point for Nucleus Programs */
int main(int argc, const char* argv[]) {
    if (argc == 1) {
        nuc_repl();
    } else if (argc == 2) {
        nuc_runFile(argv[1]);
    } else {
        nuc_immediateExit(NUC_EXIT_CMD, "Unknown command received.\n\x1b[33mUsage:\x1b[0m nucleus [path]\n");
    }

    return NUC_EXIT_SUCCESS;
}