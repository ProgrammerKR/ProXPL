// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/*
 * ProXPL Main Entry Point
 * Handles REPL mode, file execution, and PRM (Package Manager) commands
 */

#include "bytecode.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "parser.h"
#include "scanner.h"
#include "vm.h"
#include "transpiler_ui.h"
#include "object.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

void registerStdLib(VM* vm);

// Declare global VM instance - already in vm.c

static void repl() {
  char line[1024];

  printf("ProXPL v" PROXPL_VERSION_STRING " REPL\n");
  printf("Type 'exit' to quit\n\n");

  for (;;) {
    printf("> ");

    if (!fgets(line, sizeof(line), stdin)) {
      printf("\n");
      break;
    }

    // Check for exit command
    if (strcmp(line, "exit\n") == 0) {
      break;
    }

    // Remove newline
    line[strcspn(line, "\n")] = 0;

    // Skip empty lines
    if (strlen(line) == 0)
      continue;

    // Tokenize
    Scanner scanner;
    initScanner(&scanner, line);

    // Collect tokens
    Token tokens[256];
    int tokenCount = 0;

    for (;;) {
      Token token = scanToken(&scanner);
      tokens[tokenCount++] = token;

      if (token.type == TOKEN_ERROR) {
        fprintf(stderr, "Error: %.*s\n", token.length, token.start);
        break;
      }

      if (token.type == TOKEN_EOF)
        break;
      if (tokenCount >= 256) {
        fprintf(stderr, "Error: Too many tokens\n");
        break;
      }
    }

    if (tokens[tokenCount - 1].type == TOKEN_ERROR) {
      continue;
    }

    // Parse
    Parser parser;
    initParser(&parser, tokens, tokenCount, line);
    StmtList *statements = parse(&parser);

    if (statements == NULL || statements->count == 0) {
      fprintf(stderr, "Parse error\n");
      continue;
    }

    // --- Pipeline: AST -> Bytecode -> VM ---
    ObjFunction* function = newFunction();
    push(&vm, OBJ_VAL(function));
    
    if (!generateBytecode(statements, function)) {
        fprintf(stderr, "Compilation error\n");
        pop(&vm);
        freeStmtList(statements);
        continue;
    }
    pop(&vm);
    
    interpretChunk(&vm, &function->chunk);

    freeStmtList(statements);
  }
}

static char *readFile(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    return NULL;
  }

  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char *buffer = (char *)malloc(fileSize + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    fclose(file);
    return NULL;
  }

  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  if (bytesRead < fileSize) {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    free(buffer);
    fclose(file);
    return NULL;
  }

  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}

#include "type_checker.h"

static void runFile(const char *path) {
  char *source = readFile(path);
  if (source == NULL) {
    exit(74);
  }

  // Tokenize
  Scanner scanner;
  initScanner(&scanner, source);

  Token tokens[4096];
  int tokenCount = 0;

  for (;;) {
    Token token = scanToken(&scanner);
    tokens[tokenCount++] = token;

    if (token.type == TOKEN_ERROR) {
      fprintf(stderr, "Error at line %d, column %d: %.*s\n", token.line, token.column, token.length,
              token.start);
      const char* lineStart = source;
      int currentLine = 1;
      while (currentLine < token.line && *lineStart != '\0') {
          if (*lineStart == '\n') currentLine++;
          lineStart++;
      }
      const char* lineEnd = lineStart;
      while (*lineEnd != '\0' && *lineEnd != '\n') lineEnd++;
      
      fprintf(stderr, "%.*s\n", (int)(lineEnd - lineStart), lineStart);
      for(int i=1; i < token.column; i++) fprintf(stderr, " ");
      fprintf(stderr, "^\n");
      
      free(source);
      exit(65);
    }

    if (token.type == TOKEN_EOF)
      break;
    if (tokenCount >= 4096) {
      fprintf(stderr, "Error: Too many tokens\n");
      free(source);
      exit(65);
    }
  }

  // Parse
  Parser parser;
  initParser(&parser, tokens, tokenCount, source);
  StmtList *statements = parse(&parser);

  if (statements == NULL || statements->count == 0) {
    fprintf(stderr, "Parse error\n");
    free(source);
    exit(65);
  }

  // --- Pipeline Step 2: Type Checking ---
  TypeChecker checker;
  initTypeChecker(&checker);
  
  if (!checkTypes(&checker, statements)) {
      fprintf(stderr, "Type Checking Failed with %d errors.\n", checker.errorCount);
      freeTypeChecker(&checker);
      freeStmtList(statements);
      free(source);
      exit(65);
  }
  // --- Pipeline Step 3: UI Transpilation (if applicable) ---
  for (int i = 0; i < statements->count; i++) {
      if (statements->items[i]->type == STMT_UI_APP) {
          char outputDir[512];
          sprintf(outputDir, "dist_%s", statements->items[i]->as.ui_app.name);
          printf("[UI] Transpiling App '%s' to %s...\n", statements->items[i]->as.ui_app.name, outputDir);
          transpileUIApp(statements->items[i], outputDir);
      }
  }

  // --- Pipeline Step 4: Bytecode Gen & Execution ---
  InterpretResult result = interpretAST(&vm, statements);
    
  freeTypeChecker(&checker);

  if (result != INTERPRET_OK) {
      exit(70);
  }

  freeStmtList(statements);
  free(source);
}


// ============================================================
//   PRM (ProX Resource Manager) Include
// ============================================================
#include "prm/prm.h"

// ============================================================
//   PRM Command Dispatch
//   Returns 1 if handled as a PRM command, 0 to continue with normal dispatch
// ============================================================
static int dispatchPRM(int argc, const char* argv[]) {
    // Check if invoked as "prm" / "prm.exe" / "prm.bat"
    const char* exe = argv[0];
    int isPrm = 0;
    {
        const char* base = exe;
        for (const char* p = exe; *p; p++) {
            if (*p == '/' || *p == '\\') base = p + 1;
        }
        if (strncmp(base, "prm", 3) == 0) isPrm = 1;
    }

    const char* sub = (argc >= 2) ? argv[1] : NULL;

    // No subcommand: if invoked as prm, show help
    if (!sub) {
        if (isPrm) { prm_help(); return 1; }
        return 0;
    }

    // Recognize all PRM subcommands
    int knownPrmCmd = (
        strcmp(sub, "version")  == 0 || strcmp(sub, "--version") == 0 || strcmp(sub, "-v") == 0 ||
        strcmp(sub, "help")     == 0 || strcmp(sub, "--help")    == 0 || strcmp(sub, "-h") == 0 ||
        strcmp(sub, "doctor")   == 0 || strcmp(sub, "config")    == 0 ||
        strcmp(sub, "init")     == 0 || strcmp(sub, "clean")     == 0 ||
        strcmp(sub, "install")  == 0 || strcmp(sub, "remove")    == 0 ||
        strcmp(sub, "update")   == 0 || strcmp(sub, "list")      == 0 ||
        strcmp(sub, "outdated") == 0 || strcmp(sub, "audit")     == 0 ||
        strcmp(sub, "publish")  == 0 || strcmp(sub, "login")     == 0 ||
        strcmp(sub, "logout")   == 0 || strcmp(sub, "search")    == 0 ||
        strcmp(sub, "info")     == 0 || strcmp(sub, "cache")     == 0 ||
        strcmp(sub, "link")     == 0 || strcmp(sub, "unlink")    == 0 ||
        strcmp(sub, "doc")      == 0 || strcmp(sub, "exec")      == 0 ||
        strcmp(sub, "why")      == 0 || strcmp(sub, "create")    == 0 ||
        strcmp(sub, "test")     == 0 || strcmp(sub, "watch")     == 0 ||
        strcmp(sub, "run")      == 0 || strcmp(sub, "build")     == 0 ||
        (strcmp(sub, "build") == 0 && argc >= 3 && strcmp(argv[2], "web") == 0)
    );

    // Only intercept if invoked as prm OR if it's a uniquely-PRM subcommand
    // For run/build, checks below will disambiguate further
    if (!isPrm && !knownPrmCmd) return 0;
    
    // Disambiguate "run" and "build"
    // If "proxpl run <file>" -> Standard
    // If "proxpl run" -> PRM
    // If "proxpl build --release" -> PRM
    if (!isPrm && (strcmp(sub, "run") == 0 || strcmp(sub, "build") == 0)) {
        if (argc >= 3 && argv[2][0] != '-') {
            // Has 3rd argument and it's not a flag -> Likely a file path
            return 0; 
        }
    }

    // ---- Core Commands ----
    if (strcmp(sub, "version") == 0 || strcmp(sub, "--version") == 0 || strcmp(sub, "-v") == 0) {
        prm_version();

    } else if (strcmp(sub, "help") == 0 || strcmp(sub, "--help") == 0 || strcmp(sub, "-h") == 0) {
        prm_help();

    } else if (strcmp(sub, "doctor") == 0) {
        prm_doctor();

    } else if (strcmp(sub, "config") == 0) {
        const char* key   = (argc >= 3) ? argv[2] : NULL;
        const char* value = (argc >= 4) ? argv[3] : NULL;
        prm_config(key, value);

    // ---- Project Commands ----
    } else if (strcmp(sub, "init") == 0) {
        if (argc < 3) { fprintf(stderr, "Usage: prm init <name>\n"); exit(64); }
        prm_init(argv[2]);

    } else if (strcmp(sub, "clean") == 0) {
        prm_clean();

    } else if (strcmp(sub, "create") == 0) {
        if (argc < 4) { fprintf(stderr, "Usage: prm create <template> <name>\n"); exit(64); }
        prm_create(argv[2], argv[3]);

    } else if (strcmp(sub, "run") == 0 || strcmp(sub, "build") == 0 ||
               strcmp(sub, "test") == 0 || strcmp(sub, "watch") == 0) {
        // Unified manifest loading
        Manifest m;
        const char* hint = (argc >= 3 && strcmp(argv[2], "web") != 0) ? argv[2] : NULL;
        
        if (!prm_load_manifest_auto(&m, hint)) {
            fprintf(stderr, "Error: No project.pxcf found in the current directory or specified path.\n");
            fprintf(stderr, "Run 'prm init <name>' to create a new project, or 'cd' into your project folder.\n");
            exit(1);
        }

        if (strcmp(sub, "run") == 0) {
            prm_run(&m);

        } else if (strcmp(sub, "build") == 0) {
            if (argc >= 3 && strcmp(argv[2], "web") == 0) {
                const char* outDir = NULL;
                for (int i = 2; i < argc; i++) {
                    if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
                        outDir = argv[i+1];
                        break;
                    }
                }
                prm_build_web(&m, outDir);
            } else {
                bool releaseMode = (argc >= 3 && strcmp(argv[2], "--release") == 0);
                prm_build(&m, releaseMode);
            }

        } else if (strcmp(sub, "test") == 0) {
            prm_test(&m);

        } else if (strcmp(sub, "watch") == 0) {
            prm_watch(&m);
        }

    // ---- Dependency Commands ----
    } else if (strcmp(sub, "install") == 0) {
        prm_install((argc >= 3) ? argv[2] : NULL);

    } else if (strcmp(sub, "remove") == 0) {
        if (argc < 3) { fprintf(stderr, "Usage: prm remove <package>\n"); exit(64); }
        prm_remove(argv[2]);

    } else if (strcmp(sub, "update") == 0) {
        prm_update((argc >= 3) ? argv[2] : NULL);

    } else if (strcmp(sub, "list") == 0) {
        prm_list();

    } else if (strcmp(sub, "outdated") == 0) {
        prm_outdated();

    } else if (strcmp(sub, "audit") == 0) {
        prm_audit();

    } else if (strcmp(sub, "why") == 0) {
        if (argc < 3) { fprintf(stderr, "Usage: prm why <package>\n"); exit(64); }
        prm_why(argv[2]);

    // ---- Registry Commands ----
    } else if (strcmp(sub, "publish") == 0) {
        prm_publish();

    } else if (strcmp(sub, "login") == 0) {
        prm_login();

    } else if (strcmp(sub, "logout") == 0) {
        prm_logout();

    } else if (strcmp(sub, "search") == 0) {
        if (argc < 3) { fprintf(stderr, "Usage: prm search <query>\n"); exit(64); }
        prm_search(argv[2]);

    } else if (strcmp(sub, "info") == 0) {
        if (argc < 3) { fprintf(stderr, "Usage: prm info <package>\n"); exit(64); }
        prm_info(argv[2]);

    // ---- Misc Commands ----
    } else if (strcmp(sub, "cache") == 0) {
        prm_cache((argc >= 3) ? argv[2] : NULL);

    } else if (strcmp(sub, "link") == 0) {
        prm_link((argc >= 3) ? argv[2] : NULL);

    } else if (strcmp(sub, "unlink") == 0) {
        prm_unlink((argc >= 3) ? argv[2] : NULL);

    } else if (strcmp(sub, "doc") == 0) {
        prm_doc();

    } else if (strcmp(sub, "exec") == 0) {
        if (argc < 3) { fprintf(stderr, "Usage: prm exec <command>\n"); exit(64); }
        prm_exec(argv[2]);

    } else {
        return 0; // Unrecognized — fall through to normal dispatch
    }

    return 1; // Handled as PRM command
}


int main(int argc, const char *argv[]) {
  // Try PRM dispatch first (handles prm.bat -> proxpl.exe delegation)
  if (dispatchPRM(argc, argv)) {
    return 0;
  }

  // Initialize VM
  initVM(&vm);

  // Register standard library
  registerStdLib(&vm);
  
  // Populate CLI args
  vm.cliArgs = newList();
  push(&vm, OBJ_VAL(vm.cliArgs)); // Protect from GC
  for(int i=0; i < argc; i++) {
      ObjString* arg = copyString(argv[i], (int)strlen(argv[i]));
      push(&vm, OBJ_VAL(arg));
      if (vm.cliArgs->capacity < vm.cliArgs->count + 1) {
          int oldCapacity = vm.cliArgs->capacity;
          vm.cliArgs->capacity = GROW_CAPACITY(oldCapacity);
          vm.cliArgs->items = GROW_ARRAY(Value, vm.cliArgs->items, oldCapacity, vm.cliArgs->capacity);
      }
      vm.cliArgs->items[vm.cliArgs->count++] = OBJ_VAL(arg);
      pop(&vm); // arg
  }
  pop(&vm); // cliArgs

  if (argc == 1) {
    // REPL mode
    repl();
  } else if (argc == 2) {
    // File execution mode
    runFile(argv[1]);
  } else if (argc >= 3) {
    // Handle subcommands
    const char *command = argv[1];

    if (strcmp(command, "run") == 0) {
      runFile(argv[2]);
    } else if (strcmp(command, "build") == 0) {
      printf("Build command not yet implemented\n");
      exit(1);
    } else if (argv[1][strlen(argv[1]) - 1] == 'x' &&
               argv[1][strlen(argv[1]) - 2] == 'o' &&
               argv[1][strlen(argv[1]) - 3] == 'r' &&
               argv[1][strlen(argv[1]) - 4] == 'p' &&
               argv[1][strlen(argv[1]) - 5] == '.') {
      // File with .prox extension
      runFile(argv[1]);
    } else {
      fprintf(stderr, "Usage: proxpl [path]\n");
      fprintf(stderr, "       proxpl run [path]\n");
      fprintf(stderr, "       proxpl          (REPL mode)\n");
      exit(64);
    }
  }

  // Cleanup
  freeVM(&vm);

  return 0;
}
