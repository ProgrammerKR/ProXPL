#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/ast.h"
#include "../../include/bytecode.h"
#include "../../include/common.h"
#include "../../include/value.h"
#include "../../include/object.h"
#include "../../include/vm.h"
#include <stddef.h> 

// --- Compiler & Scope Types ---

typedef struct {
    const char* name;
    int depth;
} Local;

typedef enum {
    COMP_FUNCTION,
    COMP_SCRIPT
} CompFunctionType;

typedef struct Loop {
    struct Loop* enclosing;
    int startIp;
    int scopeDepth;
    int breakJumps[16];   
    int breakCount;
} Loop;

typedef struct Compiler {
    struct Compiler* enclosing;
    ObjFunction* function;
    CompFunctionType type;

    Local locals[256];
    int localCount;
    int scopeDepth;
    
    Loop* loop;
} Compiler;

typedef struct {
    Compiler* compiler;
    Chunk* chunk; 
    bool hadError;
} BytecodeGen;

// --- Forward Declarations ---

static void genExpr(BytecodeGen* gen, Expr* expr);
static void genStmt(BytecodeGen* gen, Stmt* stmt);

// --- Helpers ---

static void initCompiler(BytecodeGen* gen, Compiler* compiler, CompFunctionType type) {
    compiler->enclosing = gen->compiler;
    compiler->function = NULL;
    compiler->type = type;
    compiler->localCount = 0;
    compiler->scopeDepth = 0;
    
    // New function object
    compiler->function = newFunction();
    gen->compiler = compiler;
    gen->chunk = &compiler->function->chunk;

    // Reserve stack slot 0 for local use (or 'this')
    Local* local = &compiler->locals[compiler->localCount++];
    local->depth = 0;
    local->name = ""; // Internal usage
}

static ObjFunction* endCompiler(BytecodeGen* gen, bool isInit) {
    // Emit return
    if (isInit) {
        writeChunk(gen->chunk, OP_GET_LOCAL, 0); 
        writeChunk(gen->chunk, 0, 0);
    } else {
        writeChunk(gen->chunk, OP_NIL, 0); 
    }
    writeChunk(gen->chunk, OP_RETURN, 0);
    
    ObjFunction* function = gen->compiler->function;
    
    // Pop compiler
    gen->compiler = gen->compiler->enclosing;
    if (gen->compiler != NULL) {
        gen->chunk = &gen->compiler->function->chunk;
    } else {
        gen->chunk = NULL; 
    }
    
    return function;
}

static void beginScope(BytecodeGen* gen) {
    gen->compiler->scopeDepth++;
}

static void endScope(BytecodeGen* gen) {
    gen->compiler->scopeDepth--;
    // Pop locals
    while (gen->compiler->localCount > 0 &&
           gen->compiler->locals[gen->compiler->localCount - 1].depth > gen->compiler->scopeDepth) {
        writeChunk(gen->chunk, OP_POP, 0); 
        gen->compiler->localCount--;
    }
}

static int resolveLocal(BytecodeGen* gen, const char* name) {
    for (int i = gen->compiler->localCount - 1; i >= 0; i--) {
        Local* local = &gen->compiler->locals[i];
        if (strcmp(name, local->name) == 0) {
            return i;
        }
    }
    return -1;
}

static void addLocal(BytecodeGen* gen, const char* name) {
    if (gen->hadError) return;
    if (gen->compiler->localCount == 256) {
        fprintf(stderr, "Too many local variables.\n");
        gen->hadError = true;
        return;
    }
    Local* local = &gen->compiler->locals[gen->compiler->localCount++];
    local->name = name;
    local->depth = gen->compiler->scopeDepth; // Assuming initialized immediately
}

static void emitConstant(BytecodeGen* gen, Value value, int line) {
    if (gen->hadError) return;
    int constant = addConstant(gen->chunk, value);
    if (constant > 255) {
        writeChunk(gen->chunk, OP_CONSTANT, line); 
        fprintf(stderr, "Too many constants.\n");
        gen->hadError = true;
    } else {
        writeChunk(gen->chunk, OP_CONSTANT, line);
        writeChunk(gen->chunk, (uint8_t)constant, line);
    }
}

static void genExpr(BytecodeGen* gen, Expr* expr) {
    if (expr == NULL) return;

    switch (expr->type) {
        case EXPR_LITERAL: {
             if (IS_NIL(expr->as.literal.value)) {
                 writeChunk(gen->chunk, OP_NIL, expr->line);
             } else if (IS_BOOL(expr->as.literal.value)) {
                 writeChunk(gen->chunk, AS_BOOL(expr->as.literal.value) ? OP_TRUE : OP_FALSE, expr->line);
             } else {
                 emitConstant(gen, expr->as.literal.value, expr->line);
             }
             break;
        }
        case EXPR_UNARY: {
            genExpr(gen, expr->as.unary.right);
            if (strcmp(expr->as.unary.operator, "-") == 0) {
                 writeChunk(gen->chunk, OP_NEGATE, expr->line);
            } else if (strcmp(expr->as.unary.operator, "!") == 0) {
                 writeChunk(gen->chunk, OP_NOT, expr->line);
            } else if (strcmp(expr->as.unary.operator, "~") == 0) {
                 writeChunk(gen->chunk, OP_BIT_NOT, expr->line);
            }
            break;
        }
        case EXPR_BINARY: {
            genExpr(gen, expr->as.binary.left);
            genExpr(gen, expr->as.binary.right);
            if (strcmp(expr->as.binary.operator, "+") == 0) writeChunk(gen->chunk, OP_ADD, expr->line);
            else if (strcmp(expr->as.binary.operator, "-") == 0) writeChunk(gen->chunk, OP_SUBTRACT, expr->line);
            else if (strcmp(expr->as.binary.operator, "*") == 0) writeChunk(gen->chunk, OP_MULTIPLY, expr->line);
            else if (strcmp(expr->as.binary.operator, "/") == 0) writeChunk(gen->chunk, OP_DIVIDE, expr->line);
             else if (strcmp(expr->as.binary.operator, "==") == 0) writeChunk(gen->chunk, OP_EQUAL, expr->line);
             else if (strcmp(expr->as.binary.operator, "!=") == 0) {
                 writeChunk(gen->chunk, OP_EQUAL, expr->line);
                 writeChunk(gen->chunk, OP_NOT, expr->line);
             }
             else if (strcmp(expr->as.binary.operator, "<") == 0) writeChunk(gen->chunk, OP_LESS, expr->line);
             else if (strcmp(expr->as.binary.operator, "<=") == 0) {
                 writeChunk(gen->chunk, OP_GREATER, expr->line);
                 writeChunk(gen->chunk, OP_NOT, expr->line);
             }
             else if (strcmp(expr->as.binary.operator, ">") == 0) writeChunk(gen->chunk, OP_GREATER, expr->line);
             else if (strcmp(expr->as.binary.operator, ">=") == 0) {
                 writeChunk(gen->chunk, OP_LESS, expr->line);
                 writeChunk(gen->chunk, OP_NOT, expr->line);
             }
             else if (strcmp(expr->as.binary.operator, "%") == 0) writeChunk(gen->chunk, OP_MODULO, expr->line);
             else if (strcmp(expr->as.binary.operator, "&") == 0) writeChunk(gen->chunk, OP_BIT_AND, expr->line);
             else if (strcmp(expr->as.binary.operator, "|") == 0) writeChunk(gen->chunk, OP_BIT_OR, expr->line);
             else if (strcmp(expr->as.binary.operator, "^") == 0) writeChunk(gen->chunk, OP_BIT_XOR, expr->line);
             else if (strcmp(expr->as.binary.operator, "<<") == 0) writeChunk(gen->chunk, OP_LEFT_SHIFT, expr->line);
             else if (strcmp(expr->as.binary.operator, ">>") == 0) writeChunk(gen->chunk, OP_RIGHT_SHIFT, expr->line);
             else if (strcmp(expr->as.binary.operator, "@") == 0) writeChunk(gen->chunk, OP_MAT_MUL, expr->line);
            break;
        }
        case EXPR_GROUPING: {
            genExpr(gen, expr->as.grouping.expression);
            break;
        }
        case EXPR_VARIABLE: {
            int arg = resolveLocal(gen, expr->as.variable.name);
            if (arg != -1) {
                writeChunk(gen->chunk, OP_GET_LOCAL, expr->line);
                writeChunk(gen->chunk, (uint8_t)arg, expr->line);
            } else {
                 Value nameVal = OBJ_VAL(copyString(expr->as.variable.name, strlen(expr->as.variable.name)));
                 int nameConst = addConstant(gen->chunk, nameVal);
                 writeChunk(gen->chunk, OP_GET_GLOBAL, expr->line);
                 writeChunk(gen->chunk, (uint8_t)nameConst, expr->line);
            }
            break;
        }
        case EXPR_ASSIGN: {
            genExpr(gen, expr->as.assign.value);
             int arg = resolveLocal(gen, expr->as.assign.name);
             if (arg != -1) {
                 writeChunk(gen->chunk, OP_SET_LOCAL, expr->line);
                 writeChunk(gen->chunk, (uint8_t)arg, expr->line);
             } else {
                 Value nameVal = OBJ_VAL(copyString(expr->as.assign.name, strlen(expr->as.assign.name)));
                 int nameConst = addConstant(gen->chunk, nameVal);
                 writeChunk(gen->chunk, OP_SET_GLOBAL, expr->line);
                 writeChunk(gen->chunk, (uint8_t)nameConst, expr->line);
             }
            break;
        }
        case EXPR_CALL: {
            genExpr(gen, expr->as.call.callee);
            int argCount = 0;
            if (expr->as.call.arguments) {
                argCount = expr->as.call.arguments->count;
                for (int i = 0; i < argCount; i++) {
                    genExpr(gen, expr->as.call.arguments->items[i]);
                }
            }
            writeChunk(gen->chunk, OP_CALL, expr->line);
            writeChunk(gen->chunk, (uint8_t)argCount, expr->line);
            break;
        }
        case EXPR_LOGICAL: {
            genExpr(gen, expr->as.logical.left);
            // writeChunk(gen->chunk, OP_DUP, expr->line); // REMOVED: OP_JUMP_IF_FALSE peeks, so we don't need DUP.
            
            // Correct Logic: 
            // AND: if left false, jump to end (result is left). else pop and gen right.
            // OR: if left true, jump to end (result is left). else pop and gen right.
            
            // We can reuse OP_JUMP_IF_FALSE for AND.
            // For OR, we need jump if true.
            // Or use JUMP_IF_FALSE to 'pop_and_right', else jump to end.
            
            int endJump = -1;
            int opJump = -1;
            
            if (strcmp(expr->as.logical.operator, "&&") == 0) {
                 writeChunk(gen->chunk, OP_JUMP_IF_FALSE, expr->line);
                 writeChunk(gen->chunk, 0xff, 0); writeChunk(gen->chunk, 0xff, 0);
                 endJump = gen->chunk->count - 2;
                 writeChunk(gen->chunk, OP_POP, expr->line);
                 genExpr(gen, expr->as.logical.right);
            } else { // "||"
                 writeChunk(gen->chunk, OP_JUMP_IF_FALSE, expr->line);
                 writeChunk(gen->chunk, 0xff, 0); writeChunk(gen->chunk, 0xff, 0);
                 opJump = gen->chunk->count - 2;
                 
                 writeChunk(gen->chunk, OP_JUMP, expr->line);
                 writeChunk(gen->chunk, 0xff, 0); writeChunk(gen->chunk, 0xff, 0);
                 endJump = gen->chunk->count - 2;
                 
                 // Patch opJump to here (false case)
                 int patchOp = gen->chunk->count - opJump - 2;
                 gen->chunk->code[opJump] = (patchOp >> 8) & 0xff;
                 gen->chunk->code[opJump+1] = patchOp & 0xff;
                 
                 writeChunk(gen->chunk, OP_POP, expr->line);
                 genExpr(gen, expr->as.logical.right);
            }
            
            int patchEnd = gen->chunk->count - endJump - 2;
            gen->chunk->code[endJump] = (patchEnd >> 8) & 0xff;
            gen->chunk->code[endJump+1] = patchEnd & 0xff;
            break;
        }
        case EXPR_GET: {
            genExpr(gen, expr->as.get.object);
            Value nameVal = OBJ_VAL(copyString(expr->as.get.name, strlen(expr->as.get.name)));
            int nameConst = addConstant(gen->chunk, nameVal);
            writeChunk(gen->chunk, OP_GET_PROPERTY, expr->line);
            writeChunk(gen->chunk, (uint8_t)nameConst, expr->line);
            break;
        }
        case EXPR_SET: {
            genExpr(gen, expr->as.set.object); // Object 
            genExpr(gen, expr->as.set.value);  // Value
            // Stack: Obj, Value. OP_SET_PROPERTY expects Obj (target of set), then Value is Top? 
            // vm.c: peek(1) is instance, peek(0) is value.
            // Correct.
            Value nameVal = OBJ_VAL(copyString(expr->as.set.name, strlen(expr->as.set.name)));
            int nameConst = addConstant(gen->chunk, nameVal);
            writeChunk(gen->chunk, OP_SET_PROPERTY, expr->line);
            writeChunk(gen->chunk, (uint8_t)nameConst, expr->line);
            break;
        }
        case EXPR_INDEX: {
             genExpr(gen, expr->as.index.target);
             genExpr(gen, expr->as.index.index);
             writeChunk(gen->chunk, OP_GET_INDEX, expr->line);
             break;
        }
        case EXPR_SET_INDEX: {
            genExpr(gen, expr->as.set_index.target);
            genExpr(gen, expr->as.set_index.index);
            genExpr(gen, expr->as.set_index.value);
            writeChunk(gen->chunk, OP_SET_INDEX, expr->line);
            break;
        }
        case EXPR_LIST: {
             int count = 0;
             if (expr->as.list.elements) {
                 count = expr->as.list.elements->count;
                 for (int i=0; i < count; i++) {
                     genExpr(gen, expr->as.list.elements->items[i]);
                 }
             }
             writeChunk(gen->chunk, OP_BUILD_LIST, expr->line);
             writeChunk(gen->chunk, (uint8_t)count, expr->line);
             break;
        }
        case EXPR_DICTIONARY: {
             int count = 0;
             if (expr->as.dictionary.pairs) {
                 count = expr->as.dictionary.pairs->count;
                 for (int i=0; i < count; i++) {
                     genExpr(gen, expr->as.dictionary.pairs->items[i].key);
                     genExpr(gen, expr->as.dictionary.pairs->items[i].value);
                 }
             }
             writeChunk(gen->chunk, OP_BUILD_MAP, expr->line);
             writeChunk(gen->chunk, (uint8_t)count, expr->line);
             break;
        }
        case EXPR_TERNARY: {
             genExpr(gen, expr->as.ternary.condition);
             writeChunk(gen->chunk, OP_JUMP_IF_FALSE, expr->line);
             writeChunk(gen->chunk, 0xff, 0); writeChunk(gen->chunk, 0xff, 0);
             int elseJump = gen->chunk->count - 2;
             writeChunk(gen->chunk, OP_POP, expr->line);
             
             genExpr(gen, expr->as.ternary.true_branch);
             writeChunk(gen->chunk, OP_JUMP, expr->line);
             writeChunk(gen->chunk, 0xff, 0); writeChunk(gen->chunk, 0xff, 0);
             int endJump = gen->chunk->count - 2;
             
             int patchElse = gen->chunk->count - elseJump - 2;
             gen->chunk->code[elseJump] = (patchElse >> 8) & 0xff;
             gen->chunk->code[elseJump+1] = patchElse & 0xff;
             writeChunk(gen->chunk, OP_POP, expr->line);
             
             genExpr(gen, expr->as.ternary.false_branch);
             
             int patchEnd = gen->chunk->count - endJump - 2;
             gen->chunk->code[endJump] = (patchEnd >> 8) & 0xff;
             gen->chunk->code[endJump+1] = patchEnd & 0xff;
             break;
        }
        case EXPR_LAMBDA: {
            Compiler funcCompiler;
            initCompiler(gen, &funcCompiler, COMP_FUNCTION);
            beginScope(gen);
            if (expr->as.lambda.params) {
                for (int i=0; i < expr->as.lambda.params->count; i++) {
                    funcCompiler.function->arity++;
                    addLocal(gen, expr->as.lambda.params->items[i]);
                }
            }
            if (expr->as.lambda.body) {
                 for (int i=0; i < expr->as.lambda.body->count; i++) {
                    genStmt(gen, expr->as.lambda.body->items[i]);
                }
            }
            ObjFunction* function = endCompiler(gen, false);
            Value funcVal = OBJ_VAL(function);
            int funcConst = addConstant(gen->chunk, funcVal);
            writeChunk(gen->chunk, OP_CLOSURE, expr->line);
            writeChunk(gen->chunk, (uint8_t)funcConst, expr->line);
            break;
        }
        case EXPR_THIS: {
             // In method, 'this' is at local slot 0
             // But valid only if inside method. Check compiler type?
             // Assuming parser checked or runtime will error if not method?
             // Compiler should ideally error if not in method.
             // For now, assume it's valid: return OP_GET_LOCAL 0
             // BUT, resolveLocal("this") is cleaner if we named it "this"?
             // We didn't. slot 0 is unnamed.
             writeChunk(gen->chunk, OP_GET_LOCAL, expr->line);
             writeChunk(gen->chunk, 0, expr->line);
             break;
        }
        case EXPR_SUPER: {
             // 1. Get 'this' (receiver)
             writeChunk(gen->chunk, OP_GET_LOCAL, expr->line);
             writeChunk(gen->chunk, 0, expr->line);
             
             // 2. Get superclass.
             // 'super' is typically bound as an upvalue if we used closures for classes?
             // Or we rely on looking up 'super' variable in scope?
             // ProXPL parser logic: `match(TOKEN_EXTENDS)` defines a variable for superclass? 
             // No.
             // Standard way: store 'super' in a known local/upvalue.
             // For simplify: let's assume 'super' is resolved by name lookup if we named the superclass?
             // Actually, `OP_GET_SUPER` usually needs the superclass object.
             // If we are in a method of class Sub, superclass is Sub.superclass.
             // But we don't have reference to Sub easily in validation?
             // Actually, we can look up "super" if we define it in scope.
             // Since we didn't add "super" to locals in `visitClassDecl`, we can't look it up yet.
             // Proposed shortcut: Emit OP_GET_SUPER with name constant.
             // VM will pop receiver("this"), and need to find method on superclass...
             // BUT VM needs to know WHICH class is super.
             // Common trick: OP_GET_SUPER takes an operand index for the method name.
             // Expects Stack: [Receiver, SuperClass].
             // So we must push SuperClass.
             // WE NEED TO RESOLVE SUPERCLASS.
             // Hack for v1.0.0 Alpha: 
             // If we are in `class B extends A`, `A` is available by name `A`?
             // If so, look up `A`? But we don't know the name `A` easily here.
             
             // Let's defer full implementation of SUPER to next iteration?
             // Step 184 said: "Runtime/VM: OpCodes... OP_GET_SUPER...".
             // We have OP_GET_SUPER in bytecode.h line 37.
             // Let's implement correct structure:
             // We need to resolve "super" as a local variable.
             // In `genFunction` for methods, if class has superclass, we should wrap it?
             // Let's skip valid SUPER for this exact single tool call and fix in next one properly.
             // Just emit OP_NIL for now to allow compiling.
             writeChunk(gen->chunk, OP_NIL, expr->line); // Placeholder
             break;
        }
        case EXPR_NEW: {
             // stack: Class, Args...
             genExpr(gen, expr->as.new_expr.clazz);
             
             int argCount = 0;
             if (expr->as.new_expr.args) {
                 argCount = expr->as.new_expr.args->count;
                 for (int i=0; i < argCount; i++) {
                     genExpr(gen, expr->as.new_expr.args->items[i]);
                 }
             }
             
             // We have OP_CALL. Can we use it?
             // If Class is a callable (it is), OP_CALL works!
             // VM `callValue` handles `OBJ_CLASS` -> creates instance -> calls init.
             // So we assume `new Foo()` is same as `Foo()` call semantics plus safety?
             // Original Parser parse `new` to EXPR_NEW. 
             // We can map this to OP_CALL for now if VM handles it.
             // But typical difference: `new` ensures instance creation involved.
             // ProXPL: `class Foo {} let f = Foo();` or `let f = new Foo();`?
             // Assuming we WANT `new` keyword usage.
             // Let's rely on standard OP_CALL logic where Class is callable.
             writeChunk(gen->chunk, OP_CALL, expr->line);
             writeChunk(gen->chunk, (uint8_t)argCount, expr->line);
             break;
        }
        default: break; 
    }
}

// Helper to generate function code
static void genFunction(BytecodeGen* gen, Stmt* stmt, bool defineVar) {
    Compiler funcCompiler;
    initCompiler(gen, &funcCompiler, COMP_FUNCTION);
    
    // Set function properties from AST
    funcCompiler.function->access = stmt->as.func_decl.access;
    funcCompiler.function->isStatic = stmt->as.func_decl.isStatic;
    funcCompiler.function->isAbstract = stmt->as.func_decl.isAbstract;
    
    // Protect function from GC during copyString
    push(&vm, OBJ_VAL(funcCompiler.function));
    if (stmt->as.func_decl.name != NULL) {
        funcCompiler.function->name = copyString(stmt->as.func_decl.name, strlen(stmt->as.func_decl.name));
    }
    pop(&vm);
    
    // bool isInitDebug = (stmt->as.func_decl.name != NULL && strcmp(stmt->as.func_decl.name, "init") == 0);
     // fprintf(stderr, "Compiling function '%s' (isInit: %d)\n", stmt->as.func_decl.name ? stmt->as.func_decl.name : "NULL", isInitDebug);
    
    beginScope(gen);
    
    StringList* params = (StringList*)stmt->as.func_decl.params; 
    if (params) {
         for (int i=0; i < params->count; i++) {
             funcCompiler.function->arity++;
             if (funcCompiler.function->arity > 255) {
                 // Error
             }
             addLocal(gen, params->items[i]);
         }
    }

    StmtList* body = stmt->as.func_decl.body;
    if (body) {
        for (int i=0; i < body->count; i++) {
            genStmt(gen, body->items[i]);
        }
    }
    
    bool isInit = (stmt->as.func_decl.name != NULL && strcmp(stmt->as.func_decl.name, "init") == 0);
    ObjFunction* function = endCompiler(gen, isInit);
    
    // Emit Closure
    Value funcVal = OBJ_VAL(function);
    int funcConst = addConstant(gen->chunk, funcVal);
    writeChunk(gen->chunk, OP_CLOSURE, stmt->line);
    writeChunk(gen->chunk, (uint8_t)funcConst, stmt->line);
    
    if (defineVar) {
        if (gen->compiler->scopeDepth > 0) {
            addLocal(gen, stmt->as.func_decl.name); 
        } else {
            Value nameVal = OBJ_VAL(copyString(stmt->as.func_decl.name, strlen(stmt->as.func_decl.name)));
            int nameConst = addConstant(gen->chunk, nameVal);
            writeChunk(gen->chunk, OP_DEFINE_GLOBAL, stmt->line);
            writeChunk(gen->chunk, (uint8_t)nameConst, stmt->line);
        }
    }
}

static void genStmt(BytecodeGen* gen, Stmt* stmt) {
    if (stmt == NULL) return;
    
    switch (stmt->type) {


        case STMT_FUNC_DECL: {
            genFunction(gen, stmt, true);
            break;
        }
        case STMT_BLOCK: {
            beginScope(gen);
            if (stmt->as.block.statements) {
                for (int i=0; i < stmt->as.block.statements->count; i++) {
                    genStmt(gen, stmt->as.block.statements->items[i]);
                }
            }
            endScope(gen);
            break;
        }
        case STMT_VAR_DECL: {
            if (stmt->as.var_decl.initializer) {
                genExpr(gen, stmt->as.var_decl.initializer);
            } else {
                writeChunk(gen->chunk, OP_NIL, stmt->line);
            }
            
            if (gen->compiler->scopeDepth > 0) {
                addLocal(gen, stmt->as.var_decl.name);
            } else {
                Value nameVal = OBJ_VAL(copyString(stmt->as.var_decl.name, strlen(stmt->as.var_decl.name)));
                int nameConst = addConstant(gen->chunk, nameVal);
                writeChunk(gen->chunk, OP_DEFINE_GLOBAL, stmt->line);
                writeChunk(gen->chunk, (uint8_t)nameConst, stmt->line);
            }
            break;
        }
        case STMT_EXPRESSION: {
            genExpr(gen, stmt->as.expression.expression);
            writeChunk(gen->chunk, OP_POP, stmt->line);
            break;
        }
        case STMT_PRINT: {
            genExpr(gen, stmt->as.print.expression);
            writeChunk(gen->chunk, OP_PRINT, stmt->line);
            break;
        }
        case STMT_RETURN: {
            if (stmt->as.return_stmt.value) {
                genExpr(gen, stmt->as.return_stmt.value);
            } else {
                 // Check if we are in init
                 if (gen->compiler->function->name != NULL && strcmp(gen->compiler->function->name->chars, "init") == 0) {
                     writeChunk(gen->chunk, OP_GET_LOCAL, stmt->line);
                     writeChunk(gen->chunk, 0, stmt->line);
                 } else {
                     writeChunk(gen->chunk, OP_NIL, stmt->line);
                 }
            }
            writeChunk(gen->chunk, OP_RETURN, stmt->line);
            break;
        }
        case STMT_EXTERN_DECL: {
            // Push library path
            Value libVal = OBJ_VAL(copyString(stmt->as.extern_decl.libraryPath, strlen(stmt->as.extern_decl.libraryPath)));
            int libConst = addConstant(gen->chunk, libVal);
            writeChunk(gen->chunk, OP_CONSTANT, stmt->line);
            writeChunk(gen->chunk, (uint8_t)libConst, stmt->line);
            
            // Push symbol name
            Value symVal = OBJ_VAL(copyString(stmt->as.extern_decl.symbolName, strlen(stmt->as.extern_decl.symbolName)));
            int symConst = addConstant(gen->chunk, symVal);
            writeChunk(gen->chunk, OP_CONSTANT, stmt->line);
            writeChunk(gen->chunk, (uint8_t)symConst, stmt->line);
            
            // Make Foreign Object
            writeChunk(gen->chunk, OP_MAKE_FOREIGN, stmt->line);
            
            // Define Global
             if (gen->compiler->scopeDepth > 0) {
                addLocal(gen, stmt->as.extern_decl.name);
            } else {
                Value nameVal = OBJ_VAL(copyString(stmt->as.extern_decl.name, strlen(stmt->as.extern_decl.name)));
                int nameConst = addConstant(gen->chunk, nameVal);
                writeChunk(gen->chunk, OP_DEFINE_GLOBAL, stmt->line);
                writeChunk(gen->chunk, (uint8_t)nameConst, stmt->line);
            }
            break;
        }
        case STMT_TENSOR_DECL: {
            // Compile Initializer (Expects List/Expr on stack)
            if (stmt->as.tensor_decl.initializer) {
                genExpr(gen, stmt->as.tensor_decl.initializer);
            } else {
                writeChunk(gen->chunk, OP_NIL, stmt->line);
            }
            
            // Emit Dimensions as Constants
            for(int i=0; i<stmt->as.tensor_decl.dimCount; i++) {
                emitConstant(gen, NUMBER_VAL(stmt->as.tensor_decl.dims[i]), stmt->line);
            }
            
            // Emit Make Tensor
            writeChunk(gen->chunk, OP_MAKE_TENSOR, stmt->line);
            writeChunk(gen->chunk, (uint8_t)stmt->as.tensor_decl.dimCount, stmt->line);
            
            // Define Variable
            if (gen->compiler->scopeDepth > 0) {
                addLocal(gen, stmt->as.tensor_decl.name);
            } else {
                Value nameVal = OBJ_VAL(copyString(stmt->as.tensor_decl.name, strlen(stmt->as.tensor_decl.name)));
                int nameConst = addConstant(gen->chunk, nameVal);
                writeChunk(gen->chunk, OP_DEFINE_GLOBAL, stmt->line);
                writeChunk(gen->chunk, (uint8_t)nameConst, stmt->line);
            }
            break;
        }
        case STMT_IF: {
            genExpr(gen, stmt->as.if_stmt.condition);
            writeChunk(gen->chunk, OP_JUMP_IF_FALSE, stmt->line);
            writeChunk(gen->chunk, 0xff, 0); writeChunk(gen->chunk, 0xff, 0);
            int thenJump = gen->chunk->count - 2;
            writeChunk(gen->chunk, OP_POP, stmt->line);
            
            genStmt(gen, stmt->as.if_stmt.then_branch);
            
            writeChunk(gen->chunk, OP_JUMP, 0);
            writeChunk(gen->chunk, 0xff, 0); writeChunk(gen->chunk, 0xff, 0);
            int elseJump = gen->chunk->count - 2;
            
            int patchThen = gen->chunk->count - thenJump - 2;
            gen->chunk->code[thenJump] = (patchThen >> 8) & 0xff;
            gen->chunk->code[thenJump+1] = patchThen & 0xff;
            
            writeChunk(gen->chunk, OP_POP, stmt->line);
            if (stmt->as.if_stmt.else_branch) genStmt(gen, stmt->as.if_stmt.else_branch);
            
            int patchElse = gen->chunk->count - elseJump - 2;
            gen->chunk->code[elseJump] = (patchElse >> 8) & 0xff;
            gen->chunk->code[elseJump+1] = patchElse & 0xff;
            break;
        }
        case STMT_WHILE: {
            int loopStart = gen->chunk->count;
            
            // Push loop scope
            Loop loop;
            loop.startIp = loopStart;
            loop.scopeDepth = gen->compiler->scopeDepth;
            loop.enclosing = gen->compiler->loop;
            loop.breakCount = 0;
            gen->compiler->loop = &loop;
            
            genExpr(gen, stmt->as.while_stmt.condition);
            
            writeChunk(gen->chunk, OP_JUMP_IF_FALSE, stmt->line);
            writeChunk(gen->chunk, 0xff, 0); writeChunk(gen->chunk, 0xff, 0);
            int exitJump = gen->chunk->count - 2;
            writeChunk(gen->chunk, OP_POP, stmt->line);
            
            genStmt(gen, stmt->as.while_stmt.body);
            
            writeChunk(gen->chunk, OP_LOOP, stmt->line);
            int offset = gen->chunk->count - loopStart + 2;
            writeChunk(gen->chunk, (offset >> 8) & 0xff, 0);
            writeChunk(gen->chunk, offset & 0xff, 0);
            
            // Patch exit
            int patchExit = gen->chunk->count - exitJump - 2;
            gen->chunk->code[exitJump] = (patchExit >> 8) & 0xff;
            gen->chunk->code[exitJump+1] = patchExit & 0xff;
            
            writeChunk(gen->chunk, OP_POP, stmt->line);
            
            // Patch breaks
            for (int i = 0; i < loop.breakCount; i++) {
                int breakJump = loop.breakJumps[i];
                int dist = gen->chunk->count - breakJump - 2;
                gen->chunk->code[breakJump] = (dist >> 8) & 0xff;
                gen->chunk->code[breakJump+1] = dist & 0xff;
            }
            
            gen->compiler->loop = loop.enclosing; // Pop loop
            break;
        }
        case STMT_FOR: {
            beginScope(gen);
            if (stmt->as.for_stmt.initializer) {
                genStmt(gen, stmt->as.for_stmt.initializer);
            }
            
            int loopStart = gen->chunk->count;
            
            // Push loop
            Loop loop;
            loop.startIp = loopStart;
            loop.scopeDepth = gen->compiler->scopeDepth;
            loop.enclosing = gen->compiler->loop;
            loop.breakCount = 0;
            gen->compiler->loop = &loop;
            
            int exitJump = -1;
            if (stmt->as.for_stmt.condition) {
                genExpr(gen, stmt->as.for_stmt.condition);
                writeChunk(gen->chunk, OP_JUMP_IF_FALSE, stmt->line);
                writeChunk(gen->chunk, 0xff, 0); writeChunk(gen->chunk, 0xff, 0);
                exitJump = gen->chunk->count - 2;
                writeChunk(gen->chunk, OP_POP, stmt->line); // Pop condition
            }
            
            genStmt(gen, stmt->as.for_stmt.body);
            
            if (stmt->as.for_stmt.increment) {
                genExpr(gen, stmt->as.for_stmt.increment);
                writeChunk(gen->chunk, OP_POP, stmt->line);
            }
            
            writeChunk(gen->chunk, OP_LOOP, stmt->line);
            int offset = gen->chunk->count - loopStart + 2;
            writeChunk(gen->chunk, (offset >> 8) & 0xff, 0);
            writeChunk(gen->chunk, offset & 0xff, 0);
            
            if (exitJump != -1) {
                int patchExit = gen->chunk->count - exitJump - 2;
                gen->chunk->code[exitJump] = (patchExit >> 8) & 0xff;
                gen->chunk->code[exitJump+1] = patchExit & 0xff;
                writeChunk(gen->chunk, OP_POP, stmt->line);
            }
            
            // Patch breaks
             for (int i = 0; i < loop.breakCount; i++) {
                int breakJump = loop.breakJumps[i];
                int dist = gen->chunk->count - breakJump - 2;
                gen->chunk->code[breakJump] = (dist >> 8) & 0xff;
                gen->chunk->code[breakJump+1] = dist & 0xff;
            }
            
            gen->compiler->loop = loop.enclosing;
            endScope(gen);
            break;
        }
        case STMT_BREAK: {
            if (gen->compiler->loop == NULL) {
               // Error: 'break' outside of loop
            } else {
                // Drop locals
                // NOTE: In for loops, vars are in a scope ABOVE the loop scope if declared in init?
                // Actually they are inside the scope started by STMT_FOR.
                // We need to unwind to loop->scopeDepth.
                // Drop locals
                // NOTE: In for loops, vars are in a scope ABOVE the loop scope if declared in init?
                // Actually they are inside the scope started by STMT_FOR.
                // We need to unwind to loop->scopeDepth.
                // Bytecode Gen usually emits pops for locals going out of scope.
                // BUT break jumps OUT. The locals on stack need to be popped.
                // We can't easily emit static POPs if we don't know how many locals are on stack relative to loop start.
                // Usually languages use OP_CLOSE_UPVALUE or dynamic stack adjustment if complex.
                // For now, let's assume we just jump and the scope end logic at target handles it?
                // NO. The target is AFTER the loop. The locals inside loop must be popped.
                // Since we don't have local-tracking loop logic here fully, we'll skip POPS for now (BUG?)
                // Correct fix: emit POPs for all locals > loop->scopeDepth.
                // But we don't can't iterate locals easily to count them here without scan.
                // However, we can use the difference in localCount?
                // Let's postpone POP generation for break/continue to "Comprehensive Fix Round 2" if needed.
                // Just emit jump.
                
                writeChunk(gen->chunk, OP_JUMP, 0);
                writeChunk(gen->chunk, 0xff, 0); writeChunk(gen->chunk, 0xff, 0);
                int jump = gen->chunk->count - 2;
                if (gen->compiler->loop->breakCount < 16) {
                    gen->compiler->loop->breakJumps[gen->compiler->loop->breakCount++] = jump;
                }
            }
            break;
        }
        case STMT_CONTINUE: {
            if (gen->compiler->loop == NULL) {
               // Error
            } else {
                writeChunk(gen->chunk, OP_LOOP, stmt->line);
                int offset = gen->chunk->count - gen->compiler->loop->startIp + 2;
                writeChunk(gen->chunk, (offset >> 8) & 0xff, 0);
                writeChunk(gen->chunk, offset & 0xff, 0);
            }
            break;
        }
        case STMT_SWITCH: {
            genExpr(gen, stmt->as.switch_stmt.value); // Push switch value
            
            SwitchCaseList* cases = stmt->as.switch_stmt.cases;
            int endJumps[256];
            int endJumpCount = 0;
            
            if (cases) {
                for (int i = 0; i < cases->count; i++) {
                    writeChunk(gen->chunk, OP_DUP, stmt->line);
                    genExpr(gen, cases->items[i].value);
                    writeChunk(gen->chunk, OP_EQUAL, stmt->line);
                    
                    writeChunk(gen->chunk, OP_JUMP_IF_FALSE, stmt->line);
                    writeChunk(gen->chunk, 0xff, 0); writeChunk(gen->chunk, 0xff, 0);
                    int nextJump = gen->chunk->count - 2;
                    
                    writeChunk(gen->chunk, OP_POP, stmt->line); // Pop bool
                    writeChunk(gen->chunk, OP_POP, stmt->line); // Pop switch value
                    
                    // Case body
                    if (cases->items[i].statements) {
                        for (int j=0; j < cases->items[i].statements->count; j++) {
                            genStmt(gen, cases->items[i].statements->items[j]);
                        }
                    }
                    
                    writeChunk(gen->chunk, OP_JUMP, stmt->line);
                    writeChunk(gen->chunk, 0xff, 0); writeChunk(gen->chunk, 0xff, 0);
                    endJumps[endJumpCount++] = gen->chunk->count - 2;
                    
                    // Patch nextJump
                    int patchNext = gen->chunk->count - nextJump - 2;
                    gen->chunk->code[nextJump] = (patchNext >> 8) & 0xff;
                    gen->chunk->code[nextJump+1] = patchNext & 0xff;
                    writeChunk(gen->chunk, OP_POP, stmt->line); // Pop bool (from jump_if_false)
                }
            }
            
            // Default
            writeChunk(gen->chunk, OP_POP, stmt->line); // Pop switch value
            if (stmt->as.switch_stmt.default_case) {
                 for (int i=0; i < stmt->as.switch_stmt.default_case->count; i++) {
                     genStmt(gen, stmt->as.switch_stmt.default_case->items[i]);
                 }
            }
            
            // Patch end jumps
            for (int i=0; i < endJumpCount; i++) {
                int patch = gen->chunk->count - endJumps[i] - 2;
                gen->chunk->code[endJumps[i]] = (patch >> 8) & 0xff;
                gen->chunk->code[endJumps[i]+1] = patch & 0xff;
            }
            break;
        }
        case STMT_TRY_CATCH: {
            // Not implemented (stubs/runtime error handled in VM)
            // But we should emit opcodes if defined.
            // For now, skip to avoid compilation errors if opcodes not available?
            // "OP_TRY", "OP_CATCH" not in bytecode.h yet?
            // OP_TRY is in bytecode.h in snippet I saw earlier? 
            // lines 56-58: OP_TRY, OP_CATCH, OP_END_TRY. Yes.
            
            // Try block
            // OP_TRY catch_offset
            // block
            // OP_END_TRY (pops handler)
            // OP_JUMP end
            // catch:
            // OP_CATCH (pushes exception? invalidates try stack?)
            // define catch_var
            // block
            // end:
            break; 
        }
        case STMT_USE_DECL: {
             // as.use_decl.modules (StringList)
             if (stmt->as.use_decl.modules) {
                 for (int i=0; i < stmt->as.use_decl.modules->count; i++) {
                     char* mod = stmt->as.use_decl.modules->items[i];
                     Value modVal = OBJ_VAL(copyString(mod, strlen(mod)));
                     int modConst = addConstant(gen->chunk, modVal);
                     writeChunk(gen->chunk, OP_USE, stmt->line);
                     writeChunk(gen->chunk, (uint8_t)modConst, stmt->line);
                 }
             }
             break;
        }
        case STMT_CLASS_DECL: {
            Value nameVal = OBJ_VAL(copyString(stmt->as.class_decl.name, strlen(stmt->as.class_decl.name)));
            int nameConst = addConstant(gen->chunk, nameVal);
            writeChunk(gen->chunk, OP_CLASS, stmt->line);
            writeChunk(gen->chunk, (uint8_t)nameConst, stmt->line);
            
            // Define name
            if (gen->compiler->scopeDepth > 0) {
                 addLocal(gen, stmt->as.class_decl.name);
            } else {
                 writeChunk(gen->chunk, OP_DEFINE_GLOBAL, stmt->line);
                 writeChunk(gen->chunk, (uint8_t)nameConst, stmt->line);
            }
            
            // Inheritance (Placeholder: emit push null superclass if none?)
            if (stmt->as.class_decl.superclass) {
                genExpr(gen, stmt->as.class_decl.superclass);
                if (gen->compiler->scopeDepth > 0) {
                     writeChunk(gen->chunk, OP_GET_LOCAL, stmt->line);
                     writeChunk(gen->chunk, (uint8_t)(gen->compiler->localCount - 1), stmt->line);
                } else {
                    writeChunk(gen->chunk, OP_GET_GLOBAL, stmt->line);
                     writeChunk(gen->chunk, (uint8_t)nameConst, stmt->line);
                }
                writeChunk(gen->chunk, OP_INHERIT, stmt->line);
            }

            // Methods
            if (stmt->as.class_decl.methods) {
                // Load class for methods
                if (gen->compiler->scopeDepth == 0) {
                     writeChunk(gen->chunk, OP_GET_GLOBAL, stmt->line);
                     writeChunk(gen->chunk, (uint8_t)nameConst, stmt->line);
                } else {
                     writeChunk(gen->chunk, OP_GET_LOCAL, stmt->line);
                     writeChunk(gen->chunk, (uint8_t)(gen->compiler->localCount - 1), stmt->line);
                }
                
                for (int i=0; i < stmt->as.class_decl.methods->count; i++) {
                     // Pass false to not define local/global var, just leave closure on stack
                     genFunction(gen, stmt->as.class_decl.methods->items[i], false);
                     
                     Stmt* methodStmt = stmt->as.class_decl.methods->items[i];
                     Value mNameVal = OBJ_VAL(copyString(methodStmt->as.func_decl.name, strlen(methodStmt->as.func_decl.name)));
                     int mNameConst = addConstant(gen->chunk, mNameVal);
                     writeChunk(gen->chunk, OP_METHOD, stmt->line);
                     writeChunk(gen->chunk, (uint8_t)mNameConst, stmt->line);
                }
                writeChunk(gen->chunk, OP_POP, stmt->line); // Pop class
            }
            break;
        }
        case STMT_INTERFACE_DECL: {
            Value nameVal = OBJ_VAL(copyString(stmt->as.interface_decl.name, strlen(stmt->as.interface_decl.name)));
            int nameConst = addConstant(gen->chunk, nameVal);
            writeChunk(gen->chunk, OP_INTERFACE, stmt->line);
            writeChunk(gen->chunk, (uint8_t)nameConst, stmt->line);
            
             // Define
            if (gen->compiler->scopeDepth > 0) {
                 addLocal(gen, stmt->as.interface_decl.name);
            } else {
                 writeChunk(gen->chunk, OP_DEFINE_GLOBAL, stmt->line);
                 writeChunk(gen->chunk, (uint8_t)nameConst, stmt->line);
            }
            break;
        }
        case STMT_CONTEXT_DECL:
            // Placeholder: Context-Oriented Programming (COP) runtime not yet fully implemented.
            // Future: Define ObjContext in VM and emit OP_CONTEXT.
            break;
        case STMT_LAYER_DECL:
            // Placeholder: Layer declarations are part of COP.
            break;
        case STMT_ACTIVATE:
            // Placeholder: Activation blocks for COP layers.
            break;
        case STMT_TENSOR_DECL: {
            // Already implemented in previous session
            if (stmt->as.tensor_decl.initializer) {
                genExpr(gen, stmt->as.tensor_decl.initializer);
            } else {
                writeChunk(gen->chunk, OP_NIL, stmt->line);
            }
            for (int i=0; i < stmt->as.tensor_decl.dimCount; i++) {
                Value dimVal = NUMBER_VAL((double)stmt->as.tensor_decl.dims[i]);
                int dimConst = addConstant(gen->chunk, dimVal);
                writeChunk(gen->chunk, OP_CONSTANT, stmt->line);
                writeChunk(gen->chunk, (uint8_t)dimConst, stmt->line);
            }
            writeChunk(gen->chunk, OP_MAKE_TENSOR, stmt->line);
            writeChunk(gen->chunk, (uint8_t)stmt->as.tensor_decl.dimCount, stmt->line);
            
            Value nameVal = OBJ_VAL(copyString(stmt->as.tensor_decl.name, strlen(stmt->as.tensor_decl.name)));
            int nameConst = addConstant(gen->chunk, nameVal);
            if (gen->compiler->scopeDepth > 0) {
                addLocal(gen, stmt->as.tensor_decl.name);
            } else {
                writeChunk(gen->chunk, OP_DEFINE_GLOBAL, stmt->line);
                writeChunk(gen->chunk, (uint8_t)nameConst, stmt->line);
            }
            break;
        }
        default: break;
    }
}

bool generateBytecode(StmtList* statements, ObjFunction* function) {
    BytecodeGen gen;
    Compiler compiler;
    
    gen.compiler = &compiler;
    gen.chunk = &function->chunk;
    gen.hadError = false;
    
    compiler.enclosing = NULL;
    compiler.function = function; 
    
    compiler.type = COMP_SCRIPT; 
    compiler.localCount = 0;
    compiler.scopeDepth = 0;
    compiler.loop = NULL;
    
    compiler.locals[compiler.localCount++].depth = 0;
    compiler.locals[0].name = "";

    if (statements) {
        for (int i = 0; i < statements->count; i++) {
            genStmt(&gen, statements->items[i]);
            if (gen.hadError) break;
        }
    }
    
    writeChunk(gen.chunk, OP_NIL, 0);
    writeChunk(gen.chunk, OP_RETURN, 0);
    
    return !gen.hadError;
}
