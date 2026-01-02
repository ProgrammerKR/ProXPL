// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-22
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../src/prm/prm.h"

void print_help() {
    printf("ProX Resource Manager (v0.5.0)\n");
    printf("Usage: prm <command> [args]\n\n");
    
    printf("Core Commands:\n");
    printf("  init <name>     Initialize a new project\n");
    printf("  version, v      Show version\n");
    printf("  help, h         Show this help message\n");
    printf("  doctor          Check system health\n");
    printf("  config          Manage configuration\n\n");

    printf("Project Commands:\n");
    printf("  build           Build the current project\n");
    printf("  run             Run the current project\n");
    printf("  test, t         Run tests\n");
    printf("  clean           Clean build artifacts\n");
    printf("  watch           Run in watch mode\n");
    printf("  create, new     Create from template\n\n");

    printf("Dependency Commands:\n");
    printf("  install, i      Install dependencies\n");
    printf("  remove, rm      Remove dependencies\n");
    printf("  update, up      Update dependencies\n");
    printf("  list, ls        List installed packages\n");
    printf("  outdated        Check for outdated dependencies\n");
    printf("  audit           Run security audit\n\n");

    printf("Registry Commands:\n");
    printf("  publish         Publish package to registry\n");
    printf("  login           Login to registry\n");
    printf("  logout          Logout from registry\n");
    printf("  search          Search for packages\n");
    printf("  info            Show package info\n\n");

    printf("Misc Commands:\n");
    printf("  cache           Manage package cache\n");
    printf("  link            Symlink local package\n");
    printf("  unlink          Unlink package\n");
    printf("  doc             Generate documentation\n");
    printf("  exec            Execute arbitrary command\n");
    printf("  why             Explain why a package is installed\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_help();
        return 0;
    }

    const char* cmd = argv[1];

    // --- Core ---
    if (strcmp(cmd, "init") == 0) {
        if (argc < 3) { printf("Error: Missing project name.\nUsage: prm init <name>\n"); return 1; }
        prm_init(argv[2]);
    }
    else if (strcmp(cmd, "version") == 0 || strcmp(cmd, "v") == 0 || strcmp(cmd, "--version") == 0 || strcmp(cmd, "-v") == 0) {
        prm_version();
    }
    else if (strcmp(cmd, "help") == 0 || strcmp(cmd, "h") == 0 || strcmp(cmd, "--help") == 0 || strcmp(cmd, "-h") == 0) {
        print_help();
    }
    else if (strcmp(cmd, "doctor") == 0) {
        prm_doctor();
    }
    else if (strcmp(cmd, "config") == 0) {
        prm_config(argc > 2 ? argv[2] : NULL, argc > 3 ? argv[3] : NULL);
    }

    // --- Project ---
    else if (strcmp(cmd, "build") == 0) {
        Manifest m;
        if (prm_load_manifest(&m)) prm_build(&m, false);
        else return 1;
    }
    else if (strcmp(cmd, "run") == 0) {
        Manifest m;
        if (prm_load_manifest(&m)) prm_run(&m);
        else return 1;
    }
    else if (strcmp(cmd, "test") == 0 || strcmp(cmd, "t") == 0) {
        Manifest m;
        if (prm_load_manifest(&m)) prm_test(&m);
        else return 1;
    }
    else if (strcmp(cmd, "clean") == 0) {
        prm_clean();
    }
    else if (strcmp(cmd, "watch") == 0) {
        Manifest m;
        if (prm_load_manifest(&m)) prm_watch(&m);
        else return 1;
    }
    else if (strcmp(cmd, "create") == 0 || strcmp(cmd, "new") == 0) {
        if (argc < 4) { printf("Usage: prm create <template> <name>\n"); return 1; }
        prm_create(argv[2], argv[3]);
    }

    // --- Dependencies ---
    else if (strcmp(cmd, "install") == 0 || strcmp(cmd, "i") == 0) {
        prm_install(argc > 2 ? argv[2] : NULL);
    }
    else if (strcmp(cmd, "remove") == 0 || strcmp(cmd, "rm") == 0) {
        if (argc < 3) { printf("Usage: prm remove <package>\n"); return 1; }
        prm_remove(argv[2]);
    }
    else if (strcmp(cmd, "update") == 0 || strcmp(cmd, "up") == 0) {
        prm_update(argc > 2 ? argv[2] : NULL);
    }
    else if (strcmp(cmd, "list") == 0 || strcmp(cmd, "ls") == 0) {
        prm_list();
    }
    else if (strcmp(cmd, "outdated") == 0) {
        prm_outdated();
    }
    else if (strcmp(cmd, "audit") == 0) {
        prm_audit();
    }

    // --- Registry ---
    else if (strcmp(cmd, "publish") == 0) {
        prm_publish();
    }
    else if (strcmp(cmd, "login") == 0) {
        prm_login();
    }
    else if (strcmp(cmd, "logout") == 0) {
        prm_logout();
    }
    else if (strcmp(cmd, "search") == 0) {
        if (argc < 3) { printf("Usage: prm search <query>\n"); return 1; }
        prm_search(argv[2]);
    }
    else if (strcmp(cmd, "info") == 0) {
        if (argc < 3) { printf("Usage: prm info <package>\n"); return 1; }
        prm_info(argv[2]);
    }

    // --- Misc ---
    else if (strcmp(cmd, "cache") == 0) {
        prm_cache(argc > 2 ? argv[2] : NULL);
    }
    else if (strcmp(cmd, "link") == 0) {
         prm_link(argc > 2 ? argv[2] : NULL);
    }
    else if (strcmp(cmd, "unlink") == 0) {
         prm_unlink(argc > 2 ? argv[2] : NULL);
    }
    else if (strcmp(cmd, "doc") == 0) {
        prm_doc();
    }
    else if (strcmp(cmd, "exec") == 0) {
        if (argc < 3) { printf("Usage: prm exec <cmd>\n"); return 1; }
        prm_exec(argv[2]);
    }
    else if (strcmp(cmd, "why") == 0) {
        if (argc < 3) { printf("Usage: prm why <package>\n"); return 1; }
        prm_why(argv[2]);
    }
    else {
        printf("Unknown command: %s\n", cmd);
        print_help();
        return 1;
    }

    return 0;
}
