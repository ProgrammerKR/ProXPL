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

// Very basic parser for phase 1 of PRM.
// Looks for key: "value" across the block structure
static void parseLine(char* line, Manifest* manifest) {
    char key[64];
    char value[256];
    
    // Remove newline
    char* newline = strchr(line, '\n');
    if (newline) *newline = '\0';
    
    // Skip comments and basic blocks without data
    if (line[0] == '#' || line[0] == '\0' || strchr(line, '{') || strchr(line, '}')) return;
    
    // Parse: key: "value"
    if (sscanf(line, " %63[^:] : \"%255[^\"]\"", key, value) == 2) {
        if (strcmp(key, "name") == 0) {
            strncpy(manifest->name, value, MAX_NAME_LEN - 1);
            manifest->name[MAX_NAME_LEN - 1] = '\0';
        } else if (strcmp(key, "version") == 0) {
            strncpy(manifest->version, value, MAX_VERSION_LEN - 1);
            manifest->version[MAX_VERSION_LEN - 1] = '\0';
        } else if (strcmp(key, "entry") == 0 || strcmp(key, "src") == 0) {
            // "entry" or "src" can serve as the entry point marker for PRM
            strncpy(manifest->entryPoint, value, MAX_PATH_LEN - 1);
            manifest->entryPoint[MAX_PATH_LEN - 1] = '\0';
        }
    }
}

bool prm_load_manifest(Manifest* manifest) {
    FILE* file = fopen("project.pxcf", "r");
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
    
    fprintf(file, "# ProX Project Configuration\n\n");
    fprintf(file, "project {\n");
    fprintf(file, "    name: \"%s\"\n", name);
    fprintf(file, "    version: \"0.1.0\"\n");
    fprintf(file, "    author: \"Your Name <you@example.com>\"\n");
    fprintf(file, "    license: \"MIT\"\n");
    fprintf(file, "}\n\n");

    fprintf(file, "compiler {\n");
    fprintf(file, "    optimize: true\n");
    fprintf(file, "    debug: false\n");
    fprintf(file, "    target: \"native\"\n");
    fprintf(file, "}\n\n");

    fprintf(file, "paths {\n");
    fprintf(file, "    src: \"./src\"\n");
    fprintf(file, "    build: \"./build\"\n");
    fprintf(file, "    entry: \"src/main.prox\"\n");
    fprintf(file, "}\n\n");

    fprintf(file, "dependencies {\n");
    fprintf(file, "    # http: \"1.2.0\"\n");
    fprintf(file, "}\n\n");

    fprintf(file, "runtime {\n");
    fprintf(file, "    threads: 8\n");
    fprintf(file, "    memory_limit: \"1GB\"\n");
    fprintf(file, "}\n");
    
    fclose(file);
    
    
    // Create src directory
    if (MKDIR("src") != 0) {
        // Ignore error if directory exists
    }
    
    FILE* mainFile = fopen("src/main.prox", "w");
    if (mainFile) {
        fprintf(mainFile, "use std.io;\n\n");
        fprintf(mainFile, "std.io.print(\"Hello, %s!\");\n", name);
        fclose(mainFile);
    }
    
    // Initialize storage (lockfile and cache)
    prm_init_cache();
    Manifest dummy;
    strcpy(dummy.name, name);
    strcpy(dummy.version, "0.1.0");
    prm_save_lockfile(&dummy);

    printf("Created new project: %s\n", name);
}

void prm_save_lockfile(const Manifest* manifest) {
    FILE* file = fopen("project-lock.pxcf", "w");
    if (!file) return;

    fprintf(file, "# ProXPL Project Lockfile - DO NOT EDIT MANUALLY\n");
    fprintf(file, "lock_version: \"1.0.0\"\n");
    fprintf(file, "project: \"%s\"\n", manifest->name);
    fprintf(file, "version: \"%s\"\n\n", manifest->version);
    
    fprintf(file, "dependencies {\n");
    fprintf(file, "    # Resolved hashes will go here\n");
    fprintf(file, "}\n");
    
    fclose(file);
}

void prm_init_cache() {
    #ifdef _WIN32
        _mkdir(".pxcache");
    #else
        mkdir(".pxcache", 0777);
    #endif
    
    FILE* readme = fopen(".pxcache/README.txt", "w");
    if (readme) {
        fprintf(readme, "ProXPL Compiler Cache. Safe to delete.\n");
        fclose(readme);
    }
}

bool prm_is_cached(const char* artifactName) {
    char path[256];
    snprintf(path, sizeof(path), ".pxcache/%s", artifactName);
    FILE* f = fopen(path, "r");
    if (f) {
        fclose(f);
        return true;
    }
    return false;
}

bool prm_load_manifest_auto(Manifest* manifest, const char* hintName) {
    // 1. Try current directory
    if (prm_load_manifest(manifest)) return true;

    // 2. Try hint directory (e.g. if user just did 'prm init project' and is still in parent)
    if (hintName) {
        char path[512];
        snprintf(path, sizeof(path), "%s/project.pxcf", hintName);
        FILE* f = fopen(path, "r");
        if (f) {
            fclose(f);
            if (CHDIR(hintName) == 0) {
                return prm_load_manifest(manifest);
            }
        }
    }

    // 3. (Optional) Walk subdirectories to find exactly one project.pxcf
    // For now, let's keep it simple and just do the hint or current.
    
    return false;
}
