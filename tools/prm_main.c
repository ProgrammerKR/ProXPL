// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-22
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "../src/prm/prm.h"

void print_help() {
    printf("ProX Resource Manager (v0.1)\n");
    printf("Usage: prm <command> [args]\n");
    printf("\n");
    printf("Commands:\n");
    printf("  init <name>   Create a new project\n");
    printf("  build         Build the current project\n");
    printf("  run           Run the current project\n");
    printf("  test          Run tests (TODO)\n");
    printf("  help          Show this help message\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_help();
        return 0;
    }

    const char* command = argv[1];

    if (strcmp(command, "init") == 0) {
        if (argc < 3) {
            printf("Error: Missing project name.\nUsage: prm init <name>\n");
            return 1;
        }
        prm_init(argv[2]);
    } 
    else if (strcmp(command, "build") == 0) {
        Manifest manifest;
        if (prm_load_manifest(&manifest)) {
            prm_build(&manifest, false); // Debug mode by default
        } else {
            printf("Error: prox.toml not found. Are you in a ProXPL project directory?\n");
            return 1;
        }
    }
    else if (strcmp(command, "run") == 0) {
        Manifest manifest;
        if (prm_load_manifest(&manifest)) {
            prm_run(&manifest);
        } else {
            printf("Error: prox.toml not found. Are you in a ProXPL project directory?\n");
            return 1;
        }
    }
    else {
        printf("Unknown command: %s\n", command);
        print_help();
        return 1;
    }

    return 0;
}
