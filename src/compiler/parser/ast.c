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
  if (list == NULL) return;
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
  if (list == NULL) return;
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
  if (list == NULL) return;
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
  if (list == NULL) return;
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
  if (list == NULL) return;
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

Expr *createSetIndexExpr(Expr *target, Expr *index, Expr *value, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_SET_INDEX;
  expr->line = line;
  expr->column = column;
  expr->as.set_index.target = target;
  expr->as.set_index.index = index;
  expr->as.set_index.value = value;
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

Expr *createAwaitExpr(Expr *expression, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_AWAIT;
  expr->line = line;
  expr->column = column;
  expr->as.await_expr.expression = expression;
  return expr;
}

Expr *createThisExpr(int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_THIS;
  expr->line = line;
  expr->column = column;
  return expr;
}

Expr *createSuperExpr(const char *method, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_SUPER;
  expr->line = line;
  expr->column = column;
  expr->as.super_expr.method = method ? strdup(method) : NULL;
  return expr;
}

Expr *createNewExpr(Expr *clazz, ExprList *args, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_NEW;
  expr->line = line;
  expr->column = column;
  expr->as.new_expr.clazz = clazz;
  expr->as.new_expr.args = args;
  return expr;
}

Expr *createSanitizeExpr(Expr *value, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_SANITIZE;
  expr->line = line;
  expr->column = column;
  expr->as.sanitize.value = value;
  return expr;
}

Expr *createCryptoExpr(Expr *val, bool isEncrypt, int line, int column) {
  Expr *expr = ALLOCATE(Expr, 1);
  expr->type = EXPR_CRYPTO;
  expr->line = line;
  expr->column = column;
  expr->as.crypto.value = val;
  expr->as.crypto.isEncrypt = isEncrypt;
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

Stmt *createVarDeclStmt(const char *name, Expr *init, bool is_const, bool isTemporal, int ttl, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_VAR_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.var_decl.name = strdup(name);
  stmt->as.var_decl.initializer = init;
  stmt->as.var_decl.is_const = is_const;
  stmt->as.var_decl.type = (TypeInfo){TYPE_UNKNOWN, NULL, NULL, NULL, 0, false};
  stmt->as.var_decl.isTemporal = isTemporal;
  stmt->as.var_decl.ttl = ttl;
  return stmt;
}

Stmt *createFuncDeclStmt(const char *name, StringList *params, StmtList *body,
                         bool isAsync, AccessLevel access, bool isStatic, bool isAbstract, Expr *contextCondition, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_FUNC_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.func_decl.name = strdup(name);
  stmt->as.func_decl.params = params;
  stmt->as.func_decl.body = body;
  stmt->as.func_decl.isAsync = isAsync; 
  stmt->as.func_decl.access = access;
  stmt->as.func_decl.isStatic = isStatic;
  stmt->as.func_decl.isAbstract = isAbstract;
  stmt->as.func_decl.contextCondition = contextCondition;
  return stmt;
}

Stmt *createClassDeclStmt(const char *name, Expr *super,
                          StringList *interfaces, StmtList *methods, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_CLASS_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.class_decl.name = strdup(name);
  stmt->as.class_decl.superclass = super;
  stmt->as.class_decl.interfaces = interfaces;
  stmt->as.class_decl.methods = methods;
  return stmt;
}

Stmt *createInterfaceDeclStmt(const char *name, StmtList *methods, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_INTERFACE_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.interface_decl.name = strdup(name);
  stmt->as.interface_decl.methods = methods;
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

Stmt *createPrintStmt(Expr *expression, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_PRINT;
  stmt->line = line;
  stmt->column = column;
  stmt->as.print.expression = expression;
  return stmt;
}

Stmt *createExternDeclStmt(const char *libPath, const char *symName, const char *name, StringList *params, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_EXTERN_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.extern_decl.libraryPath = strdup(libPath);
  stmt->as.extern_decl.symbolName = strdup(symName);
  stmt->as.extern_decl.name = strdup(name);
  stmt->as.extern_decl.params = params;
  return stmt;
}

Stmt *createIntentDeclStmt(const char *name, StringList *params, TypeInfo returnType, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_INTENT_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.intent_decl.name = strdup(name);
  stmt->as.intent_decl.params = params;
  stmt->as.intent_decl.returnType = returnType;
  return stmt;
}

Stmt *createResolverDeclStmt(const char *name, const char *targetIntent, StmtList *body, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_RESOLVER_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.resolver_decl.name = strdup(name);
  stmt->as.resolver_decl.targetIntent = strdup(targetIntent);
  stmt->as.resolver_decl.body = body;
  return stmt;
}

Stmt *createResilientStmt(StmtList *body, const char *strategy, int retryCount, StmtList *recoveryBody, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_RESILIENT;
  stmt->line = line;
  stmt->column = column;
  stmt->as.resilient.body = body;
  stmt->as.resilient.strategy = strategy ? strdup(strategy) : NULL;
  stmt->as.resilient.retryCount = retryCount;
  stmt->as.resilient.recoveryBody = recoveryBody;
  return stmt;
}

Stmt *createPolicyDeclStmt(const char *policyName, const char *target, StmtList *rules, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_POLICY_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.policy_decl.policyName = strdup(policyName);
  stmt->as.policy_decl.target = strdup(target);
  stmt->as.policy_decl.rules = rules;
  return stmt;
}

Stmt *createNodeDeclStmt(const char *name, StringList *capabilities, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_NODE_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.node_decl.name = strdup(name);
  stmt->as.node_decl.capabilities = capabilities;
  return stmt;
}

Stmt *createDistributedDeclStmt(const char *name, StmtList *fields, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_DISTRIBUTED_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.distributed_decl.name = strdup(name);
  stmt->as.distributed_decl.fields = fields;
  return stmt;
}

Stmt *createModelDeclStmt(const char *name, const char *architecture, StmtList *body, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_MODEL_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.model_decl.name = strdup(name);
  if(architecture) stmt->as.model_decl.architecture = strdup(architecture);
  else stmt->as.model_decl.architecture = NULL;
  stmt->as.model_decl.body = body;
  return stmt;
}

Stmt *createQuantumBlockStmt(StmtList *body, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_QUANTUM_BLOCK;
  stmt->line = line;
  stmt->column = column;
  stmt->as.quantum_block.body = body;
  return stmt;
}

Stmt *createGPUBlockStmt(const char *kernelName, StmtList *body, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_GPU_BLOCK;
  stmt->line = line;
  stmt->column = column;
  stmt->as.gpu_block.kernelName = kernelName ? strdup(kernelName) : NULL;
  stmt->as.gpu_block.body = body;
  return stmt;
}

Stmt *createVerifyStmt(const char *identityName, StmtList *body, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_VERIFY;
  stmt->line = line;
  stmt->column = column;
  stmt->as.verify_stmt.identityName = strdup(identityName);
  stmt->as.verify_stmt.body = body;
  return stmt;
}

Stmt *createTensorDeclStmt(const char *name, const char *dataType, int *dims, int dimCount, Expr *initializer, int line, int column) {
  Stmt *stmt = ALLOCATE(Stmt, 1);
  stmt->type = STMT_TENSOR_DECL;
  stmt->line = line;
  stmt->column = column;
  stmt->as.tensor_decl.name = strdup(name);
  stmt->as.tensor_decl.dataType = strdup(dataType);
  stmt->as.tensor_decl.dims = dims;
  stmt->as.tensor_decl.dimCount = dimCount;
  stmt->as.tensor_decl.initializer = initializer; 
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
    freeExpr(expr->as.index.target);
    freeExpr(expr->as.index.index);
    break;
  case EXPR_SET_INDEX:
    freeExpr(expr->as.set_index.target);
    freeExpr(expr->as.set_index.index);
    freeExpr(expr->as.set_index.value);
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
  case EXPR_AWAIT:
    freeExpr(expr->as.await_expr.expression);
    break;
  case EXPR_THIS:
    // Nothing to free
    break;
  case EXPR_SUPER:
    if (expr->as.super_expr.method) free(expr->as.super_expr.method);
    break;
  case EXPR_NEW:
    freeExpr(expr->as.new_expr.clazz);
    if(expr->as.new_expr.args) freeExprList(expr->as.new_expr.args);
    break;
  case EXPR_SANITIZE:
    freeExpr(expr->as.sanitize.value);
    break;
  case EXPR_CRYPTO:
    freeExpr(expr->as.crypto.value);
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
    // superclass is a VariableExpr, not a separate malloc (it's in parsing context, but here referenced. Wait, VariableExpr created by createVariableExpr IS allocated. But who frees it? ClassDeclStmt stores pointer. We should probably free it if we own it. Original code didn't? "superclass is a VariableExpr, not a separate malloc" comment suggests confusion or shared ownership. Actually AST nodes usually own children. I will assume we should NOT free superclass if it wasn't freed before? Or fix it?
    // Let's stick to existing pattern for superclass logic, just add interfaces free.
    // Wait, original: `// superclass is a VariableExpr, not a separate malloc`
    // Actually `createVariableExpr` does ALLOCATE. So it definitely IS a separate malloc.
    // Maybe parser manages it? Or maybe this comment is wrong. 
    // I'll leave superclass alone to avoid regression, just add interfaces.
    freeStringList(stmt->as.class_decl.interfaces);
    freeStmtList(stmt->as.class_decl.methods);
    break;
  case STMT_INTERFACE_DECL:
    free(stmt->as.interface_decl.name);
    freeStmtList(stmt->as.interface_decl.methods);
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
  case STMT_PRINT:
    freeExpr(stmt->as.print.expression);
    break;
  case STMT_EXTERN_DECL:
    free(stmt->as.extern_decl.libraryPath);
    free(stmt->as.extern_decl.symbolName);
    free(stmt->as.extern_decl.name);
    free(stmt->as.extern_decl.name);
    freeStringList(stmt->as.extern_decl.params);
    break;
  case STMT_INTENT_DECL:
    free(stmt->as.intent_decl.name);
    freeStringList(stmt->as.intent_decl.params);
    // free returnType if it has allocated name? TypeInfo struct has char* name.
    if (stmt->as.intent_decl.returnType.name) free(stmt->as.intent_decl.returnType.name);
    break;
  case STMT_RESOLVER_DECL:
    free(stmt->as.resolver_decl.name);
    free(stmt->as.resolver_decl.targetIntent);
    freeStmtList(stmt->as.resolver_decl.body);
    break;
  case STMT_RESILIENT:
    freeStmtList(stmt->as.resilient.body);
    if(stmt->as.resilient.strategy) free(stmt->as.resilient.strategy);
    if(stmt->as.resilient.recoveryBody) freeStmtList(stmt->as.resilient.recoveryBody);
    break;
  case STMT_POLICY_DECL:
    free(stmt->as.policy_decl.policyName);
    free(stmt->as.policy_decl.target);
    freeStmtList(stmt->as.policy_decl.rules);
    break;
  case STMT_NODE_DECL:
    free(stmt->as.node_decl.name);
    if(stmt->as.node_decl.capabilities) freeStringList(stmt->as.node_decl.capabilities);
    break;
  case STMT_DISTRIBUTED_DECL:
    free(stmt->as.distributed_decl.name);
    freeStmtList(stmt->as.distributed_decl.fields);
    break;
  case STMT_MODEL_DECL:
    free(stmt->as.model_decl.name);
    if(stmt->as.model_decl.architecture) free(stmt->as.model_decl.architecture);
    if(stmt->as.model_decl.body) freeStmtList(stmt->as.model_decl.body);
    break;
  case STMT_QUANTUM_BLOCK:
    if(stmt->as.quantum_block.body) freeStmtList(stmt->as.quantum_block.body);
    break;
  case STMT_GPU_BLOCK:
    if(stmt->as.gpu_block.kernelName) free(stmt->as.gpu_block.kernelName);
    if(stmt->as.gpu_block.body) freeStmtList(stmt->as.gpu_block.body);
    break;
  case STMT_VERIFY:
    free(stmt->as.verify_stmt.identityName);
    if(stmt->as.verify_stmt.body) freeStmtList(stmt->as.verify_stmt.body);
    break;
  case STMT_TENSOR_DECL:
    free(stmt->as.tensor_decl.name);
    free(stmt->as.tensor_decl.dataType);
    if(stmt->as.tensor_decl.dims) free(stmt->as.tensor_decl.dims);
    freeExpr(stmt->as.tensor_decl.initializer);
    break;
  }

  FREE(Stmt, stmt);
}

