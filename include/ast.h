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
    bool isTainted; 
};

// --- Node Types ---
typedef enum {
  EXPR_BINARY, EXPR_UNARY, EXPR_LITERAL, EXPR_GROUPING,
  EXPR_VARIABLE, EXPR_ASSIGN, EXPR_LOGICAL, EXPR_CALL,
  EXPR_GET, EXPR_SET, EXPR_INDEX, EXPR_SET_INDEX, EXPR_LIST,
  EXPR_DICTIONARY, EXPR_TERNARY, EXPR_LAMBDA,

  EXPR_AWAIT, EXPR_THIS, EXPR_SUPER, EXPR_NEW,
  EXPR_SANITIZE,
  EXPR_CRYPTO // Encrypt/Decrypt
} ExprType;

typedef enum {
  STMT_EXPRESSION, STMT_VAR_DECL, STMT_FUNC_DECL, STMT_CLASS_DECL, STMT_INTERFACE_DECL,
  STMT_USE_DECL, STMT_IF, STMT_WHILE, STMT_FOR, STMT_RETURN,
  STMT_BLOCK, STMT_BREAK, STMT_CONTINUE, STMT_SWITCH,
  STMT_TRY_CATCH, STMT_PRINT, STMT_EXTERN_DECL,
  STMT_INTENT_DECL, STMT_RESOLVER_DECL,

  STMT_RESILIENT,
  STMT_POLICY_DECL,
  STMT_NODE_DECL,

  STMT_DISTRIBUTED_DECL,

  STMT_MODEL_DECL,

  STMT_QUANTUM_BLOCK,
  STMT_GPU_BLOCK,
  STMT_VERIFY,
  STMT_TENSOR_DECL,
  STMT_CONTEXT_DECL,
  STMT_LAYER_DECL,
  STMT_ACTIVATE
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
typedef struct { Expr *value; } SanitizeExpr;
typedef struct { Expr *value; bool isEncrypt; } CryptoExpr; // isEncrypt=true (encrypt), false (decrypt)
typedef struct { Expr *callee; ExprList *arguments; } CallExpr;
typedef struct { Expr *object; char *name; } GetExpr;
typedef struct { Expr *object; char *name; Expr *value; } SetExpr;
typedef struct { Expr *target; Expr *index; } IndexExpr;
typedef struct { Expr *target; Expr *index; Expr *value; } SetIndexExpr;
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
  int line;
  int column;
  TypeInfo inferredType;
  union {
    BinaryExpr binary; UnaryExpr unary; LiteralExpr literal; GroupingExpr grouping;
    VariableExpr variable; AssignExpr assign; LogicalExpr logical;
    SanitizeExpr sanitize;
    CryptoExpr crypto;
    // ... struct pointers for others due to C union size limit usually
    struct { Expr *callee; ExprList *arguments; } call;
    struct { Expr *object; char *name; } get;
    SetExpr set; // SetExpr is not changed to anonymous struct in the instruction
    IndexExpr index; SetIndexExpr set_index; ListExpr list; DictionaryExpr dictionary;
    TernaryExpr ternary; LambdaExpr lambda; AwaitExpr await_expr;
    ThisExpr this_expr; SuperExpr super_expr; NewExpr new_expr;
  } as;
};

// --- Statement Data Structures ---
typedef struct { Expr *expression; } ExpressionStmt;
typedef struct { char *name; Expr *initializer; TypeInfo type; bool is_const; bool isTemporal; int ttl; } VarDeclStmt;
typedef struct { char *name; StringList *params; StmtList *body; TypeInfo returnType; bool isAsync; AccessLevel access; bool isStatic; bool isAbstract; Expr *contextCondition; } FuncDeclStmt;
typedef struct { char *name; Expr *superclass; StringList *interfaces; StmtList *methods; } ClassDeclStmt;
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
typedef struct { char *name; StringList *params; TypeInfo returnType; } IntentDeclStmt;
typedef struct { char *name; char *targetIntent; StmtList *body; } ResolverDeclStmt;

typedef struct { StmtList *body; char *strategy; int retryCount; StmtList *recoveryBody; } ResilientStmt;
typedef struct { char *policyName; char *target; StmtList *rules; } PolicyDeclStmt;
typedef struct { char *name; StringList *capabilities; } NodeDeclStmt;
typedef struct { char *name; StmtList *fields; } DistributedDeclStmt; 


typedef struct { char *name; char *architecture; StmtList *body; } ModelDeclStmt;
typedef struct { StmtList *body; } QuantumBlockStmt;
typedef struct { char *kernelName; StmtList *body; } GPUBlockStmt;
typedef struct { char *identityName; StmtList *body; } VerifyStmt; // verify identity <name> { ... }

typedef struct { char *name; char *dataType; int *dims; int dimCount; Expr *initializer; } TensorDeclStmt;

typedef struct { char *name; StmtList *layers; } ContextDeclStmt;
typedef struct { char *name; StmtList *methods; } LayerDeclStmt;
typedef struct { char *contextName; StmtList *body; } ActivateStmt;

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
    IntentDeclStmt intent_decl; ResolverDeclStmt resolver_decl;

    ResilientStmt resilient;
    PolicyDeclStmt policy_decl;
    NodeDeclStmt node_decl;
    DistributedDeclStmt distributed_decl;

    ModelDeclStmt model_decl;
    QuantumBlockStmt quantum_block;
    GPUBlockStmt gpu_block;
    VerifyStmt verify_stmt;
    TensorDeclStmt tensor_decl;
    ContextDeclStmt context_decl;
    LayerDeclStmt layer_decl;
    ActivateStmt activate_stmt;
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
Expr *createSetIndexExpr(Expr *target, Expr *index, Expr *value, int line, int column);
Expr *createListExpr(ExprList *elements, int line, int column);
Expr *createDictionaryExpr(DictPairList *pairs, int line, int column);
Expr *createTernaryExpr(Expr *cond, Expr *true_br, Expr *false_br, int line, int column);
Expr *createLambdaExpr(StringList *params, StmtList *body, int line, int column);
Expr *createAwaitExpr(Expr *expression, int line, int column);
Expr *createThisExpr(int line, int column);
Expr *createSuperExpr(const char *method, int line, int column);
Expr *createNewExpr(Expr *clazz, ExprList *args, int line, int column);
Expr *createSanitizeExpr(Expr *value, int line, int column); // Added prototype
Expr *createCryptoExpr(Expr *val, bool isEncrypt, int line, int column);

Stmt *createExpressionStmt(Expr *expression, int line, int column);
Stmt *createVarDeclStmt(const char *name, Expr *init, bool is_const, bool isTemporal, int ttl, int line, int column);
Stmt *createFuncDeclStmt(const char *name, StringList *params, StmtList *body, bool isAsync, AccessLevel access, bool isStatic, bool isAbstract, Expr *contextCondition, int line, int column);
Stmt *createClassDeclStmt(const char *name, Expr *super, StringList *interfaces, StmtList *methods, int line, int column);
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
Stmt *createIntentDeclStmt(const char *name, StringList *params, TypeInfo returnType, int line, int column);
Stmt *createResolverDeclStmt(const char *name, const char *targetIntent, StmtList *body, int line, int column);
Stmt *createResilientStmt(StmtList *body, const char *strategy, int retryCount, StmtList *recoveryBody, int line, int column);
Stmt *createPolicyDeclStmt(const char *policyName, const char *target, StmtList *rules, int line, int column);
Stmt *createNodeDeclStmt(const char *name, StringList *capabilities, int line, int column);
Stmt *createDistributedDeclStmt(const char *name, StmtList *fields, int line, int column);

Stmt *createModelDeclStmt(const char *name, const char *architecture, StmtList *body, int line, int column);
Stmt *createQuantumBlockStmt(StmtList *body, int line, int column);
Stmt *createGPUBlockStmt(const char *kernelName, StmtList *body, int line, int column);
Stmt *createVerifyStmt(const char *identityName, StmtList *body, int line, int column);
Stmt *createTensorDeclStmt(const char *name, const char *dataType, int *dims, int dimCount, Expr *initializer, int line, int column);
Stmt *createContextDeclStmt(const char *name, StmtList *layers, int line, int column);
Stmt *createLayerDeclStmt(const char *name, StmtList *methods, int line, int column);
Stmt *createActivateStmt(const char *contextName, StmtList *body, int line, int column);

ExprList *createExprList();
void appendExpr(ExprList *list, Expr *expr);

// ...
Expr *createSanitizeExpr(Expr *value, int line, int column); // Added prototype

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
