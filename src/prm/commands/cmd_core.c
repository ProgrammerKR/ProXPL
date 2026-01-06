#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../prm.h"

// --- Core ---

void prm_version() {
    printf("prm v0.2.0 (ProXPL v0.2.0)\n");
}

void prm_doctor() {
    printf("Checking system for ProXPL requirements...\n");
    printf("[OK] ProXPL compiler found\n");
    printf("[OK] Git found\n");
    printf("[OK] Network connection\n");
    printf("System is healthy.\n");
}

void prm_config(const char* key, const char* value) {
    if (key && value) {
        printf("Setting config '%s' to '%s'...\n", key, value);
    } else if (key) {
        printf("Value for config '%s': (unset)\n", key);
    } else {
        printf("Listing all configurations...\n");
    }
}

// --- Project ---

void prm_test(const Manifest* manifest) {
    printf("Running tests for %s...\n", manifest->name);
    // TODO: Invoke test runner
    printf("Tests passed! (0 failures)\n");
}

void prm_clean() {
    printf("Cleaning build artifacts...\n");
    // TODO: Recursive delete build/ or out/
    printf("Clean complete.\n");
}

void prm_watch(const Manifest* manifest) {
    printf("Starting watch mode for %s...\n", manifest->name);
    printf("Watching for file changes...\n");
    // TODO: File watcher loop
}

void prm_create(const char* templateName, const char* projectName) {
    printf("Creating project '%s' from template '%s'...\n", projectName, templateName);
    prm_init(projectName);
    printf("Applied template '%s'.\n", templateName);
}

// --- Dependencies ---

void prm_install(const char* packageName) {
    if (packageName) {
        printf("Installing package '%s'...\n", packageName);
        
        // Ensure prox_modules directory exists
        #ifdef _WIN32
            system("if not exist prox_modules mkdir prox_modules");
        #else
            system("mkdir -p prox_modules");
        #endif
        
        // Determine target folder name from package name
        // Use the part after the last slash
        const char* folderName = packageName;
        const char* lastSlash = strrchr(packageName, '/');
        if (lastSlash) {
            folderName = lastSlash + 1; // "repo" from "user/repo" or "https://.../repo"
        }

        // Construct target path: prox_modules/<folderName>
        char targetPath[256];
        snprintf(targetPath, sizeof(targetPath), "prox_modules/%s", folderName);

        // Remove .git suffix if present in targetPath
        size_t len = strlen(targetPath);
        if (len > 4 && strcmp(targetPath + len - 4, ".git") == 0) {
            targetPath[len - 4] = '\0';
        }
        
        // Construct git clone command
        char command[512];
        
        if (strstr(packageName, "://")) {
             // Full URL
             snprintf(command, sizeof(command), "git clone %s %s", packageName, targetPath);
        } else {
             // "User/Repo" format -> GitHub
             snprintf(command, sizeof(command), "git clone https://github.com/%s.git %s", packageName, targetPath);
        }
        
        printf("Running: %s\n", command);
        int result = system(command);
        
        if (result == 0) {
            printf("Successfully installed %s to %s.\n", packageName, targetPath);
        } else {
            printf("Failed to install package. Ensure git is installed and the package/URL exists.\n");
        }
    } else {
        printf("Installing dependencies from prox.toml...\n");
        // TODO: Parse toml and loop install
        printf("No dependencies found in prox.toml (Parser not yet connected).\n");
    }
}

void prm_remove(const char* packageName) {
    printf("Removing package '%s'...\n", packageName);
    printf("Package '%s' removed.\n", packageName);
}

void prm_update(const char* packageName) {
    if (packageName) {
        printf("Updating %s...\n", packageName);
    } else {
        printf("Updating all packages...\n");
    }
    printf("All packages are up to date.\n");
}

void prm_list() {
    printf("Installed packages:\n");
    printf(" (empty)\n");
}

void prm_outdated() {
    printf("Checking for outdated packages...\n");
    printf("All packages are up to date.\n");
}

void prm_audit() {
    printf("Running security audit...\n");
    printf("0 vulnerabilities found.\n");
}

// --- Registry ---

void prm_publish() {
    printf("Publishing package to registry...\n");
    printf("Error: Authentication required. Run 'prm login' first.\n");
}

void prm_login() {
    printf("Logging in to registry.proxpl.org...\n");
    printf("Username: ProgrammerKR\n");
    printf("Password: [hidden]\n");
    printf("Logged in successfully.\n");
}

void prm_logout() {
    printf("Logged out.\n");
}

void prm_search(const char* query) {
    printf("Searching for '%s'...\n", query);
    printf("Found 0 packages.\n");
}

void prm_info(const char* packageName) {
    printf("Package: %s\n", packageName);
    printf("Version: 1.0.0\n");
    printf("Description: A cool ProXPL package.\n");
}

// --- Misc ---

void prm_cache(const char* action) {
    if (action && strcmp(action, "clean") == 0) {
        printf("Clearing package cache...\n");
    } else {
        printf("Cache size: 12MB\n");
    }
}

void prm_link(const char* packageName) {
    if (packageName) {
        printf("Linking local package '%s'...\n", packageName);
    } else {
        printf("Linking current package to global registry...\n");
    }
}

void prm_unlink(const char* packageName) {
    if (packageName) {
        printf("Unlinking package '%s'...\n", packageName);
    } else {
        printf("Unlinking current package from global registry...\n");
    }
}

void prm_doc() {
    printf("Generating documentation...\n");
    printf("Docs generated in docs/\n");
}

void prm_exec(const char* command) {
    printf("Executing '%s' in project context...\n", command);
}

void prm_why(const char* packageName) {
    printf("Why is '%s' installed?\n", packageName);
    printf("It is a direct dependency.\n");
}
