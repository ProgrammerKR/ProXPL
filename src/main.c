/*
 * ProXPL Main Entry Point
 * Handles REPL mode and file execution
 */

#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "parser.h"
#include "scanner.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void registerStdLib(VM* vm);

static void repl(VM *vm) {
  char line[1024];

  printf("ProXPL v1.0 REPL\n");
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
    initParser(&parser, tokens, tokenCount);
    StmtList *statements = parse(&parser);

    if (statements == NULL || statements->count == 0) {
      fprintf(stderr, "Parse error\n");
      continue;
    }

    // TODO: Compile and execute
    // For now, just indicate success
    printf("Parsed successfully (%d statements)\n", statements->count);

    // Free AST
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

extern void generateCode(StmtList* statements); // Defined in llvm_backend.cpp
#include "type_checker.h"

static void runFile(VM *vm, const char *path) {
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
      fprintf(stderr, "Error at line %d: %.*s\n", token.line, token.length,
              token.start);
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
  initParser(&parser, tokens, tokenCount);
  StmtList *statements = parse(&parser);

  if (statements == NULL || statements->count == 0) {
    fprintf(stderr, "Parse error\n");
    free(source);
    exit(65);
  }

  printf("Successfully parsed %d statements from %s\n", statements->count,
         path);

  // --- Pipeline Step 2: Type Checking ---
  printf("Running Type Checker...\n");
  TypeChecker checker;
  initTypeChecker(&checker);
  
  if (!checkTypes(&checker, statements)) {
      fprintf(stderr, "Type Checking Failed with %d errors.\n", checker.errorCount);
      freeTypeChecker(&checker);
      freeStmtList(statements);
      free(source);
      exit(65);
  }
  freeTypeChecker(&checker);
  printf("Type Check Passed.\n");

  // --- Pipeline Step 3: LLVM CodeGen ---
  printf("Generating LLVM IR...\n");
  generateCode(statements);

  // Free resources
  freeStmtList(statements);
  free(source);
}


int main(int argc, const char *argv[]) {
  // Initialize VM
  VM vm;
  initVM(&vm);

  // Register standard library
  registerStdLib(&vm);

  if (argc == 1) {
    // REPL mode
    repl(&vm);
  } else if (argc == 2) {
    // File execution mode
    runFile(&vm, argv[1]);
  } else if (argc >= 3) {
    // Handle subcommands
    const char *command = argv[1];

    if (strcmp(command, "run") == 0) {
      runFile(&vm, argv[2]);
    } else if (strcmp(command, "build") == 0) {
      printf("Build command not yet implemented\n");
      exit(1);
    } else if (argv[1][strlen(argv[1]) - 1] == 'x' &&
               argv[1][strlen(argv[1]) - 2] == 'o' &&
               argv[1][strlen(argv[1]) - 3] == 'r' &&
               argv[1][strlen(argv[1]) - 4] == 'p' &&
               argv[1][strlen(argv[1]) - 5] == '.') {
      // File with .prox extension
      runFile(&vm, argv[1]);
    } else {
      fprintf(stderr, "Usage: prox [path]\n");
      fprintf(stderr, "       prox run [path]\n");
      fprintf(stderr, "       prox          (REPL mode)\n");
      exit(64);
    }
  }

  // Cleanup
  freeVM(&vm);

  return 0;
}
