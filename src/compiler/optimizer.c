#include "../include/optimizer.h"
#include <string.h>
#include <math.h>

static Expr* foldExpr(Expr* expr);

static Expr* foldBinary(Expr* expr) {
    expr->as.binary.left = foldExpr(expr->as.binary.left);
    expr->as.binary.right = foldExpr(expr->as.binary.right);

    Expr* l = expr->as.binary.left;
    Expr* r = expr->as.binary.right;

    if (l->type == EXPR_LITERAL && r->type == EXPR_LITERAL) {
        Value lv = l->as.literal.value;
        Value rv = r->as.literal.value;

        if (IS_NUMBER(lv) && IS_NUMBER(rv)) {
            double a = AS_NUMBER(lv);
            double b = AS_NUMBER(rv);
            const char* op = expr->as.binary.operator;

            Value res = NIL_VAL;
            bool folded = true;

            if (strcmp(op, "+") == 0) res = NUMBER_VAL(a + b);
            else if (strcmp(op, "-") == 0) res = NUMBER_VAL(a - b);
            else if (strcmp(op, "*") == 0) res = NUMBER_VAL(a * b);
            else if (strcmp(op, "/") == 0) res = NUMBER_VAL(a / b);
            else if (strcmp(op, "<") == 0) res = BOOL_VAL(a < b);
            else if (strcmp(op, ">") == 0) res = BOOL_VAL(a > b);
            else if (strcmp(op, "<=") == 0) res = BOOL_VAL(a <= b);
            else if (strcmp(op, ">=") == 0) res = BOOL_VAL(a >= b);
            else if (strcmp(op, "==") == 0) res = BOOL_VAL(a == b);
            else if (strcmp(op, "!=") == 0) res = BOOL_VAL(a != b);
            else folded = false;

            if (folded) {
                expr->type = EXPR_LITERAL;
                expr->as.literal.value = res;
                // Note: We leak the old children here, but this is a simple compiler.
                // In a production one, we would call freeExpr(l) and freeExpr(r).
                return expr;
            }
        }
    }

    return expr;
}

static Expr* foldUnary(Expr* expr) {
    expr->as.unary.right = foldExpr(expr->as.unary.right);
    Expr* r = expr->as.unary.right;

    if (r->type == EXPR_LITERAL) {
        Value rv = r->as.literal.value;
        const char* op = expr->as.unary.operator;

        if (strcmp(op, "-") == 0 && IS_NUMBER(rv)) {
            expr->type = EXPR_LITERAL;
            expr->as.literal.value = NUMBER_VAL(-AS_NUMBER(rv));
            return expr;
        } else if (strcmp(op, "!") == 0) {
            // isFalsey logic
            bool res = IS_NIL(rv) || (IS_BOOL(rv) && !AS_BOOL(rv));
            expr->type = EXPR_LITERAL;
            expr->as.literal.value = BOOL_VAL(res);
            return expr;
        }
    }

    return expr;
}

static Expr* foldExpr(Expr* expr) {
    if (!expr) return NULL;

    switch (expr->type) {
        case EXPR_BINARY: return foldBinary(expr);
        case EXPR_UNARY: return foldUnary(expr);
        case EXPR_GROUPING: 
            expr->as.grouping.expression = foldExpr(expr->as.grouping.expression);
            if (expr->as.grouping.expression->type == EXPR_LITERAL) return expr->as.grouping.expression;
            return expr;
        case EXPR_CALL:
            expr->as.call.callee = foldExpr(expr->as.call.callee);
            if (expr->as.call.arguments) {
                for (int i = 0; i < expr->as.call.arguments->count; i++) {
                    expr->as.call.arguments->items[i] = foldExpr(expr->as.call.arguments->items[i]);
                }
            }
            return expr;
        // Add more as needed
        default: return expr;
    }
}

static void foldStmt(Stmt* stmt) {
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_EXPRESSION:
            stmt->as.expression.expression = foldExpr(stmt->as.expression.expression);
            break;
        case STMT_PRINT:
            stmt->as.print.expression = foldExpr(stmt->as.print.expression);
            break;
        case STMT_VAR_DECL:
            if (stmt->as.var_decl.initializer)
                stmt->as.var_decl.initializer = foldExpr(stmt->as.var_decl.initializer);
            break;
        case STMT_IF:
            stmt->as.if_stmt.condition = foldExpr(stmt->as.if_stmt.condition);
            if (stmt->as.if_stmt.condition->type == EXPR_LITERAL) {
                Value cond = stmt->as.if_stmt.condition->as.literal.value;
                bool is_true = !IS_NIL(cond) && (!IS_BOOL(cond) || AS_BOOL(cond));
                if (is_true) {
                    // Only keep then branch (simplified for now by just folding children)
                    foldStmt(stmt->as.if_stmt.then_branch);
                } else {
                    // Only keep else branch
                    if (stmt->as.if_stmt.else_branch)
                        foldStmt(stmt->as.if_stmt.else_branch);
                }
            } else {
                foldStmt(stmt->as.if_stmt.then_branch);
                foldStmt(stmt->as.if_stmt.else_branch);
            }
            break;
        case STMT_WHILE:
            stmt->as.while_stmt.condition = foldExpr(stmt->as.while_stmt.condition);
            if (stmt->as.while_stmt.condition->type == EXPR_LITERAL) {
                Value cond = stmt->as.while_stmt.condition->as.literal.value;
                bool is_true = !IS_NIL(cond) && (!IS_BOOL(cond) || AS_BOOL(cond));
                if (!is_true) {
                    // Loop will never run. We could technically remove the statement.
                    // For now, we just mark it as potentially skippable or do nothing.
                } else {
                    foldStmt(stmt->as.while_stmt.body);
                }
            } else {
                foldStmt(stmt->as.while_stmt.body);
            }
            break;
        case STMT_BLOCK:
            if (stmt->as.block.statements) {
                for (int i = 0; i < stmt->as.block.statements->count; i++) {
                    foldStmt(stmt->as.block.statements->items[i]);
                }
            }
            break;
        case STMT_FUNC_DECL:
            if (stmt->as.func_decl.body) {
                for (int i = 0; i < stmt->as.func_decl.body->count; i++) {
                    foldStmt(stmt->as.func_decl.body->items[i]);
                }
            }
            break;
        case STMT_RETURN:
            if (stmt->as.return_stmt.value)
                stmt->as.return_stmt.value = foldExpr(stmt->as.return_stmt.value);
            break;
        default: break;
    }
}

void optimizeAST(StmtList* statements) {
    if (!statements) return;
    for (int i = 0; i < statements->count; i++) {
        foldStmt(statements->items[i]);
    }
}
