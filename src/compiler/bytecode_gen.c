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
    TYPE_FUNCTION,
    TYPE_SCRIPT
} FunctionType;

typedef struct Compiler {
    struct Compiler* enclosing;
    ObjFunction* function;
    FunctionType type;

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

static void initCompiler(BytecodeGen* gen, Compiler* compiler, FunctionType type) {
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

static int addConstant(Chunk* chunk, Value value); // From bytecode.h

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

// --- Generation Implementation ---

static void genExpr(BytecodeGen* gen, Expr* expr) {
    if (expr == NULL) return;

    switch (expr->type) {
        case EXPR_LITERAL:
            if (IS_NUMBER(expr->as.literal.value)) {
                emitConstant(gen, expr->as.literal.value, expr->line);
            } else if (IS_BOOL(expr->as.literal.value)) {
                writeChunk(gen->chunk, AS_BOOL(expr->as.literal.value) ? OP_TRUE : OP_FALSE, expr->line);
            } else if (IS_NULL(expr->as.literal.value)) {
                 writeChunk(gen->chunk, OP_NIL, expr->line);
            }
            break;

        case EXPR_BINARY:
            genExpr(gen, expr->as.binary.left);
            genExpr(gen, expr->as.binary.right);
            // ... (Same ops as before) ...
            if (strcmp(expr->as.binary.operator, "+") == 0) writeChunk(gen->chunk, OP_ADD, expr->line);
            else if (strcmp(expr->as.binary.operator, "-") == 0) writeChunk(gen->chunk, OP_SUBTRACT, expr->line);
            else if (strcmp(expr->as.binary.operator, "*") == 0) writeChunk(gen->chunk, OP_MULTIPLY, expr->line);
            else if (strcmp(expr->as.binary.operator, "/") == 0) writeChunk(gen->chunk, OP_DIVIDE, expr->line);
            else if (strcmp(expr->as.binary.operator, "==") == 0) writeChunk(gen->chunk, OP_EQUAL, expr->line);
            else if (strcmp(expr->as.binary.operator, "!=") == 0) {
                 writeChunk(gen->chunk, OP_EQUAL, expr->line);
                 writeChunk(gen->chunk, OP_NOT, expr->line);
            }
            else if (strcmp(expr->as.binary.operator, ">") == 0) writeChunk(gen->chunk, OP_GREATER, expr->line);
            else if (strcmp(expr->as.binary.operator, "<") == 0) writeChunk(gen->chunk, OP_LESS, expr->line);
            else if (strcmp(expr->as.binary.operator, ">=") == 0) {
                 writeChunk(gen->chunk, OP_LESS, expr->line);
                 writeChunk(gen->chunk, OP_NOT, expr->line);
            }
            else if (strcmp(expr->as.binary.operator, "<=") == 0) {
                 writeChunk(gen->chunk, OP_GREATER, expr->line);
                 writeChunk(gen->chunk, OP_NOT, expr->line);
            }
            break;
            
        case EXPR_UNARY:
             genExpr(gen, expr->as.unary.right);
             if (strcmp(expr->as.unary.operator, "!") == 0) writeChunk(gen->chunk, OP_NOT, expr->line);
             else if (strcmp(expr->as.unary.operator, "-") == 0) writeChunk(gen->chunk, OP_NEGATE, expr->line);
             break;

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
            if (expr->as.call.arguments != NULL) {
                for (int i=0; i < expr->as.call.arguments->count; i++) {
                    genExpr(gen, expr->as.call.arguments->items[i]);
                    argCount++;
                }
            }
            writeChunk(gen->chunk, OP_CALL, expr->line);
            writeChunk(gen->chunk, (uint8_t)argCount, expr->line); // Assumes < 256 args
            break;
        }
        
        // ... (Other cases skipped for brevity if not changed significantly, but we must implement logic)
        // Grouping, Logical are same logic.
        case EXPR_GROUPING:
            genExpr(gen, expr->as.grouping.expression);
            break;
            
        case EXPR_LOGICAL: {
             // ... Logic same as before, simplified copy ...
            if (strcmp(expr->as.logical.operator, "&&") == 0) {
                genExpr(gen, expr->as.logical.left);
                // Emit Jump If False
                writeChunk(gen->chunk, OP_JUMP_IF_FALSE, expr->line);
                writeChunk(gen->chunk, 0xff, expr->line); writeChunk(gen->chunk, 0xff, expr->line); // Placeholder
                int jump = gen->chunk->count - 2;
                
                writeChunk(gen->chunk, OP_POP, expr->line);
                genExpr(gen, expr->as.logical.right);
                
                int patch = gen->chunk->count - jump - 2;
                gen->chunk->code[jump] = (patch >> 8) & 0xff;
                gen->chunk->code[jump+1] = patch & 0xff;
            } else {
                genExpr(gen, expr->as.logical.left);
                
                writeChunk(gen->chunk, OP_JUMP_IF_FALSE, expr->line);
                writeChunk(gen->chunk, 0xff, expr->line); writeChunk(gen->chunk, 0xff, expr->line);
                int elseJump = gen->chunk->count - 2;
                
                writeChunk(gen->chunk, OP_JUMP, expr->line);
                writeChunk(gen->chunk, 0xff, expr->line); writeChunk(gen->chunk, 0xff, expr->line);
                int endJump = gen->chunk->count - 2;
                
                int patchElse = gen->chunk->count - elseJump - 2;
                gen->chunk->code[elseJump] = (patchElse >> 8) & 0xff;
                gen->chunk->code[elseJump+1] = patchElse & 0xff;
                
                writeChunk(gen->chunk, OP_POP, expr->line);
                genExpr(gen, expr->as.logical.right);
                
                int patchEnd = gen->chunk->count - endJump - 2;
                gen->chunk->code[endJump] = (patchEnd >> 8) & 0xff;
                gen->chunk->code[endJump+1] = patchEnd & 0xff;
            }
            break;
        }

        default: break;
    }
}

static void genStmt(BytecodeGen* gen, Stmt* stmt) {
    if (stmt == NULL) return;
    
    switch (stmt->type) {
        case STMT_BLOCK:
            beginScope(gen);
            for (int i=0; i < stmt->as.block.statements->count; i++) {
                genStmt(gen, stmt->as.block.statements->items[i]);
            }
            endScope(gen);
            break;
            
        case STMT_VAR_DECL: {
            if (stmt->as.var_decl.initializer) {
                genExpr(gen, stmt->as.var_decl.initializer);
            } else {
                writeChunk(gen->chunk, OP_NIL, stmt->line);
            }
            
            if (gen->compiler->scopeDepth > 0) {
                addLocal(gen, stmt->as.var_decl.name);
                // No opcode needed, value is already on stack
            } else {
                Value nameVal = OBJ_VAL(copyString(stmt->as.var_decl.name, strlen(stmt->as.var_decl.name)));
                int nameConst = addConstant(gen->chunk, nameVal);
                writeChunk(gen->chunk, OP_DEFINE_GLOBAL, stmt->line);
                writeChunk(gen->chunk, (uint8_t)nameConst, stmt->line);
            }
            break;
        }
        
        case STMT_FUNC_DECL: {
            // New Compiler for Function
            Compiler funcCompiler;
            initCompiler(gen, &funcCompiler, TYPE_FUNCTION);
            
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
            
            // Body
            genStmt(gen, (Stmt*)stmt->as.func_decl.body); // Cast: StmtList is wrapped in BlockStmt?
            // Wait, AST.h: FuncDeclStmt has StmtList* body, but here we treat StmtList* as block? 
            // We need to iterate StmtList directly if not wrapped in BlockStmt.
            // Actually genStmt expects Stmt*. 
            // In genStmt(STMT_BLOCK), it iterates lists.
            // FuncDecl body IS a StmtList*.
            // We should modify genStmt to accept StmtList? No.
            // We should manually iterate here.
            
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

        case STMT_EXPRESSION:
            genExpr(gen, stmt->as.expression.expression);
            writeChunk(gen->chunk, OP_POP, stmt->line);
            break;
            
        case STMT_PRINT:
            genExpr(gen, stmt->as.print.expression);
            writeChunk(gen->chunk, OP_PRINT, stmt->line);
            break;
            
        case STMT_RETURN:
            if (stmt->as.return_stmt.value) {
                genExpr(gen, stmt->as.return_stmt.value);
            } else {
                 writeChunk(gen->chunk, OP_NIL, stmt->line);
            }
            writeChunk(gen->chunk, OP_RETURN, stmt->line);
            break;
            
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
            break;
        }

        default: break;
    }
}

void generateBytecode(StmtList* statements, Chunk* chunk) {
    BytecodeGen gen;
    Compiler compiler;
    
    // We need to initialize the chunk that was passed in.
    // However, our initCompiler creates a NEW Function and Chunk.
    // For the main script, we want to use the passed chunk.
    // We can manually setup the first compiler.
    
    gen.compiler = &compiler;
    gen.chunk = chunk;
    
    compiler.enclosing = NULL;
    compiler.function = newFunction(); // Temporary, wrapper
    // Actually, in main.c, chunk is malloced.
    // We should treat the top-level as a function too, ideally.
    // But `interpretChunk` uses the raw chunk.
    // So we just point to it.
    compiler.function->chunk = *chunk; // Shallow copy props? No, chunk is ptr.
    // wait, chunk is *Chunk.
    // compiler.function has a Chunk (value).
    
    // Let's just point gen.chunk to the input chunk and use compiler for locals.
    compiler.type = TYPE_SCRIPT;
    compiler.localCount = 0;
    compiler.scopeDepth = 0;
    
    // Reserve slot 0? If it's script, maybe not needed, but consistency helps.
    // Compiler usually expects locals[0] to be reserved.
    compiler.locals[compiler.localCount++].depth = 0;
    compiler.locals[0].name = "";

    for (int i = 0; i < statements->count; i++) {
        genStmt(&gen, statements->items[i]);
    }
    
    // We do NOT emit OP_RETURN here because the REPL loop/main.c might expect specific behavior?
    // main.c calls interpretChunk which runs untill return.
    // So yes, we need OP_RETURN.
    writeChunk(gen->chunk, OP_NIL, 0);
    writeChunk(gen->chunk, OP_RETURN, 0);
}
