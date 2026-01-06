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

<<<<<<< HEAD
#ifdef _WIN32
#include <direct.h>
#define MKDIR(d) _mkdir(d)
#define CHDIR(d) _chdir(d)
#else
#include <unistd.h>
#include <sys/stat.h>
#define MKDIR(d) mkdir(d, 0777)
#define CHDIR(d) chdir(d)
#endif

=======
>>>>>>> fix-ci-build
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
<<<<<<< HEAD
            manifest->name[MAX_NAME_LEN - 1] = '\0';
        } else if (strcmp(key, "version") == 0) {
            strncpy(manifest->version, value, MAX_VERSION_LEN - 1);
            manifest->version[MAX_VERSION_LEN - 1] = '\0';
        } else if (strcmp(key, "entry") == 0) {
            strncpy(manifest->entryPoint, value, MAX_PATH_LEN - 1);
            manifest->entryPoint[MAX_PATH_LEN - 1] = '\0';
=======
        } else if (strcmp(key, "version") == 0) {
            strncpy(manifest->version, value, MAX_VERSION_LEN - 1);
        } else if (strcmp(key, "entry") == 0) {
            strncpy(manifest->entryPoint, value, MAX_PATH_LEN - 1);
>>>>>>> fix-ci-build
        }
    }
}

bool prm_load_manifest(Manifest* manifest) {
<<<<<<< HEAD
    FILE* file = fopen("project.pxcf", "r");
=======
    FILE* file = fopen("prox.toml", "r");
>>>>>>> fix-ci-build
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
<<<<<<< HEAD
    if (MKDIR(name) != 0) {
        printf("Note: Directory '%s' might already exist.\n", name);
    }
    
    if (CHDIR(name) != 0) {
        printf("Error: Could not enter directory '%s'\n", name);
        return;
    }

    FILE* file = fopen("project.pxcf", "w");
    if (!file) {
        printf("Error: Could not create project.pxcf\n");
        return;
    }
    
    fprintf(file, "[project]\n");
    fprintf(file, "name = \"%s\"\n", name);
    fprintf(file, "version = \"0.1.0\"\n");
    fprintf(file, "description = \"A new ProXPL project\"\n");
    fprintf(file, "authors = [\"Your Name <you@example.com>\"]\n");
    fprintf(file, "license = \"MIT\"\n");
    fprintf(file, "entry = \"src/main.prox\"\n");

    fprintf(file, "\n[dependencies]\n");
    fprintf(file, "# std = \"1.0.0\"\n");
    
    fclose(file);
    
    
    // Create src directory
    if (MKDIR("src") != 0) {
        // Ignore error if directory exists
    }
    
    FILE* mainFile = fopen("src/main.prox", "w");
    if (mainFile) {
        fprintf(mainFile, "use std.io;\n\n");
=======
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
>>>>>>> fix-ci-build
        fprintf(mainFile, "std.io.print(\"Hello, %s!\");\n", name);
        fclose(mainFile);
    }
    
    printf("Created new project: %s\n", name);
}
