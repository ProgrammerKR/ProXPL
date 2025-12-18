// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/*
 * Type Checker Header
 * -------------------
 * Responsible for static analysis, type inference, and validation.
 */

#ifndef PROX_TYPE_CHECKER_H
#define PROX_TYPE_CHECKER_H

#include "ast.h"
#include "common.h"

// --- Symbol Table for Scoping ---
typedef struct Symbol {
    char* name;
    TypeInfo type;
    struct Symbol* next; // Hash collision chain
} Symbol;

#define TABLE_SIZE 256

typedef struct Scope {
    Symbol* table[TABLE_SIZE];
    struct Scope* parent;
} Scope;

typedef struct {
    int errorCount;
    Scope* currentScope;
} TypeChecker;

// --- API ---
void initTypeChecker(TypeChecker *checker);
bool checkTypes(TypeChecker *checker, StmtList *statements);
void freeTypeChecker(TypeChecker *checker);

// --- Helpers exposed for other compiler types ---
TypeInfo resolveVariableType(TypeChecker* checker, const char* name);

#endif
