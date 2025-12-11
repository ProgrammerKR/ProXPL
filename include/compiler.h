#ifndef PROX_COMPILER_H
#define PROX_COMPILER_H

#include "scanner.h"
#include "vm.h"


typedef struct {
  Token current;
  Token previous;
  bool hadError;
  bool panicMode;
  Scanner *scanner;
} Parser;

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT, // =
  PREC_OR,         // ||
  PREC_AND,        // &&
  PREC_EQUALITY,   // == !=
  PREC_COMPARISON, // < > <= >=
  PREC_TERM,       // + -
  PREC_FACTOR,     // * /
  PREC_UNARY,      // ! -
  PREC_CALL,       // . ()
  PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)(bool canAssign);

typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
} ParseRule;

bool compile(const char *source, Chunk *chunk);

#endif
