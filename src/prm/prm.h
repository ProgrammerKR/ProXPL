// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-22
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#ifndef PROX_PRM_H
#define PROX_PRM_H

#include <stdbool.h>

// Max lengths
#define MAX_PATH_LEN 1024
#define MAX_NAME_LEN 64
#define MAX_VERSION_LEN 32

// Project Manifest (prox.toml)
typedef struct {
    char name[MAX_NAME_LEN];
    char version[MAX_VERSION_LEN];
    char entryPoint[MAX_PATH_LEN]; // e.g., src/main.prox
    // char** dependencies; // Future
} Manifest;

// Initialize a new project in the given directory
void prm_init(const char* name);

// Parse prox.toml from the current directory
bool prm_load_manifest(Manifest* manifest);

// Build the project
void prm_build(const Manifest* manifest, bool releaseMode);

// Run the project
void prm_run(const Manifest* manifest);

#endif
