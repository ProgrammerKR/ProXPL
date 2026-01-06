#include <stdio.h>
#include <string.h>
#include "../include/error_report.h"

static void printLine(const char* source, int targetLine) {
  int currentLine = 1;
<<<<<<< HEAD

=======
  const char* lineStart = source;
>>>>>>> fix-ci-build
  const char* lineEnd = source;

  while (*lineEnd != '\0') {
    if (currentLine == targetLine) {
      const char* p = lineEnd;
      while (*p != '\n' && *p != '\0') p++;
      
      printf("%4d | %.*s\n", targetLine, (int)(p - lineEnd), lineEnd);
      return;
    }

    if (*lineEnd == '\n') currentLine++;
    lineEnd++;
  }
}

void reportCompileError(const char* source, Token token, const char* message) {
  fprintf(stderr, "\033[1;31merror\033[0m: %s\n", message);
  fprintf(stderr, "  --> line %d\n", token.line);
  fprintf(stderr, "     |\n");
  
  printLine(source, token.line);
  
  // Point to the token
  fprintf(stderr, "     | ");
  for (int i = 0; i < token.length; i++) fprintf(stderr, "^");
  fprintf(stderr, "\n     |\n");
}

void reportRuntimeError(const char* source, int line, const char* message) {
   fprintf(stderr, "\033[1;31mruntime error\033[0m: %s\n", message);
   if (source != NULL) {
     fprintf(stderr, "  --> line %d\n", line);
     fprintf(stderr, "     |\n");
     printLine(source, line);
     fprintf(stderr, "     |\n");
   } else {
     fprintf(stderr, "[line %d]\n", line);
   }
}
