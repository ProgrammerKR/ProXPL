// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-22
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prm.h"

// Very basic TOML line parser for Phase 1
// Looks for key = "value"
static void parseLine(char* line, Manifest* manifest) {
    char key[64];
    char value[256];
    
    // Remove newline
    char* newline = strchr(line, '\n');
    if (newline) *newline = '\0';
    
    // Skip [headers] and comments
    if (line[0] == '[' || line[0] == '#' || line[0] == '\0') return;
    
    // Parse: key = "value"
    if (sscanf(line, " %63[^ =] = \"%255[^\"]\"", key, value) == 2) {
        if (strcmp(key, "name") == 0) {
            strncpy(manifest->name, value, MAX_NAME_LEN - 1);
        } else if (strcmp(key, "version") == 0) {
            strncpy(manifest->version, value, MAX_VERSION_LEN - 1);
        } else if (strcmp(key, "entry") == 0) {
            strncpy(manifest->entryPoint, value, MAX_PATH_LEN - 1);
        }
    }
}

bool prm_load_manifest(Manifest* manifest) {
    FILE* file = fopen("prox.toml", "r");
    if (!file) return false;
    
    // Defaults
    strcpy(manifest->entryPoint, "src/main.prox");
    strcpy(manifest->version, "0.1.0");
    strcpy(manifest->name, "untitled");
    
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        parseLine(line, manifest);
    }
    
    fclose(file);
    return true;
}

void prm_init(const char* name) {
    FILE* file = fopen("prox.toml", "w");
    if (!file) {
        printf("Error: Could not create prox.toml\n");
        return;
    }
    
    fprintf(file, "[package]\n");
    fprintf(file, "name = \"%s\"\n", name);
    fprintf(file, "version = \"0.1.0\"\n");
    fprintf(file, "entry = \"src/main.prox\"\n");
    
    fclose(file);
    
    // Create src directory
    system("mkdir src"); // Windows/Linux compatible enough for simple cases
    
    FILE* mainFile = fopen("src/main.prox", "w");
    if (mainFile) {
        fprintf(mainFile, "import std.io;\n\n");
        fprintf(mainFile, "std.io.print(\"Hello, %s!\");\n", name);
        fclose(mainFile);
    }
    
    printf("Created new project: %s\n", name);
}
