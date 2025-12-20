#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/ast.h"
#include "../../include/bytecode.h"
#include "../../include/common.h"
#include "../../include/value.h"
#include "../../include/object.h"

typedef struct {
    Chunk* chunk;
    // Local scope management (simplified for now, will need full symbol table later)
    // For now, we assume global scope if not handled.
} BytecodeGen;

static void emitByte(BytecodeGen* gen, uint8_t byte, int line) {
    writeChunk(gen->chunk, byte, line);
}

static void emitBytes(BytecodeGen* gen, uint8_t byte1, uint8_t byte2, int line) {
    emitByte(gen, byte1, line);
    emitByte(gen, byte2, line);
}

static void emitConstant(BytecodeGen* gen, Value value, int line) {
    int constant = addConstant(gen->chunk, value);
    if (constant > 255) {
        // Handle long constants if needed
        fprintf(stderr, "Too many constants in one chunk\n");
        return;
    }
    emitBytes(gen, OP_CONSTANT, (uint8_t)constant, line);
}

static int emitJump(BytecodeGen *gen, uint8_t instruction, int line) {
  emitByte(gen, instruction, line);
  emitByte(gen, 0xff, line);
  emitByte(gen, 0xff, line);
  return gen->chunk->count - 2;
}

static void patchJump(BytecodeGen *gen, int offset) {
  // -2 to adjust for the bytecode for the jump offset itself.
  int jump = gen->chunk->count - offset - 2;

  if (jump > UINT16_MAX) {
    fprintf(stderr, "Too much code to jump over.\n");
  }

  gen->chunk->code[offset] = (jump >> 8) & 0xff;
  gen->chunk->code[offset + 1] = jump & 0xff;
}

static void emitLoop(BytecodeGen *gen, int loopStart, int line) {
  emitByte(gen, OP_LOOP, line);

  int offset = gen->chunk->count - loopStart + 2;
  if (offset > UINT16_MAX) fprintf(stderr, "Loop body too large.\n");

  emitByte(gen, (offset >> 8) & 0xff, line);
  emitByte(gen, offset & 0xff, line);
}

// Forward declarations for recursive traversal
static void genExpr(BytecodeGen* gen, Expr* expr);
static void genStmt(BytecodeGen* gen, Stmt* stmt);

static void genExpr(BytecodeGen* gen, Expr* expr) {
    if (expr == NULL) return;

    switch (expr->type) {
        case EXPR_LITERAL:
            if (IS_NUMBER(expr->as.literal.value)) {
                emitConstant(gen, expr->as.literal.value, expr->line);
            } else if (IS_BOOL(expr->as.literal.value)) {
                emitByte(gen, AS_BOOL(expr->as.literal.value) ? OP_TRUE : OP_FALSE, expr->line);
            } else if (IS_NULL(expr->as.literal.value)) {
                emitByte(gen, OP_NIL, expr->line);
            }
            break;

        case EXPR_BINARY:
            genExpr(gen, expr->as.binary.left);
            genExpr(gen, expr->as.binary.right);

            if (strcmp(expr->as.binary.operator, "+") == 0) emitByte(gen, OP_ADD, expr->line);
            else if (strcmp(expr->as.binary.operator, "-") == 0) emitByte(gen, OP_SUBTRACT, expr->line);
            else if (strcmp(expr->as.binary.operator, "*") == 0) emitByte(gen, OP_MULTIPLY, expr->line);
            else if (strcmp(expr->as.binary.operator, "/") == 0) emitByte(gen, OP_DIVIDE, expr->line);
            else if (strcmp(expr->as.binary.operator, "==") == 0) emitByte(gen, OP_EQUAL, expr->line);
            else if (strcmp(expr->as.binary.operator, "!=") == 0) {
               emitBytes(gen, OP_EQUAL, OP_NOT, expr->line);
            }
            else if (strcmp(expr->as.binary.operator, ">") == 0) emitByte(gen, OP_GREATER, expr->line);
            else if (strcmp(expr->as.binary.operator, "<") == 0) emitByte(gen, OP_LESS, expr->line);
            else if (strcmp(expr->as.binary.operator, ">=") == 0) {
               emitBytes(gen, OP_LESS, OP_NOT, expr->line);
            }
            else if (strcmp(expr->as.binary.operator, "<=") == 0) {
               emitBytes(gen, OP_GREATER, OP_NOT, expr->line);
            }
            break;

        case EXPR_UNARY:
            genExpr(gen, expr->as.unary.right);
            if (strcmp(expr->as.unary.operator, "!") == 0) emitByte(gen, OP_NOT, expr->line);
            else if (strcmp(expr->as.unary.operator, "-") == 0) emitByte(gen, OP_NEGATE, expr->line);
            break;

        case EXPR_GROUPING:
            genExpr(gen, expr->as.grouping.expression);
            break;

        case EXPR_VARIABLE: {
            // Placeholder: Assume global for now
            // In a real implementation, we'd lookup in scope and emit OP_GET_LOCAL if found
            // For now, we'll need to store the variable name as a constant
            Value nameVal = OBJ_VAL(copyString(expr->as.variable.name, strlen(expr->as.variable.name)));
            int nameConst = addConstant(gen->chunk, nameVal);
            emitBytes(gen, OP_GET_GLOBAL, (uint8_t)nameConst, expr->line);
            break;
        }

        case EXPR_ASSIGN: {
            genExpr(gen, expr->as.assign.value);
            Value nameVal = OBJ_VAL(copyString(expr->as.assign.name, strlen(expr->as.assign.name)));
            int nameConst = addConstant(gen->chunk, nameVal);
            emitBytes(gen, OP_SET_GLOBAL, (uint8_t)nameConst, expr->line);
            break;
        }

        case EXPR_LOGICAL: {
            if (strcmp(expr->as.logical.operator, "&&") == 0) {
                genExpr(gen, expr->as.logical.left);
                int endJump = emitJump(gen, OP_JUMP_IF_FALSE, expr->line);
                emitByte(gen, OP_POP, expr->line);
                genExpr(gen, expr->as.logical.right);
                patchJump(gen, endJump);
            } else { // ||
                genExpr(gen, expr->as.logical.left);
                int elseJump = emitJump(gen, OP_JUMP_IF_FALSE, expr->line);
                int endJump = emitJump(gen, OP_JUMP, expr->line);
                patchJump(gen, elseJump);
                emitByte(gen, OP_POP, expr->line);
                genExpr(gen, expr->as.logical.right);
                patchJump(gen, endJump);
            }
            break;
        }

        default:
            fprintf(stderr, "Unimplemented expression type in BytecodeGen: %d\n", expr->type);
            break;
    }
}

static void genStmt(BytecodeGen* gen, Stmt* stmt) {
    if (stmt == NULL) return;

    switch (stmt->type) {
        case STMT_EXPRESSION:
            genExpr(gen, stmt->as.expression.expression);
            emitByte(gen, OP_POP, stmt->line);
            break;

        case STMT_PRINT:
            genExpr(gen, stmt->as.print.expression);
            emitByte(gen, OP_PRINT, stmt->line);
            break;

        case STMT_RETURN:
            if (stmt->as.return_stmt.value != NULL) {
                genExpr(gen, stmt->as.return_stmt.value);
            } else {
                emitByte(gen, OP_NIL, stmt->line);
            }
            emitByte(gen, OP_RETURN, stmt->line);
            break;

        case STMT_BLOCK:
            for (int i = 0; i < stmt->as.block.statements->count; i++) {
                genStmt(gen, stmt->as.block.statements->items[i]);
            }
            break;

        case STMT_IF: {
            genExpr(gen, stmt->as.if_stmt.condition);
            int thenJump = emitJump(gen, OP_JUMP_IF_FALSE, stmt->line);
            emitByte(gen, OP_POP, stmt->line);
            genStmt(gen, stmt->as.if_stmt.then_branch);

            int elseJump = emitJump(gen, OP_JUMP, stmt->line);
            patchJump(gen, thenJump);
            emitByte(gen, OP_POP, stmt->line);

            if (stmt->as.if_stmt.else_branch != NULL) {
                genStmt(gen, stmt->as.if_stmt.else_branch);
            }
            patchJump(gen, elseJump);
            break;
        }

        case STMT_WHILE: {
            int loopStart = gen->chunk->count;
            genExpr(gen, stmt->as.while_stmt.condition);

            int exitJump = emitJump(gen, OP_JUMP_IF_FALSE, stmt->line);
            emitByte(gen, OP_POP, stmt->line);
            genStmt(gen, stmt->as.while_stmt.body);
            emitLoop(gen, loopStart, stmt->line);

            patchJump(gen, exitJump);
            emitByte(gen, OP_POP, stmt->line);
            break;
        }

        case STMT_VAR_DECL: {
            if (stmt->as.var_decl.initializer != NULL) {
                genExpr(gen, stmt->as.var_decl.initializer);
            } else {
                emitByte(gen, OP_NIL, stmt->line);
            }
            Value nameVal = OBJ_VAL(copyString(stmt->as.var_decl.name, strlen(stmt->as.var_decl.name)));
            int nameConst = addConstant(gen->chunk, nameVal);
            emitBytes(gen, OP_DEFINE_GLOBAL, (uint8_t)nameConst, stmt->line);
            break;
        }

        default:
            fprintf(stderr, "Unimplemented statement type in BytecodeGen: %d\n", stmt->type);
            break;
    }
}

void generateBytecode(StmtList* statements, Chunk* chunk) {
    BytecodeGen gen;
    gen.chunk = chunk;

    for (int i = 0; i < statements->count; i++) {
        genStmt(&gen, statements->items[i]);
    }
}
