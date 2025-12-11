/*
 * Type Checker Header
 */

#ifndef PROX_TYPE_CHECKER_H
#define PROX_TYPE_CHECKER_H

#include "ast.h"
#include "common.h"


typedef struct {
  int errorCount;
} TypeChecker;

void initTypeChecker(TypeChecker *checker);
bool checkTypes(TypeChecker *checker, StmtList *statements);
void freeTypeChecker(TypeChecker *checker);

#endif
