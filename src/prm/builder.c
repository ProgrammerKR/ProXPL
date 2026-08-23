#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prm.h"
#include "transpiler_ui.h"
#include "scanner.h"
#include "parser.h"

#ifdef _WIN32
#include <process.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#endif
#include <sys/stat.h>

#include "compiler.h"
#include "optimizer.h"
#include "bytecode.h"
#include "vm.h"
#include "object.h"

extern VM vm;

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

static long get_mtime(const char* path) {
    struct stat st;
    if (stat(path, &st) == 0) return st.st_mtime;
    return 0;
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

void prm_build(const Manifest* manifest, BuildOptions options) {
    printf("[PRM] Building project: %s v%s\n", manifest->name, manifest->version);
    
    if (options.releaseMode) printf("[PRM] Mode: Release\n");
    if (options.debugMode) printf("[PRM] Mode: Debug\n");
    if (options.profileMode) printf("[PRM] Mode: Profile\n");
    if (options.traceMode) printf("[PRM] Mode: Trace\n");
    if (options.sanitizeMode) printf("[PRM] Mode: Sanitize\n");
    
    prm_init_cache();
    prm_save_lockfile(manifest);
    
    const char* targetDir = ".proxpl/cache";
#ifdef _WIN32
    _mkdir(".proxpl");
    _mkdir(targetDir);
#else
    mkdir(".proxpl", 0777);
    mkdir(targetDir, 0777);
#endif
    
    char cacheFile[512];
    const char* ep = manifest->entryPoint;
    const char* lastSlash = strrchr(ep, '/');
    if (!lastSlash) lastSlash = strrchr(ep, '\\');
    const char* basename = lastSlash ? lastSlash + 1 : ep;
    snprintf(cacheFile, sizeof(cacheFile), "%s/%s.pxbc", targetDir, basename);

    long src_mtime = get_mtime(manifest->entryPoint);
    long cache_mtime = get_mtime(cacheFile);

    if (cache_mtime > 0 && cache_mtime >= src_mtime) {
        printf("[PRM] [Cache Hit] Loading compiled bytecode from %s\n", cacheFile);
        Chunk chunk;
        if (loadPXBC(cacheFile, &chunk) == 0) {
            ObjFunction* function = newFunction();
            function->chunk = chunk;
            push(&vm, OBJ_VAL(function));
            interpretChunk(&vm, &function->chunk);
            pop(&vm);
            return;
        } else {
            printf("[PRM] [Cache Miss] Failed to load cache, recompiling...\n");
        }
    } else {
        printf("[PRM] [Cache Miss] Source modified or unbuilt, recompiling %s...\n", manifest->entryPoint);
    }
    
    char* source = read_file_prm(manifest->entryPoint);
    if (!source) {
        fprintf(stderr, "[PRM] Error: Could not read entry point '%s'\n", manifest->entryPoint);
        return;
    }

    Scanner scanner;
    initScanner(&scanner, source);
    Token tokens[4096];
    int tokenCount = 0;
    while (1) {
        Token t = scanToken(&scanner);
        tokens[tokenCount++] = t;
        if (t.type == TOKEN_EOF || tokenCount >= 4096) break;
    }

    Parser parser;
    initParser(&parser, tokens, tokenCount, source);
    StmtList* statements = parse(&parser);
    if (!statements) { free(source); return; }
    
    optimizeAST(statements);
    
    ObjFunction* function = newFunction();
    push(&vm, OBJ_VAL(function));
    if (!generateBytecode(statements, function)) {
        pop(&vm); freeStmtList(statements); free(source); return;
    }
    pop(&vm);
    
    printf("[PRM] Caching compiled artifact to %s\n", cacheFile);
    dumpPXBC(cacheFile, &function->chunk);
    
    push(&vm, OBJ_VAL(function));
    interpretChunk(&vm, &function->chunk);
    pop(&vm);
    
    freeStmtList(statements);
    free(source);
}

void prm_inspect(const Manifest* manifest, const char* file, const char* format) {
    const char* target = file ? file : manifest->entryPoint;
    printf("[PRM] Inspecting %s (Format: %s)\n", target, format ? format : "default");
    
    char command[MAX_PATH_LEN + 64];
    const char* exe = "proxpl";
    snprintf(command, sizeof(command), "%s inspect \"%s\" %s", exe, target, format ? format : "");
    
    printf("[PRM] Executing: %s\n", command);
    int code = -1;
    #ifdef _WIN32
    code = _spawnlp(_P_WAIT, exe, exe, "inspect", target, format, NULL);
    #else
    pid_t pid = fork();
    if (pid == 0) {
        execlp(exe, exe, "inspect", target, format, (char*)NULL);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) code = WEXITSTATUS(status);
    }
    #endif
    
    if (code != 0) {
        printf("[PRM] Inspect process exited with code %d\n", code);
    }
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
