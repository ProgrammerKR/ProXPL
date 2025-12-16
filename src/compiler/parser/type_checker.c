// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/*
 * Type Checker - STUB IMPLEMENTATION
 *
 * Full implementation would include:
 * - Type inference
 * - Type compatibility checking
 * - Generic type support
 * - Error reporting with line numbers
 */

#include "type_checker.h"
#include <stdio.h>

void initTypeChecker(TypeChecker *checker) { checker->errorCount = 0; }

bool checkTypes(TypeChecker *checker, StmtList *statements) {
  // STUB: Always passes type checking
  printf("[TypeChecker] Type checking skipped (stub implementation)\n");
  return true;
}

void freeTypeChecker(TypeChecker *checker) {
  // Nothing to free in stub
}
