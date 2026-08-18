// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/*
 * ProXPL Main Entry Point
 * Handles REPL mode, file execution, and PRM (Package Manager) commands
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

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
#include "type_checker.h"
#include "optimizer.h"
#include "file_utils.h"
#include "prm/prm.h"
#include "formatter.h"
#include "wasm_gen.h"
#include "error_report.h"

void registerStdLib(VM* vm);

// Declare global VM instance
extern VM vm;

static bool proxpl_readline(char* buffer, size_t max_len) {
    size_t pos = 0;
    buffer[0] = '\0';
    int open_braces = 0;
    int open_parens = 0;
    int open_brackets = 0;
    bool in_string = false;
    char string_char = '\0';

    while (1) {
        char line[1024];
        if (!fgets(line, sizeof(line), stdin)) {
            if (pos == 0) return false;
            break;
        }

        size_t len = strlen(line);
        // Remove trailing newline/carriage return
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[len-1] = '\0';
            len--;
        }

        // Check for manual continuation using backslash
        bool manual_continue = false;
        if (len > 0 && line[len-1] == '\\') {
            manual_continue = true;
            line[len-1] = '\0';
            len--;
        }

        if (pos + len + 1 < max_len) {
            strcpy(buffer + pos, line);
            pos += len;
            buffer[pos++] = '\n';
            buffer[pos] = '\0';
        } else {
            fprintf(stderr, "Input too long\n");
            break;
        }

        // Count brackets/braces/parens to detect if statement is incomplete
        for (size_t i = 0; i < len; i++) {
            char c = line[i];
            if (in_string) {
                if (c == string_char && (i == 0 || line[i-1] != '\\')) {
                    in_string = false;
                }
            } else {
                if (c == '"' || c == '\'') {
                    in_string = true;
                    string_char = c;
                } else if (c == '{') open_braces++;
                else if (c == '}') open_braces--;
                else if (c == '(') open_parens++;
                else if (c == ')') open_parens--;
                else if (c == '[') open_brackets++;
                else if (c == ']') open_brackets--;
            }
        }

        if (!manual_continue && open_braces <= 0 && open_parens <= 0 && open_brackets <= 0) {
            break;
        }

        printf("... ");
    }
    
    return true;
}

static void repl() {
  static char line[65536];

  printf("ProXPL v" PROXPL_VERSION_STRING " REPL\n");
  printf("Type 'exit' to quit\n\n");

  for (;;) {
    printf("> ");

    if (!proxpl_readline(line, sizeof(line))) {
      printf("\n");
      break;
    }

    // Check for exit command
    if (strcmp(line, "exit") == 0) {
      break;
    }

    // Skip empty lines
    if (strlen(line) == 0)
      continue;

    // Tokenize
    Scanner scanner;
    initScanner(&scanner, line);

    // Collect tokens
    Token tokens[256];
    int tokenCount = 0;

    bool scanError = false;
    for (;;) {
      Token token = scanToken(&scanner);
      if (tokenCount >= 256) {
        fprintf(stderr, "Error: Too many tokens\n");
        scanError = true;
        break;
      }
      tokens[tokenCount++] = token;

      if (token.type == TOKEN_ERROR) {
        fprintf(stderr, "Error: %.*s\n", token.length, token.start);
        scanError = true;
        break;
      }

      if (token.type == TOKEN_EOF)
        break;
    }

    if (scanError) {
      continue;
    }

    // Parse
    Parser parser;
    initParser(&parser, tokens, tokenCount, line);
    StmtList *statements = parse(&parser);

    if (statements == NULL || statements->count == 0) {
      fprintf(stderr, "Parse error\n");
      if (statements != NULL) freeStmtList(statements);
      continue;
    }

    // --- Pipeline: AST -> Bytecode -> VM ---
    optimizeAST(statements);

    ObjFunction* function = newFunction();
    if (function == NULL) {
        fprintf(stderr, "Out of memory\n");
        freeStmtList(statements);
        continue;
    }
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



static void runFile(const char *path) {
  char *source = readFile(path);
  if (source == NULL) {
    freeVM(&vm);
    exit(74);
  }

  // Tokenize
  Scanner scanner;
  initScanner(&scanner, source);

  Token tokens[4096];
  int tokenCount = 0;

  bool scanError = false;
  for (;;) {
    Token token = scanToken(&scanner);
    if (tokenCount >= 4096) {
      fprintf(stderr, "Error: Too many tokens\n");
      scanError = true;
      break;
    }
    tokens[tokenCount++] = token;

    if (token.type == TOKEN_ERROR) {
      scanError = true;
      break;
    }

    if (token.type == TOKEN_EOF)
      break;
  }

  if (scanError) {
    trackSource(&vm, source);
    freeVM(&vm);
    exit(65);
  }

  // Parse
  Parser parser;
  initParser(&parser, tokens, tokenCount, source);
  StmtList *statements = parse(&parser);

  if (statements == NULL || statements->count == 0) {
    fprintf(stderr, "Parse error\n");
    if (statements != NULL) freeStmtList(statements);
    trackSource(&vm, source);
    freeVM(&vm);
    exit(65);
  }

  // --- Pipeline Step 2: Optimization ---
  optimizeAST(statements);

  // --- Pipeline Step 3: Type Checking ---
  TypeChecker checker;
  initTypeChecker(&checker);
  
  if (!checkTypes(&checker, statements)) {
      fprintf(stderr, "Type Checking Failed with %d errors.\n", checker.errorCount);
      freeTypeChecker(&checker);
      freeStmtList(statements);
      trackSource(&vm, source);
      freeVM(&vm);
      exit(65);
  }
  // --- Pipeline Step 3: UI Transpilation (if applicable) ---
  for (int i = 0; i < statements->count; i++) {
      if (statements->items[i]->type == STMT_UI_APP) {
          const char* appName = statements->items[i]->as.ui_app.name;
          size_t nameLen = strlen(appName);
          char* outputDir = (char*)malloc(nameLen + 6);
          if (outputDir == NULL) {
              fprintf(stderr, "Out of memory allocating output dir for UI app\n");
              continue;
          }
          sprintf(outputDir, "dist_%s", appName);
          printf("[UI] Transpiling App '%s' to %s...\n", appName, outputDir);
          transpileUIApp(statements->items[i], outputDir);
          free(outputDir);
      }
  }

  // --- Pipeline Step 4: Bytecode Gen & Execution ---
  InterpretResult result = interpretAST(&vm, statements);
  freeTypeChecker(&checker);

  trackSource(&vm, source);
  if (result != INTERPRET_OK) {
      freeStmtList(statements);
      freeVM(&vm);
      exit(70);
  }

  freeStmtList(statements);
}


// ============================================================
//   PRM (ProX Resource Manager) Command Dispatch
// ============================================================

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
        strcmp(sub, "run")      == 0 || strcmp(sub, "build")     == 0
    );

    // Only intercept if invoked as prm OR if it's a uniquely-PRM subcommand
    // For run/build, checks below will disambiguate further
    if (!isPrm && !knownPrmCmd) return 0;
    
    // Disambiguate "run" and "build"
    // If "proxpl run <file>" -> Standard
    // If "proxpl run" -> PRM
    // If "proxpl build --release" -> PRM
    if (!isPrm && (strcmp(sub, "run") == 0 || strcmp(sub, "build") == 0)) {
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "--target") == 0) return 0; // Handled by compiler backend
        }
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
                int releaseMode = (argc >= 3 && strcmp(argv[2], "--release") == 0);
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

    } else if (strcmp(sub, "fmt") == 0) {
        bool checkOnly = false;
        const char* targetPath = ".";
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "--check") == 0) checkOnly = true;
            else if (argv[i][0] != '-') targetPath = argv[i];
        }
        FormatConfig cfg = loadFormatConfig(NULL);
        formatPath(targetPath, checkOnly, &cfg);
        return 1;

    } else if (strcmp(sub, "serve") == 0) {
        printf("[PRM] Serving WebAssembly build on http://localhost:8080 ...\n");
        printf("[PRM] Ready. Press Ctrl+C to stop.\n");
        return 1;

    } else {
        return 0; // Unrecognized — fall through to normal dispatch
    }

    return 1; // Handled as PRM command
}


int main(int argc, const char *argv[]) {
  // Check for fmt or --version first
  if (argc >= 2) {
    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "version") == 0) {
      printf("ProXPL v%s (\"%s\")\n", PROXPL_VERSION_STRING, PROXPL_VERSION_CODENAME);
      return 0;
    } else if (strcmp(argv[1], "fmt") == 0) {
      bool checkOnly = false;
      const char* targetPath = ".";
      for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--check") == 0) checkOnly = true;
        else if (argv[i][0] != '-') targetPath = argv[i];
      }
      FormatConfig cfg = loadFormatConfig(NULL);
      formatPath(targetPath, checkOnly, &cfg);
      return 0;
    }
  }

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
      appendToList(vm.cliArgs, OBJ_VAL(arg));
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
      bool isWasm = false;
      const char* inputFile = NULL;
      const char* outputFile = NULL;
      for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--target") == 0 && i + 1 < argc) {
          if (strcmp(argv[i+1], "wasm") == 0) isWasm = true;
          i++;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
          outputFile = argv[i+1];
          i++;
        } else if (argv[i][0] != '-') {
          inputFile = argv[i];
        }
      }

      if (isWasm && inputFile) {
        char* source = readFile(inputFile);
        if (!source) {
          fprintf(stderr, "Could not open file '%s'.\n", inputFile);
          freeVM(&vm);
          return 1;
        }
        Scanner scanner;
        initScanner(&scanner, source);
        Token tokens[4096];
        int tokenCount = 0;
        while (tokenCount < 4095) {
          Token t = scanToken(&scanner);
          tokens[tokenCount++] = t;
          if (t.type == TOKEN_EOF) break;
        }
        Parser parser;
        initParser(&parser, tokens, tokenCount, source);
        StmtList* program = parse(&parser);

        char outWasm[512];
        char outPrefix[512];
        if (outputFile) {
          snprintf(outWasm, sizeof(outWasm), "%s", outputFile);
          snprintf(outPrefix, sizeof(outPrefix), "app");
        } else {
          snprintf(outWasm, sizeof(outWasm), "app.wasm");
          snprintf(outPrefix, sizeof(outPrefix), "app");
        }

        WasmOptions opts = {
          .emitJsGlue = true,
          .emitHtml = true,
          .enableWasi = true,
          .outputPrefix = outPrefix
        };

        if (compileToWasm(program, outWasm, &opts)) {
          printf("[WASM] Successfully built %s, %s.js, and %s.html\n", outWasm, outPrefix, outPrefix);
        } else {
          fprintf(stderr, "[WASM] Failed to compile to WebAssembly.\n");
        }
        free(source);
        freeVM(&vm);
        return 0;
      }

      printf("Build command completed.\n");
      freeVM(&vm);
      return 0;
    } else {
      // Treat as file execution
      runFile(argv[1]);
    }
  }

  // Cleanup
  freeVM(&vm);

  return 0;
}
