#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/ast.h"
#include "../../include/bytecode.h"
#include "../../include/common.h"
#include "../../include/value.h"
#include "../../include/object.h"

// --- Compiler & Scope Types ---

typedef struct {
    const char* name;
    int depth;
} Local;

typedef enum {
    COMP_FUNCTION,
    COMP_SCRIPT
} CompFunctionType;

typedef struct Compiler {
    struct Compiler* enclosing;
    ObjFunction* function;
    CompFunctionType type;

    Local locals[256];
    int localCount;
    int scopeDepth;
} Compiler;

typedef struct {
    Compiler* compiler;
    Chunk* chunk; // Current chunk being written to (convenience)
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

static ObjFunction* endCompiler(BytecodeGen* gen) {
    // Emit return
    writeChunk(gen->chunk, OP_NIL, 0); // TODO: Line info
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
    if (gen->compiler->localCount == 256) {
        fprintf(stderr, "Too many local variables.\n");
        return;
    }
    Local* local = &gen->compiler->locals[gen->compiler->localCount++];
    local->name = name;
    local->depth = gen->compiler->scopeDepth; // Assuming initialized immediately
}

// remove static declaration of addConstant, use header implementation implicitly linked

static void emitConstant(BytecodeGen* gen, Value value, int line) {
    int constant = addConstant(gen->chunk, value);
    if (constant > 255) {
        writeChunk(gen->chunk, OP_CONSTANT, line); // Handle appropriately if we had OP_CONSTANT_LONG
        fprintf(stderr, "Too many constants.\n");
    } else {
        writeChunk(gen->chunk, OP_CONSTANT, line);
        writeChunk(gen->chunk, (uint8_t)constant, line);
    }
}

// ... (Rest of BytecodeGen is fine until generateBytecode/STMT_FUNC_DECL) ...
// NOTE: Must ensure STMT_FUNC_DECL uses COMP_FUNCTION

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
        default: break; 
    }
}

static void genStmt(BytecodeGen* gen, Stmt* stmt) {
    if (stmt == NULL) return;
    
    switch (stmt->type) {
        // ...
        case STMT_FUNC_DECL: {
            // New Compiler for Function
            Compiler funcCompiler;
            initCompiler(gen, &funcCompiler, COMP_FUNCTION);
            
            // Add params to scope
            beginScope(gen);
            if (stmt->as.func_decl.params) {
                for (int i=0; i < stmt->as.func_decl.params->count; i++) {
                    funcCompiler.function->arity++;
                    if (funcCompiler.function->arity > 255) {
                        // Error
                    }
                    addLocal(gen, stmt->as.func_decl.params->items[i]);
                }
            }
            
            StmtList* body = stmt->as.func_decl.body;
            if (body) {
                for (int i=0; i < body->count; i++) {
                    genStmt(gen, body->items[i]);
                }
            }
            
            // Function object created in funcCompiler.function
            ObjFunction* function = endCompiler(gen);
            
            // Emit Closure
            Value funcVal = OBJ_VAL(function);
            int funcConst = addConstant(gen->chunk, funcVal);
            writeChunk(gen->chunk, OP_CLOSURE, stmt->line);
            writeChunk(gen->chunk, (uint8_t)funcConst, stmt->line);
            
            // Define name
            if (gen->compiler->scopeDepth > 0) {
                addLocal(gen, stmt->as.func_decl.name); 
            } else {
                Value nameVal = OBJ_VAL(copyString(stmt->as.func_decl.name, strlen(stmt->as.func_decl.name)));
                int nameConst = addConstant(gen->chunk, nameVal);
                writeChunk(gen->chunk, OP_DEFINE_GLOBAL, stmt->line);
                writeChunk(gen->chunk, (uint8_t)nameConst, stmt->line);
            }
            break;
        }
        // ... (Rest default)
        default:
             if(stmt->type == STMT_BLOCK) { // Handle other cases similarly if overridden
                beginScope(gen);
                for (int i=0; i < stmt->as.block.statements->count; i++) {
                    genStmt(gen, stmt->as.block.statements->items[i]);
                }
                endScope(gen);
             } else if (stmt->type == STMT_VAR_DECL) {
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
             } else if (stmt->type == STMT_EXPRESSION) {
                genExpr(gen, stmt->as.expression.expression);
                writeChunk(gen->chunk, OP_POP, stmt->line);
             } else if (stmt->type == STMT_PRINT) {
                genExpr(gen, stmt->as.print.expression);
                writeChunk(gen->chunk, OP_PRINT, stmt->line);
             } else if (stmt->type == STMT_RETURN) {
                if (stmt->as.return_stmt.value) {
                    genExpr(gen, stmt->as.return_stmt.value);
                } else {
                     writeChunk(gen->chunk, OP_NIL, stmt->line);
                }
                writeChunk(gen->chunk, OP_RETURN, stmt->line);
             } else if (stmt->type == STMT_IF) {
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
             } else if (stmt->type == STMT_WHILE) {
                int loopStart = gen->chunk->count;
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
                
                int patchExit = gen->chunk->count - exitJump - 2;
                gen->chunk->code[exitJump] = (patchExit >> 8) & 0xff;
                gen->chunk->code[exitJump+1] = patchExit & 0xff;
                
                writeChunk(gen->chunk, OP_POP, stmt->line);
             }
             else {
                // fall back for other types if any
             }
             break;
    }
}

void generateBytecode(StmtList* statements, Chunk* chunk) {
    BytecodeGen gen;
    Compiler compiler;
    
    gen.compiler = &compiler;
    gen.chunk = chunk;
    
    compiler.enclosing = NULL;
    compiler.function = newFunction(); 
    compiler.function->chunk = *chunk; 
    
    compiler.type = COMP_SCRIPT; // Use new enum value
    compiler.localCount = 0;
    compiler.scopeDepth = 0;
    
    compiler.locals[compiler.localCount++].depth = 0;
    compiler.locals[0].name = "";

    for (int i = 0; i < statements->count; i++) {
        genStmt(&gen, statements->items[i]);
    }
    
    writeChunk(gen.chunk, OP_NIL, 0); // Fix: gen.chunk instead of gen->chunk
    writeChunk(gen.chunk, OP_RETURN, 0); // Fix: gen.chunk instead of gen->chunk
}
