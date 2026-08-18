#include "../include/escape_analysis.h"
#include <string.h>

static bool exprContainsVar(Expr* expr, const char* varName) {
    if (!expr) return false;
    switch (expr->type) {
        case EXPR_VARIABLE:
            return strcmp(expr->as.variable.name, varName) == 0;
        case EXPR_BINARY:
            return exprContainsVar(expr->as.binary.left, varName) || exprContainsVar(expr->as.binary.right, varName);
        case EXPR_UNARY:
            return exprContainsVar(expr->as.unary.right, varName);
        case EXPR_CALL: {
            if (exprContainsVar(expr->as.call.callee, varName)) return true;
            if (expr->as.call.arguments) {
                for (int i = 0; i < expr->as.call.arguments->count; i++) {
                    if (exprContainsVar(expr->as.call.arguments->items[i], varName)) return true;
                }
            }
            return false;
        }
        case EXPR_GET:
            return exprContainsVar(expr->as.get.object, varName);
        case EXPR_SET:
            return exprContainsVar(expr->as.set.object, varName) || exprContainsVar(expr->as.set.value, varName);
        case EXPR_INDEX:
            return exprContainsVar(expr->as.index.target, varName) || exprContainsVar(expr->as.index.index, varName);
        case EXPR_SET_INDEX:
            return exprContainsVar(expr->as.set_index.target, varName) ||
                   exprContainsVar(expr->as.set_index.index, varName) ||
                   exprContainsVar(expr->as.set_index.value, varName);
        case EXPR_LIST:
            if (expr->as.list.elements) {
                for (int i = 0; i < expr->as.list.elements->count; i++) {
                    if (exprContainsVar(expr->as.list.elements->items[i], varName)) return true;
                }
            }
            return false;
        case EXPR_DICTIONARY:
            if (expr->as.dictionary.pairs) {
                for (int i = 0; i < expr->as.dictionary.pairs->count; i++) {
                    if (exprContainsVar(expr->as.dictionary.pairs->items[i].key, varName) ||
                        exprContainsVar(expr->as.dictionary.pairs->items[i].value, varName)) return true;
                }
            }
            return false;
        case EXPR_TEMPLATE_LITERAL:
            if (expr->as.template_literal.parts) {
                for (int i = 0; i < expr->as.template_literal.parts->count; i++) {
                    if (exprContainsVar(expr->as.template_literal.parts->items[i], varName)) return true;
                }
            }
            return false;
        default:
            return false;
    }
}

EscapeState analyzeClosureEscape(Stmt* funcDecl) {
    if (!funcDecl || funcDecl->type != STMT_FUNC_DECL) return ESCAPE_NONE;

    // By default, if the function is not static or is at top-level script scope, it's global
    if (funcDecl->as.func_decl.isStatic) {
        return ESCAPE_NONE;
    }

    return ESCAPE_NONE;
}

EscapeState analyzeLambdaEscape(Expr* lambdaExpr, StmtList* enclosingBody) {
    if (!lambdaExpr || lambdaExpr->type != EXPR_LAMBDA || !enclosingBody) return ESCAPE_NONE;

    // Check statements in enclosing body for return statements returning this lambda
    for (int i = 0; i < enclosingBody->count; i++) {
        Stmt* stmt = enclosingBody->items[i];
        if (stmt->type == STMT_RETURN && stmt->as.return_stmt.value == lambdaExpr) {
            return ESCAPE_RETURN;
        }
    }

    return ESCAPE_NONE;
}

bool isClosureStackEligible(Stmt* funcDecl) {
    return analyzeClosureEscape(funcDecl) == ESCAPE_NONE;
}
