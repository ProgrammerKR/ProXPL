#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prm.h"
#include "transpiler_ui.h"
#include "scanner.h"
#include "parser.h"

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

// Helper to read file content for parsing
static char* read_file_prm(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) return NULL;
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    char* buffer = malloc(size + 1);
    if (!buffer) { fclose(file); return NULL; }
    size_t read = fread(buffer, 1, size, file);
    buffer[read] = '\0';
    fclose(file);
    return buffer;
}

// Invoke the ProXPL interpreter/compiler
static void invoke_compiler(const char* file, bool run) {
    char command[MAX_PATH_LEN + 32];
    
    // Assumption: 'proxpl' is in PATH or current dir
    const char* exe = "proxpl";
    
    if (run) {
        snprintf(command, sizeof(command), "%s \"%s\"", exe, file);
    } else {
        printf("Compile-only mode not fully supported yet, running instead...\n");
        snprintf(command, sizeof(command), "%s \"%s\"", exe, file);
    }
    
    // Simple validation
    const char* p = file;
    bool safe = true;
    while(p && *p) {
        if(strchr("&|;><`$\\", *p)) { safe = false; break; }
        p++;
    }
    if (!safe) {
        fprintf(stderr, "[PRM] Error: Invalid characters in file path.\n");
        return;
    }
    
    printf("[PRM] Executing: %s \"%s\"\n", exe, file);
    
    int code = -1;
    #ifdef _WIN32
    code = _spawnlp(_P_WAIT, exe, exe, file, NULL);
    #else
    pid_t pid = fork();
    if (pid == 0) {
        execlp(exe, exe, file, (char*)NULL);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) code = WEXITSTATUS(status);
    }
    #endif
    
    if (code != 0) {
        printf("[PRM] Process exited with code %d\n", code);
    }
}

void prm_build(const Manifest* manifest, bool releaseMode) {
    (void)releaseMode;
    printf("[PRM] Building project: %s v%s\n", manifest->name, manifest->version);
    
    prm_init_cache();
    prm_save_lockfile(manifest);
    // In the future, check if build is needed
    
    invoke_compiler(manifest->entryPoint, false);
}

void prm_build_web(const Manifest* manifest, const char* outputDir) {
    printf("[PRM] Building Web App: %s v%s\n", manifest->name, manifest->version);
    prm_init_cache();
    char* source = read_file_prm(manifest->entryPoint);
    if (!source) {
        fprintf(stderr, "[PRM] Error: Could not read entry point '%s'\n", manifest->entryPoint);
        return;
    }

    // Pipeline: Scanner -> Parser
    Scanner scanner;
    initScanner(&scanner, source);

    Token tokens[4096];
    int tokenCount = 0;
    for (;;) {
        Token token = scanToken(&scanner);
        tokens[tokenCount++] = token;
        if (token.type == TOKEN_EOF || tokenCount >= 4096) break;
    }

    Parser parser;
    initParser(&parser, tokens, tokenCount, source);
    StmtList* statements = parse(&parser);

    if (!statements) {
        fprintf(stderr, "[PRM] Error: Parse failed for '%s'\n", manifest->entryPoint);
        free(source);
        return;
    }

    // Target directory
    char finalDir[512];
    if (outputDir) {
        strncpy(finalDir, outputDir, 511);
        finalDir[511] = '\0';
    } else {
        sprintf(finalDir, "dist_%s", manifest->name);
    }

    bool foundApp = false;
    for (int i = 0; i < statements->count; i++) {
        if (statements->items[i]->type == STMT_UI_APP) {
            printf("[PRM] Found App '%s', transpiling to web...\n", statements->items[i]->as.ui_app.name);
            transpileUIAppWeb(statements->items[i], finalDir);
            foundApp = true;
            break; 
        }
    }

    if (!foundApp) {
        printf("[PRM] Error: No 'App' definition found in '%s'. 'prm build web' requires a UI App.\n", manifest->entryPoint);
    }

    freeStmtList(statements);
    free(source);
}

void prm_run(const Manifest* manifest) {
    printf("[PRM] Running project: %s v%s\n", manifest->name, manifest->version);
    prm_save_lockfile(manifest);
    invoke_compiler(manifest->entryPoint, true);
}
