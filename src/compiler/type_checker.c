// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include "type_checker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Memory & Helper Functions ---

static TypeInfo createType(TypeKind kind) {
    TypeInfo t;
    t.kind = kind;
    t.name = NULL;
    t.returnType = NULL;
    t.paramTypes = NULL;
    t.paramCount = 0;
    return t;
}

static bool isTypesEqual(TypeInfo t1, TypeInfo t2) {
    if (t1.kind != t2.kind) return false;
    // For structs/classes, check name
    if (t1.kind == TYPE_CLASS) {
        if (t1.name && t2.name) return strcmp(t1.name, t2.name) == 0;
    }
    // Deep check for functions could go here
    return true;
}


// --- Symbol Table Helpers ---

static Scope* beginScope(TypeChecker* checker) {
    Scope* scope = (Scope*)malloc(sizeof(Scope));
    if (!scope) {
        fprintf(stderr, "Fatal: Out of memory for scope.\n");
        exit(1);
    }
    memset(scope->table, 0, sizeof(scope->table));
    scope->parent = checker->currentScope;
    checker->currentScope = scope;
    return scope;
}

static void endScope(TypeChecker* checker) {
    Scope* scope = checker->currentScope;
    checker->currentScope = scope->parent;
    
    // Free symbols in this scope
    for (int i = 0; i < TABLE_SIZE; i++) {
        Symbol* sym = scope->table[i];
        while (sym) {
            Symbol* next = sym->next;
            // TypeInfo might have allocated members, but for now we assume AST ownership
            // or simple types. If we alloc param arrays, we should free them here.
            if (sym->type.paramTypes) free(sym->type.paramTypes);
            free(sym->type.returnType);
            free(sym);
            sym = next;
        }
    }
    free(scope);
}

static unsigned int hash(const char* name) {
    unsigned int h = 2166136261u;
    for (; *name; name++) {
        h ^= *name;
        h *= 16777619;
    }
    return h % TABLE_SIZE;
}

static void defineSymbol(TypeChecker* checker, const char* name, TypeInfo type) {
    Scope* scope = checker->currentScope;
    unsigned int idx = hash(name);
    
    Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
    sym->name = (char*)name; // Weak reference
    sym->type = type;
    sym->next = scope->table[idx];
    scope->table[idx] = sym;
}

static TypeInfo lookupSymbol(TypeChecker* checker, const char* name) {
    Scope* scope = checker->currentScope;
    unsigned int idx = hash(name);
    
    while (scope) {
        Symbol* sym = scope->table[idx];
        while (sym) {
            if (strcmp(sym->name, name) == 0) return sym->type;
            sym = sym->next;
        }
        scope = scope->parent;
    }
    
    return createType(TYPE_UNKNOWN);
}

// helpers exposed
TypeInfo resolveVariableType(TypeChecker* checker, const char* name) {
    return lookupSymbol(checker, name);
}

// --- Type Checking Logic ---

static void error(TypeChecker* checker, int line, const char* msg) {
    fprintf(stderr, "[Type Error] Line %d: %s\n", line, msg);
    checker->errorCount++;
}

static TypeInfo checkExpr(TypeChecker* checker, Expr* expr);
static void checkStmt(TypeChecker* checker, Stmt* stmt);

static TypeInfo checkBinary(TypeChecker* checker, Expr* expr) {
    TypeInfo l = checkExpr(checker, expr->as.binary.left);
    TypeInfo r = checkExpr(checker, expr->as.binary.right);

    const char* op = expr->as.binary.operator;
    
    // If operands are unknown, we can't strict check, so we propagate unknown or assume valid?
    // Let's be safe: if unknown, we can't guarantee safety, but usually in static analysis we warn.
    // Here we will allow it to proceed as Unknown to avoid cascading errors.
    // DEBUG:
    // printf("Binary '%s' L:%d R:%d at line %d\n", op, l.kind, r.kind, expr->line);
    
    if (l.kind == TYPE_UNKNOWN || r.kind == TYPE_UNKNOWN) {
        return createType(TYPE_UNKNOWN);
    }

    // Arithmetic: +, -, *, /, %
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || 
        strcmp(op, "*") == 0 || strcmp(op, "/") == 0) {
        
        if (l.kind == TYPE_INT && r.kind == TYPE_INT) return l; // Int + Int = Int
        if (l.kind == TYPE_FLOAT && r.kind == TYPE_FLOAT) return l; // Float + Float = Float
        
        // Promotion
        if ((l.kind == TYPE_INT && r.kind == TYPE_FLOAT) || 
            (l.kind == TYPE_FLOAT && r.kind == TYPE_INT)) {
            return createType(TYPE_FLOAT);
        }

        // String Concatenation
        if (strcmp(op, "+") == 0) {
            if (l.kind == TYPE_STRING && r.kind == TYPE_STRING) return l;
            // Maybe allow String + Int -> String?
            if (l.kind == TYPE_STRING || r.kind == TYPE_STRING) return createType(TYPE_STRING);
        }
        
        error(checker, expr->line, "Type mismatch in binary operation.");
        return createType(TYPE_UNKNOWN);
    }
    
    // Comparisons: <, >, <=, >=
    if (strcmp(op, "<") == 0 || strcmp(op, ">") == 0 || 
        strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0) {
        
        if (l.kind == TYPE_INT || l.kind == TYPE_FLOAT) {
            if (r.kind == TYPE_INT || r.kind == TYPE_FLOAT) {
                return createType(TYPE_BOOL);
            }
        }
        error(checker, expr->line, " Comparison operands must be numbers.");
        return createType(TYPE_BOOL); // Return bool to recover
    }

    // Equality: ==, !=
    if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
        if (!isTypesEqual(l, r)) {
            // "1" == 1 is often false in static strict
             // error(checker, expr->line, "Types must match for equality."); 
             // Actually, equality between different types is usually just 'false', not an error.
             // But strict typing often forbids it.
        }
        return createType(TYPE_BOOL);
    }

    return createType(TYPE_UNKNOWN);
}

static TypeInfo checkUnary(TypeChecker* checker, Expr* expr) {
    TypeInfo r = checkExpr(checker, expr->as.unary.right);
    const char* op = expr->as.unary.operator;

    if (strcmp(op, "!") == 0) {
        if (r.kind != TYPE_BOOL && r.kind != TYPE_UNKNOWN) {
            error(checker, expr->line, "Example error: '!' requires boolean operand.");
        }
        return createType(TYPE_BOOL);
    }
    if (strcmp(op, "-") == 0) {
        if (r.kind != TYPE_INT && r.kind != TYPE_FLOAT && r.kind != TYPE_UNKNOWN) {
            error(checker, expr->line, "Negation requires numeric operand.");
        }
        return r; // Returns same type (Int or Float)
    }
    return createType(TYPE_UNKNOWN);
}

static TypeInfo checkExpr(TypeChecker* checker, Expr* expr) {
    if (!expr) return createType(TYPE_VOID);

    TypeInfo result = createType(TYPE_UNKNOWN);
    
    switch (expr->type) {
        case EXPR_LITERAL: {
            // Need to inspect Value type
            Value v = expr->as.literal.value;
            if (IS_BOOL(v)) result = createType(TYPE_BOOL);
            else if (IS_NUMBER(v)) {
                // In this VM, numbers are doubles, but let's pretend we have Ints if whole number?
                // For now, always FLOAT to match VM.
                result = createType(TYPE_FLOAT);
            }
            else if (IS_NIL(v)) result = createType(TYPE_VOID);
            else if (IS_OBJ(v)) {
                // Assume string for now if ObjString
                result = createType(TYPE_STRING);
            }
            break;
        }
            
        case EXPR_BINARY:
            result = checkBinary(checker, expr);
            break;
            
        case EXPR_UNARY:
            result = checkUnary(checker, expr);
            break;

        case EXPR_GROUPING:
            result = checkExpr(checker, expr->as.grouping.expression);
            break;
            
        case EXPR_VARIABLE:
            result = lookupSymbol(checker, expr->as.variable.name);
            if (result.kind == TYPE_UNKNOWN) {
                // error(checker, expr->line, "Undefined variable."); 
                // Don't error if it's truly unknown (maybe from function param without type),
                // but usually lookup returns UNKNOWN if NOT FOUND.
                // We need to distinguish Not Found vs Unknown Type.
                // For this simple implementation, UNKNOWN means either.
            }
            break;

        case EXPR_ASSIGN: {
            TypeInfo varType = lookupSymbol(checker, expr->as.assign.name);
            TypeInfo valType = checkExpr(checker, expr->as.assign.value);
            
             if (varType.kind == TYPE_UNKNOWN) {
                 // If the variable is unknown, it might be:
                 // 1. Not declared (Error)
                 // 2. Declared but inferred as Unknown (Valid dynamic)
                 // Since we cannot distinguish easily without better symbol table API,
                 // and we want to allow dynamic types (Any), we will NOT error here.
                 // This fixes the bug where reassignment to inferred variables fails.
             } else {
                if (!isTypesEqual(varType, valType) && valType.kind != TYPE_UNKNOWN) {
                    if (!(varType.kind == TYPE_FLOAT && valType.kind == TYPE_INT)) {
                        error(checker, expr->line, "Type mismatch in assignment.");
                    }
                }
                // IFC Check: Tainted -> Pure Illegal
                if (valType.isTainted && !varType.isTainted) {
                    error(checker, expr->line, "Security Violation: Cannot assign tainted value to pure variable without sanitization.");
                }
            }
            result = valType;
            break;
        }

        case EXPR_CALL: {
             TypeInfo callee = checkExpr(checker, expr->as.call.callee);
             // Verify args
             // If callee is FUNCTION, check params
             // Currently AST might not have function types populated fully.
             ExprList* args = expr->as.call.arguments;
             if (args) {
                 for (int i=0; i<args->count; i++) {
                     checkExpr(checker, args->items[i]);
                 }
             }
             
             if (callee.kind == TYPE_FUNCTION || callee.kind == TYPE_CLASS) {
                 if (callee.returnType) {
                     result = *callee.returnType; // Copy return type
                 } else if (callee.kind == TYPE_CLASS) {
                     result = callee; // Constructor returns class instance
                 }
                 // TODO: Check argument types against callee.paramTypes
             } else if (callee.kind == TYPE_UNKNOWN) {
                 // Assume it works
             } else {
                 error(checker, expr->line, "Attempt to call non-function.");
             }
             break;
        }

        case EXPR_GET:
            checkExpr(checker, expr->as.get.object);
            result = createType(TYPE_UNKNOWN);
            break;

        case EXPR_SET:
            checkExpr(checker, expr->as.set.object);
            result = checkExpr(checker, expr->as.set.value);
            break;

        case EXPR_THIS:
            result = createType(TYPE_CLASS);
            break;

        case EXPR_NEW:
             checkExpr(checker, expr->as.new_expr.clazz);
             if (expr->as.new_expr.args) {
                 for(int i=0; i<expr->as.new_expr.args->count; i++) {
                     checkExpr(checker, expr->as.new_expr.args->items[i]);
                 }
             }
             result = createType(TYPE_CLASS);
             break;

        case EXPR_SANITIZE: {
            TypeInfo val = checkExpr(checker, expr->as.sanitize.value);
            result = val;
            result.isTainted = false; // Mark as pure
            break;
        }

        default:
            // Relaxed for others
            break;
    }
    
    expr->inferredType = result;
    return result;
}

static void checkStmt(TypeChecker* checker, Stmt* stmt) {
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_VAR_DECL: {
            TypeInfo declaredType = stmt->as.var_decl.type;
            
            // If initialized, check compatibility
            if (stmt->as.var_decl.initializer) {
                TypeInfo initType = checkExpr(checker, stmt->as.var_decl.initializer);
                
                if (declaredType.kind == TYPE_UNKNOWN) {
                    // Inference
                    declaredType = initType;
                    stmt->as.var_decl.type = initType; 
                } else {
                    if (!isTypesEqual(declaredType, initType) && initType.kind != TYPE_UNKNOWN) {
                        // Allow Int -> Float
                        if (!(declaredType.kind == TYPE_FLOAT && initType.kind == TYPE_INT)) {
                             error(checker, stmt->line, "Variable initializer type mismatch.");
                        }
                    }
                }
            } else {
                // No initializer. If type is unknown, that's an issue for static typing.
                if (declaredType.kind == TYPE_UNKNOWN) {
                     // error(checker, stmt->line, "Variable requires type or initializer.");
                     // Allow for now, will be TYPE_UNKNOWN (dynamic)
                }
            }
            
            
            // Temporal Check
            if (stmt->as.var_decl.isTemporal) {
                if (stmt->as.var_decl.ttl <= 0) {
                     error(checker, stmt->line, "Temporal variables must have a positive decay duration (TTL > 0).");
                }
            }

            defineSymbol(checker, stmt->as.var_decl.name, declaredType);
            break;
        }
        
        case STMT_FUNC_DECL: {
            // Function Name
             TypeInfo funcType = createType(TYPE_FUNCTION);
             // Setup signature
             
             // We need to capture return type if specified or inferred
             if (stmt->as.func_decl.returnType.kind != TYPE_UNKNOWN) {
                 funcType.returnType = (TypeInfo*)malloc(sizeof(TypeInfo));
                 *funcType.returnType = stmt->as.func_decl.returnType;
             } else {
                 // Will try to infer from body later?
                 // For recursion, we need to define symbol first.
                 // Let's assume Void if not specified? Or Unknown.
             }
             
             // Define function symbol in CURRENT scope (before entering body)
             defineSymbol(checker, stmt->as.func_decl.name, funcType);
             
             // Enter Body Scope
             beginScope(checker);
             
             // Define Parameters
             if (stmt->as.func_decl.params) {
                 StringList* params = stmt->as.func_decl.params;
                 for (int i=0; i < params->count; i++) {
                     // Params are unknown/dynamic for now as Parser doesn't have types
                     defineSymbol(checker, params->items[i], createType(TYPE_UNKNOWN));
                 }
             }
             
             checkStmt(checker, (Stmt*)stmt->as.func_decl.body); // Cast BlockStmt* to Stmt*? No, BlockStmt is a union member. 
             // StmtList* is what body is
             StmtList* body = stmt->as.func_decl.body;
             if (body) {
                 for(int i=0; i<body->count; i++) {
                     checkStmt(checker, body->items[i]);
                 }
             }

             endScope(checker);
             break;
        }

        case STMT_CLASS_DECL: {
            TypeInfo classType = createType(TYPE_CLASS);
            if (stmt->as.class_decl.name) {
                classType.name = strdup(stmt->as.class_decl.name);
                defineSymbol(checker, stmt->as.class_decl.name, classType);
            }
            
            // Methods
            StmtList* methods = stmt->as.class_decl.methods;
            if (methods) {
                // Technically should enter class scope or handle 'this'
                // For now, just check method bodies
                for (int i=0; i < methods->count; i++) {
                     checkStmt(checker, methods->items[i]);
                }
            }
            break;
        }

        case STMT_EXPRESSION:
            checkExpr(checker, stmt->as.expression.expression);
            break;

        case STMT_BLOCK:
            beginScope(checker);
            StmtList* list = stmt->as.block.statements;
            for (int i=0; i < list->count; i++) {
                checkStmt(checker, list->items[i]);
            }
            endScope(checker);
            break;
            
        case STMT_IF:
            checkExpr(checker, stmt->as.if_stmt.condition); 
            // Expect Bool?
            // if (condType.kind != TYPE_BOOL) ...
            checkStmt(checker, stmt->as.if_stmt.then_branch);
            if (stmt->as.if_stmt.else_branch) checkStmt(checker, stmt->as.if_stmt.else_branch);
            break;
        
        case STMT_WHILE:
            checkExpr(checker, stmt->as.while_stmt.condition);
            checkStmt(checker, stmt->as.while_stmt.body);
            break;

        case STMT_FOR:
            if (stmt->as.for_stmt.initializer) checkStmt(checker, stmt->as.for_stmt.initializer);
            if (stmt->as.for_stmt.condition) checkExpr(checker, stmt->as.for_stmt.condition);
            if (stmt->as.for_stmt.increment) checkExpr(checker, stmt->as.for_stmt.increment);
            checkStmt(checker, stmt->as.for_stmt.body);
            break;
            
        case STMT_PRINT:
            checkExpr(checker, stmt->as.print.expression);
            break;

        case STMT_RETURN: {
            TypeInfo retType;
            if (stmt->as.return_stmt.value) {
                retType = checkExpr(checker, stmt->as.return_stmt.value);
            } else {
                retType = createType(TYPE_VOID);
            }
            (void)retType; // Suppress unused variable warning until TODOs are implemented
            // TODO: Match validity against current function return type
            break;
        }

        case STMT_INTENT_DECL: {
            TypeInfo intentType = createType(TYPE_FUNCTION);
            if (stmt->as.intent_decl.returnType.kind != TYPE_UNKNOWN) {
                intentType.returnType = (TypeInfo*)malloc(sizeof(TypeInfo));
                *intentType.returnType = stmt->as.intent_decl.returnType;
            }
            // Define intent as a function-like symbol so calls work
            defineSymbol(checker, stmt->as.intent_decl.name, intentType);
            break;
        }

        case STMT_RESOLVER_DECL: {
            // Check if intent exists
            TypeInfo target = lookupSymbol(checker, stmt->as.resolver_decl.targetIntent);
            if (target.kind == TYPE_UNKNOWN) {
                 error(checker, stmt->line, "Resolver matches unknown intent.");
            }
            
            // Check body
            beginScope(checker);
            StmtList* body = stmt->as.resolver_decl.body;
            if (body) {
                for(int i=0; i<body->count; i++) {
                    checkStmt(checker, body->items[i]);
                }
            }
            endScope(checker);
            break;
        }

        case STMT_RESILIENT: {
            beginScope(checker);
            StmtList* body = stmt->as.resilient.body;
            if (body) {
                for(int i=0; i<body->count; i++) {
                     checkStmt(checker, body->items[i]);
                }
            }
            endScope(checker);

            if (stmt->as.resilient.recoveryBody) {
                beginScope(checker);
                StmtList* recBody = stmt->as.resilient.recoveryBody;
                for(int i=0; i<recBody->count; i++) {
                     checkStmt(checker, recBody->items[i]);
                }
                endScope(checker);
            }
            break;
        }

        case STMT_POLICY_DECL: {
             // Just verify body for now
             beginScope(checker);
             StmtList* rules = stmt->as.policy_decl.rules;
             if (rules) {
                 for(int i=0; i<rules->count; i++) {
                     checkStmt(checker, rules->items[i]);
                 }
             }
             endScope(checker);
             break;
        }

        default:
            break;
    }
}

// --- Public Entry Points ---

void initTypeChecker(TypeChecker *checker) {
    checker->errorCount = 0;
    checker->currentScope = NULL;
    beginScope(checker); // Global Scope
    
    // Define Builtins
    
    // clock() -> Float
    TypeInfo clockType = createType(TYPE_FUNCTION);
    clockType.returnType = (TypeInfo*)malloc(sizeof(TypeInfo));
    *clockType.returnType = createType(TYPE_FLOAT);
    defineSymbol(checker, "clock", clockType);

    // len(str) -> Float
    TypeInfo lenType = createType(TYPE_FUNCTION);
    lenType.returnType = (TypeInfo*)malloc(sizeof(TypeInfo));
    *lenType.returnType = createType(TYPE_FLOAT);
    defineSymbol(checker, "len", lenType);
}

bool checkTypes(TypeChecker *checker, StmtList *statements) {
    if (!statements) return true;
    
    for (int i = 0; i < statements->count; i++) {
        checkStmt(checker, statements->items[i]);
    }
    
    return checker->errorCount == 0;
}

void freeTypeChecker(TypeChecker *checker) {
    while (checker->currentScope) {
        endScope(checker);
    }
}
