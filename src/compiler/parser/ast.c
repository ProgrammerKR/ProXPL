// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include "ast.h"
#include "memory.h"
#include <stdlib.h>
#include <string.h>

// --- List Management Functions ---

ExprList *createExprList() {
  ExprList *list = ALLOCATE(ExprList, 1);
  list->items = NULL;
  list->count = 0;
  list->capacity = 0;
  return list;
}

void appendExpr(ExprList *list, Expr *expr) {
  if (list->capacity < list->count + 1) {
    int oldCap = list->capacity;
    list->capacity = GROW_CAPACITY(oldCap);
    list->items = GROW_ARRAY(Expr *, list->items, oldCap, list->capacity);
  }
  list->items[list->count++] = expr;
}

void freeExprList(ExprList *list) {
  for (int i = 0; i < list->count; i++) {
    freeExpr(list->items[i]);
  }
  FREE_ARRAY(Expr *, list->items, list->capacity);
  FREE(ExprList, list);
}

StmtList *createStmtList() {
  StmtList *list = ALLOCATE(StmtList, 1);
  list->items = NULL;
  list->count = 0;
  list->capacity = 0;
  return list;
}

void appendStmt(StmtList *list, Stmt *stmt) {
  if (list->capacity < list->count + 1) {
    int oldCap = list->capacity;
    list->capacity = GROW_CAPACITY(oldCap);
    list->items = GROW_ARRAY(Stmt *, list->items, oldCap, list->capacity);
  }
  list->items[list->count++] = stmt;
}

void freeStmtList(StmtList *list) {
  for (int i = 0; i < list->count; i++) {
    freeStmt(list->items[i]);
  }
  FREE_ARRAY(Stmt *, list->items, list->capacity);
  FREE(StmtList, list);
}

StringList *createStringList() {
  StringList *list = ALLOCATE(StringList, 1);
  list->items = NULL;
  list->count = 0;
  list->capacity = 0;
  return list;
}

void appendString(StringList *list, const char *str) {
  if (list->capacity < list->count + 1) {
    int oldCap = list->capacity;
    list->capacity = GROW_CAPACITY(oldCap);
    list->items = GROW_ARRAY(char *, list->items, oldCap, list->capacity);
  }
  list->items[list->count++] = strdup(str);
}

void freeStringList(StringList *list) {
  for (int i = 0; i < list->count; i++) {
    free(list->items[i]);
  }
  FREE_ARRAY(char *, list->items, list->capacity);
  FREE(StringList, list);
}

DictPairList *createDictPairList() {
  DictPairList *list = ALLOCATE(DictPairList, 1);
  list->items = NULL;
  list->count = 0;
  list->capacity = 0;
  return list;
}

void appendDictPair(DictPairList *list, Expr *key, Expr *value) {
  if (list->capacity < list->count + 1) {
    int oldCap = list->capacity;
    list->capacity = GROW_CAPACITY(oldCap);
    list->items = GROW_ARRAY(DictPair, list->items, oldCap, list->capacity);
  }
  list->items[list->count].key = key;
  list->items[list->count].value = value;
  list->count++;
}

void freeDictPairList(DictPairList *list) {
  for (int i = 0; i < list->count; i++) {
    freeExpr(list->items[i].key);
    freeExpr(list->items[i].value);
  }
  FREE_ARRAY(DictPair, list->items, list->capacity);
  FREE(DictPairList, list);
}

SwitchCaseList *createSwitchCaseList() {
  SwitchCaseList *list = ALLOCATE(SwitchCaseList, 1);
  list->items = NULL;
  list->count = 0;
  list->capacity = 0;
  return list;
}

void appendSwitchCase(SwitchCaseList *list, Expr *value, StmtList *statements) {
  if (list->capacity < list->count + 1) {
    int oldCap = list->capacity;
    list->capacity = GROW_CAPACITY(oldCap);
    list->items = GROW_ARRAY(SwitchCase, list->items, oldCap, list->capacity);
  }
  list->items[list->count].value = value;
  list->items[list->count].statements = statements;
  list->count++;
}

void freeSwitchCaseList(SwitchCaseList *list) {
  for (int i = 0; i < list->count; i++) {
    freeExpr(list->items[i].value);
    freeStmtList(list->items[i].statements);
  }
  FREE_ARRAY(SwitchCase, list->items, list->capacity);
  FREE(SwitchCaseList, list);
}

// --- Expression Creation Functions ---

Expr *createBinaryExpr(Expr *left, const char *op, Expr *right, int line,
                       int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_BINARY;
  expr->line = line;
  expr->column = column;
  expr->as.binary.left = left;
  expr->as.binary.operator = strdup(op);
  expr->as.binary.right = right;
  return expr;
}

Expr *createUnaryExpr(const char *op, Expr *right, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_UNARY;
  expr->line = line;
  expr->column = column;
  expr->as.unary.operator = strdup(op);
  expr->as.unary.right = right;
  return expr;
}

Expr *createLiteralExpr(Value value, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_LITERAL;
  expr->line = line;
  expr->column = column;
  expr->as.literal.value = value;
  return expr;
}

Expr *createGroupingExpr(Expr *expression, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_GROUPING;
  expr->line = line;
  expr->column = column;
  expr->as.grouping.expression = expression;
  return expr;
}

Expr *createVariableExpr(const char *name, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_VARIABLE;
  expr->line = line;
  expr->column = column;
  expr->as.variable.name = strdup(name);
  return expr;
}

Expr *createAssignExpr(const char *name, Expr *value, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_ASSIGN;
  expr->line = line;
  expr->column = column;
  expr->as.assign.name = strdup(name);
  expr->as.assign.value = value;
  return expr;
}

Expr *createLogicalExpr(Expr *left, const char *op, Expr *right, int line,
                        int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_LOGICAL;
  expr->line = line;
  expr->column = column;
  expr->as.logical.left = left;
  expr->as.logical.operator = strdup(op);
  expr->as.logical.right = right;
  return expr;
}

Expr *createCallExpr(Expr *callee, ExprList *arguments, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_CALL;
  expr->line = line;
  expr->column = column;
  expr->as.call.callee = callee;
  expr->as.call.arguments = arguments;
  return expr;
}

Expr *createGetExpr(Expr *object, const char *name, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_GET;
  expr->line = line;
  expr->column = column;
  expr->as.get.object = object;
  expr->as.get.name = strdup(name);
  return expr;
}

Expr *createSetExpr(Expr *object, const char *name, Expr *value, int line,
                    int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_SET;
  expr->line = line;
  expr->column = column;
  expr->as.set.object = object;
  expr->as.set.name = strdup(name);
  expr->as.set.value = value;
  return expr;
}

Expr *createIndexExpr(Expr *target, Expr *index, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_INDEX;
  expr->line = line;
  expr->column = column;
  expr->as.index.target = target;
  expr->as.index.index = index;
  return expr;
}

Expr *createListExpr(ExprList *elements, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_LIST;
  expr->line = line;
  expr->column = column;
  expr->as.list.elements = elements;
  return expr;
}

Expr *createDictionaryExpr(DictPairList *pairs, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_DICTIONARY;
  expr->line = line;
  expr->column = column;
  expr->as.dictionary.pairs = pairs;
  return expr;
}

Expr *createTernaryExpr(Expr *cond, Expr *true_br, Expr *false_br, int line,
                        int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_TERNARY;
  expr->line = line;
  expr->column = column;
  expr->as.ternary.condition = cond;
  expr->as.ternary.true_branch = true_br;
  expr->as.ternary.false_branch = false_br;
  return expr;
}

Expr *createLambdaExpr(StringList *params, StmtList *body, int line,
                       int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_LAMBDA;
  expr->line = line;
  expr->column = column;
  expr->as.lambda.params = params;
  expr->as.lambda.body = body;
  return expr;
}

// --- Statement Creation Functions ---

Stmt *createExpressionStmt(Expr *expression, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_EXPRESSION;
  stmt->line = line;
  stmt->column = column;
  stmt->as.expression.expression = expression;
  return stmt;
}

Stmt *createVarDeclStmt(const char *name, Expr *init, bool is_const, int line,
                        int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_VAR_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.var_decl.name = strdup(name);
  stmt->as.var_decl.initializer = init;
  stmt->as.var_decl.is_const = is_const;
  return stmt;
}

Stmt *createFuncDeclStmt(const char *name, StringList *params, StmtList *body,
                         int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_FUNC_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.func_decl.name = strdup(name);
  stmt->as.func_decl.params = params;
  stmt->as.func_decl.body = body;
  return stmt;
}

Stmt *createClassDeclStmt(const char *name, VariableExpr *super,
                          StmtList *methods, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_CLASS_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.class_decl.name = strdup(name);
  stmt->as.class_decl.superclass = super;
  stmt->as.class_decl.methods = methods;
  return stmt;
}

Stmt *createUseDeclStmt(StringList *modules, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_USE_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.use_decl.modules = modules;
  return stmt;
}

Stmt *createIfStmt(Expr *cond, Stmt *then_br, Stmt *else_br, int line,
                   int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_IF;
  stmt->line = line;
  stmt->column = column;
  stmt->as.if_stmt.condition = cond;
  stmt->as.if_stmt.then_branch = then_br;
  stmt->as.if_stmt.else_branch = else_br;
  return stmt;
}

Stmt *createWhileStmt(Expr *cond, Stmt *body, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_WHILE;
  stmt->line = line;
  stmt->column = column;
  stmt->as.while_stmt.condition = cond;
  stmt->as.while_stmt.body = body;
  return stmt;
}

Stmt *createForStmt(Stmt *init, Expr *cond, Expr *incr, Stmt *body, int line,
                    int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_FOR;
  stmt->line = line;
  stmt->column = column;
  stmt->as.for_stmt.initializer = init;
  stmt->as.for_stmt.condition = cond;
  stmt->as.for_stmt.increment = incr;
  stmt->as.for_stmt.body = body;
  return stmt;
}

Stmt *createReturnStmt(Expr *value, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_RETURN;
  stmt->line = line;
  stmt->column = column;
  stmt->as.return_stmt.value = value;
  return stmt;
}

Stmt *createBlockStmt(StmtList *statements, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_BLOCK;
  stmt->line = line;
  stmt->column = column;
  stmt->as.block.statements = statements;
  return stmt;
}

Stmt *createBreakStmt(int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_BREAK;
  stmt->line = line;
  stmt->column = column;
  return stmt;
}

Stmt *createContinueStmt(int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_CONTINUE;
  stmt->line = line;
  stmt->column = column;
  return stmt;
}

Stmt *createSwitchStmt(Expr *value, SwitchCaseList *cases, StmtList *def,
                       int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_SWITCH;
  stmt->line = line;
  stmt->column = column;
  stmt->as.switch_stmt.value = value;
  stmt->as.switch_stmt.cases = cases;
  stmt->as.switch_stmt.default_case = def;
  return stmt;
}

Stmt *createTryCatchStmt(StmtList *try_blk, const char *catch_var,
                         StmtList *catch_blk, StmtList *finally_blk, int line,
                         int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_TRY_CATCH;
  stmt->line = line;
  stmt->column = column;
  stmt->as.try_catch.try_block = try_blk;
  stmt->as.try_catch.catch_var = strdup(catch_var);
  stmt->as.try_catch.catch_block = catch_blk;
  stmt->as.try_catch.finally_block = finally_blk;
  return stmt;
}

// --- Free Functions ---

void freeExpr(Expr *expr) {
  if (!expr)
    return;

  switch (expr->type) {
  case EXPR_BINARY:
    freeExpr(expr->as.binary.left);
    free(expr->as.binary.operator);
    freeExpr(expr->as.binary.right);
    break;
  case EXPR_UNARY:
    free(expr->as.unary.operator);
    freeExpr(expr->as.unary.right);
    break;
  case EXPR_LITERAL:
    // Value is managed separately
    break;
  case EXPR_GROUPING:
    freeExpr(expr->as.grouping.expression);
    break;
  case EXPR_VARIABLE:
    free(expr->as.variable.name);
    break;
  case EXPR_ASSIGN:
    free(expr->as.assign.name);
    freeExpr(expr->as.assign.value);
    break;
  case EXPR_LOGICAL:
    freeExpr(expr->as.logical.left);
    free(expr->as.logical.operator);
    freeExpr(expr->as.logical.right);
    break;
  case EXPR_CALL:
    freeExpr(expr->as.call.callee);
    freeExprList(expr->as.call.arguments);
    break;
  case EXPR_GET:
    freeExpr(expr->as.get.object);
    free(expr->as.get.name);
    break;
  case EXPR_SET:
    freeExpr(expr->as.set.object);
    free(expr->as.set.name);
    freeExpr(expr->as.set.value);
    break;
  case EXPR_INDEX:
    freeExpr(expr -\u003eas.index.target);
    freeExpr(expr -\u003eas.index.index);

    break;
  case EXPR_LIST:
    freeExprList(expr->as.list.elements);
    break;
  case EXPR_DICTIONARY:
    freeDictPairList(expr->as.dictionary.pairs);
    break;
  case EXPR_TERNARY:
    freeExpr(expr->as.ternary.condition);
    freeExpr(expr->as.ternary.true_branch);
    freeExpr(expr->as.ternary.false_branch);
    break;
  case EXPR_LAMBDA:
    freeStringList(expr->as.lambda.params);
    freeStmtList(expr->as.lambda.body);
    break;
  }

  FREE(Expr, expr);
}

void freeStmt(Stmt *stmt) {
  if (!stmt)
    return;

  switch (stmt->type) {
  case STMT_EXPRESSION:
    freeExpr(stmt->as.expression.expression);
    break;
  case STMT_VAR_DECL:
    free(stmt->as.var_decl.name);
    freeExpr(stmt->as.var_decl.initializer);
    break;
  case STMT_FUNC_DECL:
    free(stmt->as.func_decl.name);
    freeStringList(stmt->as.func_decl.params);
    freeStmtList(stmt->as.func_decl.body);
    break;
  case STMT_CLASS_DECL:
    free(stmt->as.class_decl.name);
    // superclass is a VariableExpr, not a separate malloc
    freeStmtList(stmt->as.class_decl.methods);
    break;
  case STMT_USE_DECL:
    freeStringList(stmt->as.use_decl.modules);
    break;
  case STMT_IF:
    freeExpr(stmt->as.if_stmt.condition);
    freeStmt(stmt->as.if_stmt.then_branch);
    freeStmt(stmt->as.if_stmt.else_branch);
    break;
  case STMT_WHILE:
    freeExpr(stmt->as.while_stmt.condition);
    freeStmt(stmt->as.while_stmt.body);
    break;
  case STMT_FOR:
    freeStmt(stmt->as.for_stmt.initializer);
    freeExpr(stmt->as.for_stmt.condition);
    freeExpr(stmt->as.for_stmt.increment);
    freeStmt(stmt->as.for_stmt.body);
    break;
  case STMT_RETURN:
    freeExpr(stmt->as.return_stmt.value);
    break;
  case STMT_BLOCK:
    freeStmtList(stmt->as.block.statements);
    break;
  case STMT_BREAK:
  case STMT_CONTINUE:
    // No fields to free
    break;
  case STMT_SWITCH:
    freeExpr(stmt->as.switch_stmt.value);
    freeSwitchCaseList(stmt->as.switch_stmt.cases);
    freeStmtList(stmt->as.switch_stmt.default_case);
    break;
  case STMT_TRY_CATCH:
    freeStmtList(stmt->as.try_catch.try_block);
    free(stmt->as.try_catch.catch_var);
    freeStmtList(stmt->as.try_catch.catch_block);
    freeStmtList(stmt->as.try_catch.finally_block);
    break;
  }

  FREE(Stmt, stmt);
}
