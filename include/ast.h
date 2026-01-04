// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#ifndef PROX_AST_H
#define PROX_AST_H

#include "common.h"
#include "value.h"

// --- Type System ---
typedef enum {
    TYPE_UNKNOWN = 0,
    TYPE_VOID,
    TYPE_BOOL,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_FUNCTION,
    TYPE_CLASS,
    TYPE_INTERFACE
} TypeKind;

struct TypeInfo {
    TypeKind kind;
    char* name;
    struct TypeInfo* returnType;
    struct TypeInfo* paramTypes; 
    int paramCount;
};

// --- Node Types ---
typedef enum {
  EXPR_BINARY, EXPR_UNARY, EXPR_LITERAL, EXPR_GROUPING,
  EXPR_VARIABLE, EXPR_ASSIGN, EXPR_LOGICAL, EXPR_CALL,
  EXPR_GET, EXPR_SET, EXPR_INDEX, EXPR_LIST,
  EXPR_DICTIONARY, EXPR_TERNARY, EXPR_LAMBDA,
  EXPR_AWAIT, EXPR_THIS, EXPR_SUPER, EXPR_NEW
} ExprType;

typedef enum {
  STMT_EXPRESSION, STMT_VAR_DECL, STMT_FUNC_DECL, STMT_CLASS_DECL, STMT_INTERFACE_DECL,
  STMT_USE_DECL, STMT_IF, STMT_WHILE, STMT_FOR, STMT_RETURN,
  STMT_BLOCK, STMT_BREAK, STMT_CONTINUE, STMT_SWITCH,
  STMT_TRY_CATCH, STMT_PRINT, STMT_EXTERN_DECL
} StmtType;

// --- List Structures ---
struct ExprList {
  Expr **items;
  int count;
  int capacity;
};

struct StmtList {
  Stmt **items;
  int count;
  int capacity;
};

struct StringList {
  char **items;
  int count;
  int capacity;
};

struct DictPair {
  Expr *key;
  Expr *value;
};

struct DictPairList {
  DictPair *items;
  int count;
  int capacity;
};

struct SwitchCase {
  Expr *value;
  StmtList *statements;
};

struct SwitchCaseList {
  SwitchCase *items;
  int count;
  int capacity;
};

// --- Expression Data Structures ---
typedef struct { Expr *left; char *operator; Expr *right; } BinaryExpr;
typedef struct { char *operator; Expr *right; } UnaryExpr;
typedef struct { Value value; } LiteralExpr;
typedef struct { Expr *expression; } GroupingExpr;
typedef struct { char *name; } VariableExpr;
typedef struct { char *name; Expr *value; } AssignExpr;
typedef struct { Expr *left; char *operator; Expr *right; } LogicalExpr;
typedef struct { Expr *callee; ExprList *arguments; } CallExpr;
typedef struct { Expr *object; char *name; } GetExpr;
typedef struct { Expr *object; char *name; Expr *value; } SetExpr;
typedef struct { Expr *target; Expr *index; } IndexExpr;
typedef struct { ExprList *elements; } ListExpr;
typedef struct { DictPairList *pairs; } DictionaryExpr;
typedef struct { Expr *condition; Expr *true_branch; Expr *false_branch; } TernaryExpr;
typedef struct { StringList *params; StmtList *body; } LambdaExpr;
typedef struct { Expr *expression; } AwaitExpr;
typedef struct { int dummy; } ThisExpr;
typedef struct { char *method; } SuperExpr; 
typedef struct { Expr *clazz; ExprList *args; } NewExpr;

struct Expr {
  ExprType type;
  TypeInfo inferredType; 
  int line;
  int column;
  union {
    BinaryExpr binary; UnaryExpr unary; LiteralExpr literal;
    GroupingExpr grouping; VariableExpr variable; AssignExpr assign;
    LogicalExpr logical; CallExpr call; GetExpr get; SetExpr set;
    IndexExpr index; ListExpr list; DictionaryExpr dictionary;
    TernaryExpr ternary; LambdaExpr lambda; AwaitExpr await_expr;
    ThisExpr this_expr; SuperExpr super_expr; NewExpr new_expr;
  } as;
};

// --- Statement Data Structures ---
typedef struct { Expr *expression; } ExpressionStmt;
typedef struct { char *name; Expr *initializer; TypeInfo type; bool is_const; } VarDeclStmt;
typedef struct { char *name; StringList *params; StmtList *body; TypeInfo returnType; bool isAsync; AccessLevel access; bool isStatic; bool isAbstract; } FuncDeclStmt;
typedef struct { char *name; VariableExpr *superclass; StringList *interfaces; StmtList *methods; } ClassDeclStmt;
typedef struct { char *name; StmtList *methods; } InterfaceDeclStmt;
typedef struct { StringList *modules; } UseDeclStmt;
typedef struct { Expr *condition; Stmt *then_branch; Stmt *else_branch; } IfStmt;
typedef struct { Expr *condition; Stmt *body; } WhileStmt;
typedef struct { Stmt *initializer; Expr *condition; Expr *increment; Stmt *body; } ForStmt;
typedef struct { Expr *value; } ReturnStmt;
typedef struct { StmtList *statements; } BlockStmt;
typedef struct { int dummy; } BreakStmt;
typedef struct { int dummy; } ContinueStmt;
typedef struct { Expr *value; SwitchCaseList *cases; StmtList *default_case; } SwitchStmt;
typedef struct { StmtList *try_block; char *catch_var; StmtList *catch_block; StmtList *finally_block; } TryCatchStmt;
typedef struct { Expr *expression; } PrintStmt;
typedef struct { char *libraryPath; char *symbolName; char *name; StringList *params; } ExternDeclStmt;

struct Stmt {
  StmtType type;
  int line;
  int column;
  union {
    ExpressionStmt expression; VarDeclStmt var_decl; FuncDeclStmt func_decl;
    ClassDeclStmt class_decl; InterfaceDeclStmt interface_decl; UseDeclStmt use_decl; IfStmt if_stmt;
    WhileStmt while_stmt; ForStmt for_stmt; ReturnStmt return_stmt;
    BlockStmt block; BreakStmt break_stmt; ContinueStmt continue_stmt;
    SwitchStmt switch_stmt; TryCatchStmt try_catch; PrintStmt print;
    ExternDeclStmt extern_decl;
  } as;
};

// --- Function Prototypes ---
Expr *createBinaryExpr(Expr *left, const char *op, Expr *right, int line, int column);
Expr *createUnaryExpr(const char *op, Expr *right, int line, int column);
Expr *createLiteralExpr(Value value, int line, int column);
Expr *createGroupingExpr(Expr *expression, int line, int column);
Expr *createVariableExpr(const char *name, int line, int column);
Expr *createAssignExpr(const char *name, Expr *value, int line, int column);
Expr *createLogicalExpr(Expr *left, const char *op, Expr *right, int line, int column);
Expr *createCallExpr(Expr *callee, ExprList *arguments, int line, int column);
Expr *createGetExpr(Expr *object, const char *name, int line, int column);
Expr *createSetExpr(Expr *object, const char *name, Expr *value, int line, int column);
Expr *createIndexExpr(Expr *target, Expr *index, int line, int column);
Expr *createListExpr(ExprList *elements, int line, int column);
Expr *createDictionaryExpr(DictPairList *pairs, int line, int column);
Expr *createTernaryExpr(Expr *cond, Expr *true_br, Expr *false_br, int line, int column);
Expr *createLambdaExpr(StringList *params, StmtList *body, int line, int column);
Expr *createAwaitExpr(Expr *expression, int line, int column);
Expr *createThisExpr(int line, int column);
Expr *createSuperExpr(const char *method, int line, int column);
Expr *createNewExpr(Expr *clazz, ExprList *args, int line, int column);

Stmt *createExpressionStmt(Expr *expression, int line, int column);
Stmt *createVarDeclStmt(const char *name, Expr *init, bool is_const, int line, int column);
Stmt *createFuncDeclStmt(const char *name, StringList *params, StmtList *body, bool isAsync, AccessLevel access, bool isStatic, bool isAbstract, int line, int column);
Stmt *createClassDeclStmt(const char *name, VariableExpr *super, StringList *interfaces, StmtList *methods, int line, int column);
Stmt *createInterfaceDeclStmt(const char *name, StmtList *methods, int line, int column);
Stmt *createUseDeclStmt(StringList *modules, int line, int column);
Stmt *createIfStmt(Expr *cond, Stmt *then_br, Stmt *else_br, int line, int column);
Stmt *createWhileStmt(Expr *cond, Stmt *body, int line, int column);
Stmt *createForStmt(Stmt *init, Expr *cond, Expr *incr, Stmt *body, int line, int column);
Stmt *createReturnStmt(Expr *value, int line, int column);
Stmt *createBlockStmt(StmtList *statements, int line, int column);
Stmt *createBreakStmt(int line, int column);
Stmt *createContinueStmt(int line, int column);
Stmt *createSwitchStmt(Expr *value, SwitchCaseList *cases, StmtList *def, int line, int column);
Stmt *createTryCatchStmt(StmtList *try_blk, const char *catch_var, StmtList *catch_blk, StmtList *finally_blk, int line, int column);
Stmt *createPrintStmt(Expr *expression, int line, int column);
Stmt *createExternDeclStmt(const char *libPath, const char *symName, const char *name, StringList *params, int line, int column);

ExprList *createExprList();
void appendExpr(ExprList *list, Expr *expr);
void freeExprList(ExprList *list);
StmtList *createStmtList();
void appendStmt(StmtList *list, Stmt *stmt);
void freeStmtList(StmtList *list);
StringList *createStringList();
void appendString(StringList *list, const char *str);
void freeStringList(StringList *list);
DictPairList *createDictPairList();
void appendDictPair(DictPairList *list, Expr *key, Expr *value);
void freeDictPairList(DictPairList *list);
SwitchCaseList *createSwitchCaseList();
void appendSwitchCase(SwitchCaseList *list, Expr *value, StmtList *statements);
void freeSwitchCaseList(SwitchCaseList *list);

void freeExpr(Expr *expr);
void freeStmt(Stmt *stmt);

#endif // PROX_AST_H
