// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#ifndef PROX_COMPILER_H
#define PROX_COMPILER_H

#include "bytecode.h"
#include "vm.h"


// Parser forward declaration removed to avoid conflict with parser.h

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

ObjFunction* compile(const char *source);

#include "ast.h"
<<<<<<< HEAD
bool generateBytecode(StmtList* statements, ObjFunction* function);
=======
void generateBytecode(StmtList* statements, Chunk* chunk);
>>>>>>> fix-ci-build

void markCompilerRoots();

#endif
