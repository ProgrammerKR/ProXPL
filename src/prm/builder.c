// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-23
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prm.h"

// Invoke the ProXPL interpreter/compiler
static void invoke_compiler(const char* file, bool run) {
    char command[MAX_PATH_LEN + 32];
    
    // Assumption: 'proxpl' is in PATH or current dir
    // In dev environment, we might want to point to build/proxpl
    const char* exe = "proxpl";
    
    #ifdef _WIN32
    // Try to find proxpl.exe in likely spots if system("proxpl") fails?
    // For now, assume it's in PATH
    #endif
    
    if (run) {
        // Run mode: proxpl src/main.prox
        snprintf(command, sizeof(command), "%s \"%s\"", exe, file);
    } else {
        // Build mode: proxpl build src/main.prox (Not fully implemented in main.c yet)
        printf("Compile-only mode not fully supported yet, running instead...\n");
        snprintf(command, sizeof(command), "%s \"%s\"", exe, file);
    }
    
    printf("[PRM] Executing: %s\n", command);
    int code = system(command);
    
    if (code != 0) {
        printf("[PRM] Process exited with code %d\n", code);
    }
}

void prm_build(const Manifest* manifest, bool releaseMode) {
    (void)releaseMode;
    printf("[PRM] Building project: %s v%s\n", manifest->name, manifest->version);
    // Phase 1: Just invoke compiler on entry point
    invoke_compiler(manifest->entryPoint, false);
}

void prm_run(const Manifest* manifest) {
    printf("[PRM] Running project: %s v%s\n", manifest->name, manifest->version);
    invoke_compiler(manifest->entryPoint, true);
}