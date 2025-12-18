// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include "parser.h"
#include "memory.h"
#include "../include/object.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Forward declarations of parsing functions
static Stmt *declaration(Parser *p);
static Stmt *statement(Parser *p);
static Stmt *funcDecl(Parser *p, const char *kind);
static Stmt *classDecl(Parser *p);
static Stmt *varDecl(Parser *p);
static Stmt *useDecl(Parser *p);
static Stmt *forStmt(Parser *p);
static Stmt *ifStmt(Parser *p);
static Stmt *whileStmt(Parser *p);
static Stmt *switchStmt(Parser *p);
static Stmt *tryStmt(Parser *p);
static Stmt *returnStmt(Parser *p);
static Stmt *breakStmt(Parser *p);
static Stmt *continueStmt(Parser *p);
static Stmt *printStmt(Parser *p);
static Stmt *exprStmt(Parser *p);
static StmtList *block(Parser *p);

static Expr *expression(Parser *p);
static Expr *assignment(Parser *p);
static Expr *ternary(Parser *p);
static Expr *orExpr(Parser *p);
static Expr *andExpr(Parser *p);
static Expr *equality(Parser *p);
static Expr *comparison(Parser *p);
static Expr *term(Parser *p);
static Expr *factor(Parser *p);
static Expr *unary(Parser *p);
static Expr *call(Parser *p);
static Expr *primary(Parser *p);

// Helper functions
static bool isAtEnd(Parser *p);
static Token peek(Parser *p);
static Token previous(Parser *p);
static Token advance(Parser *p);
static bool check(Parser *p, TokenType type);
static bool match(Parser *p, int count, ...);
static Token consume(Parser *p, TokenType type, const char *message);
static void synchronize(Parser *p);

// === Initialization ===

void initParser(Parser *parser, Token *tokens, int count) {
  parser->tokens = tokens;
  parser->count = count;
  parser->current = 0;
}

void parserError(Parser *parser, const char *message) {
  Token token = peek(parser);
  fprintf(stderr, "ParseError: %s at line %d\\n", message, token.line);
}

// === Helper Functions ===

static bool isAtEnd(Parser *p) { return peek(p).type == TOKEN_EOF; }

static Token peek(Parser *p) { return p->tokens[p->current]; }

static Token previous(Parser *p) { return p->tokens[p->current - 1]; }

static Token advance(Parser *p) {
  if (!isAtEnd(p))
    p->current++;
  return previous(p);
}

static bool check(Parser *p, TokenType type) {
  if (isAtEnd(p))
    return false;
  return peek(p).type == type;
}

static bool match(Parser *p, int count, ...) {
  va_list args;
  va_start(args, count);

  for (int i = 0; i < count; i++) {
    TokenType type = va_arg(args, TokenType);
    if (check(p, type)) {
      advance(p);
      va_end(args);
      return true;
    }
  }

  va_end(args);
  return false;
}

static Token consume(Parser *p, TokenType type, const char *message) {
  if (check(p, type))
    return advance(p);
  parserError(p, message);
  return peek(p); // Return current token on error
}

static void synchronize(Parser *p) {
  advance(p);
  while (!isAtEnd(p)) {
    if (previous(p).type == TOKEN_SEMICOLON)
      return;

    switch (peek(p).type) {
    case TOKEN_CLASS:
    case TOKEN_FUNC:
    case TOKEN_LET:
    case TOKEN_FOR:
    case TOKEN_IF:
    case TOKEN_RETURN:
      return;
    default:
      break;
    }

    advance(p);
  }
}

// Helper to extract token value as string
static char *tokenToString(Token token) {
  int len = token.length;
  char *str = (char *)malloc(len + 1);
  memcpy(str, token.start, len);
  str[len] = '\0';
  return str;
}

// === Main Parse Function ===

StmtList *parse(Parser *parser) {
  StmtList *statements = createStmtList();

  while (!isAtEnd(parser)) {
    Stmt *decl = declaration(parser);
    if (decl) {
      appendStmt(statements, decl);
    }
  }

  return statements;
}

// === Declarations ===

static Stmt *declaration(Parser *p) {
  if (match(p, 1, TOKEN_FUNC))
    return funcDecl(p, "function");
  if (match(p, 1, TOKEN_CLASS))
    return classDecl(p);
  if (match(p, 1, TOKEN_USE))
    return useDecl(p);
  if (match(p, 2, TOKEN_LET, TOKEN_CONST))
    return varDecl(p);
  return statement(p);
}

static Stmt *funcDecl(Parser *p, const char *kind) {
  Token nameToken = consume(p, TOKEN_IDENTIFIER, "Expect function name.");
  char *name = tokenToString(nameToken);

  consume(p, TOKEN_LEFT_PAREN, "Expect '(' after name.");

  StringList *params = createStringList();
  if (!check(p, TOKEN_RIGHT_PAREN)) {
    do {
      Token param = consume(p, TOKEN_IDENTIFIER, "Expect parameter name.");
      char *paramName = tokenToString(param);
      appendString(params, paramName);
      free(paramName);
    } while (match(p, 1, TOKEN_COMMA));
  }

  consume(p, TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
  consume(p, TOKEN_LEFT_BRACE, "Expect '{' before body.");

  StmtList *body = block(p);

  Stmt *stmt = createFuncDeclStmt(name, params, body, nameToken.line, 0);
  free(name);
  return stmt;
}

static Stmt *classDecl(Parser *p) {
  Token nameToken = consume(p, TOKEN_IDENTIFIER, "Expect class name.");
  char *name = tokenToString(nameToken);

  VariableExpr *superclass = NULL;
  if (match(p, 1, TOKEN_EXTENDS)) {
    Token superToken = consume(p, TOKEN_IDENTIFIER, "Expect superclass name.");
    char *superName = tokenToString(superToken);
    Expr *superExpr = createVariableExpr(superName, superToken.line, 0);
    superclass = &superExpr->as.variable;
    free(superName);
  }

  consume(p, TOKEN_LEFT_BRACE, "Expect '{'.");

  StmtList *methods = createStmtList();
  while (!check(p, TOKEN_RIGHT_BRACE) && !isAtEnd(p)) {
    Stmt *method = funcDecl(p, "method");
    appendStmt(methods, method);
  }

  consume(p, TOKEN_RIGHT_BRACE, "Expect '}'.");

  Stmt *stmt =
      createClassDeclStmt(name, superclass, methods, nameToken.line, 0);
  free(name);
  return stmt;
}

static Stmt *varDecl(Parser *p) {
  bool is_const = (previous(p).type == TOKEN_CONST);
  Token nameToken = consume(p, TOKEN_IDENTIFIER, "Expect variable name.");
  char *name = tokenToString(nameToken);

  Expr *initializer = NULL;
  if (match(p, 1, TOKEN_EQUAL)) {
    initializer = expression(p);
  }

  consume(p, TOKEN_SEMICOLON, "Expect ';'.");

  Stmt *stmt =
      createVarDeclStmt(name, initializer, is_const, nameToken.line, 0);
  free(name);
  return stmt;
}

static Stmt *useDecl(Parser *p) {
  StringList *modules = createStringList();

  do {
    // Parse module path like "utils/io"
    if (!check(p, TOKEN_IDENTIFIER)) {
      parserError(p, "Expect module name.");
      break;
    }

    // Build module path
    char path[256] = "";
    Token part = advance(p);
    char *partStr = tokenToString(part);
    strcat(path, partStr);
    free(partStr);

    while (match(p, 1, TOKEN_SLASH)) {
      part = consume(p, TOKEN_IDENTIFIER, "Expect module part after '/'.");
      strcat(path, "/");
      partStr = tokenToString(part);
      strcat(path, partStr);
      free(partStr);
    }

    appendString(modules, path);
  } while (match(p, 1, TOKEN_COMMA));

  consume(p, TOKEN_SEMICOLON, "Expect ';'.");

  return createUseDeclStmt(modules, previous(p).line, 0);
}

// === Statements ===

static Stmt *statement(Parser *p) {
  if (match(p, 1, TOKEN_FOR))
    return forStmt(p);
  if (match(p, 1, TOKEN_IF))
    return ifStmt(p);
  if (match(p, 1, TOKEN_WHILE))
    return whileStmt(p);
  if (match(p, 1, TOKEN_SWITCH))
    return switchStmt(p);
  if (match(p, 1, TOKEN_TRY))
    return tryStmt(p);
  if (match(p, 1, TOKEN_RETURN))
    return returnStmt(p);
  if (match(p, 1, TOKEN_BREAK))
    return breakStmt(p);
  if (match(p, 1, TOKEN_CONTINUE))
    return continueStmt(p);
  if (match(p, 1, TOKEN_PRINT))
    return printStmt(p);
  if (match(p, 1, TOKEN_LEFT_BRACE)) {
    StmtList *stmts = block(p);
    return createBlockStmt(stmts, previous(p).line, 0);
  }
  return exprStmt(p);
}

static Stmt *forStmt(Parser *p) {
  consume(p, TOKEN_LEFT_PAREN, "Expect '('.");

  Stmt *initializer = NULL;
  if (match(p, 1, TOKEN_SEMICOLON)) {
    // No initializer
  } else if (match(p, 2, TOKEN_LET, TOKEN_CONST)) {
    initializer = varDecl(p);
  } else {
    initializer = exprStmt(p);
  }

  Expr *condition = NULL;
  if (!check(p, TOKEN_SEMICOLON)) {
    condition = expression(p);
  }
  consume(p, TOKEN_SEMICOLON, "Expect ';'.");

  Expr *increment = NULL;
  if (!check(p, TOKEN_RIGHT_PAREN)) {
    increment = expression(p);
  }
  consume(p, TOKEN_RIGHT_PAREN, "Expect ')'.");

  Stmt *body = statement(p);

  return createForStmt(initializer, condition, increment, body,
                       previous(p).line, 0);
}

static Stmt *ifStmt(Parser *p) {
  consume(p, TOKEN_LEFT_PAREN, "Expect '('.");
  Expr *condition = expression(p);
  consume(p, TOKEN_RIGHT_PAREN, "Expect ')'.");

  Stmt *thenBranch = statement(p);
  Stmt *elseBranch = NULL;
  if (match(p, 1, TOKEN_ELSE)) {
    elseBranch = statement(p);
  }

  return createIfStmt(condition, thenBranch, elseBranch, previous(p).line, 0);
}

static Stmt *whileStmt(Parser *p) {
  consume(p, TOKEN_LEFT_PAREN, "Expect '('.");
  Expr *condition = expression(p);
  consume(p, TOKEN_RIGHT_PAREN, "Expect ')'.");
  Stmt *body = statement(p);

  return createWhileStmt(condition, body, previous(p).line, 0);
}

static Stmt *switchStmt(Parser *p) {
  consume(p, TOKEN_LEFT_PAREN, "Expect '('.");
  Expr *value = expression(p);
  consume(p, TOKEN_RIGHT_PAREN, "Expect ')'.");
  consume(p, TOKEN_LEFT_BRACE, "Expect '{'.");

  SwitchCaseList *cases = createSwitchCaseList();
  StmtList *defaultCase = NULL;

  while (!check(p, TOKEN_RIGHT_BRACE) && !isAtEnd(p)) {
    if (match(p, 1, TOKEN_CASE)) {
      Expr *caseValue = expression(p);
      consume(p, TOKEN_COLON, "Expect ':'.");

      StmtList *caseStmts = createStmtList();
      while (!check(p, TOKEN_CASE) && !check(p, TOKEN_DEFAULT) &&
             !check(p, TOKEN_RIGHT_BRACE)) {
        appendStmt(caseStmts, statement(p));
      }

      appendSwitchCase(cases, caseValue, caseStmts);
    } else if (match(p, 1, TOKEN_DEFAULT)) {
      consume(p, TOKEN_COLON, "Expect ':'.");

      defaultCase = createStmtList();
      while (!check(p, TOKEN_CASE) && !check(p, TOKEN_DEFAULT) &&
             !check(p, TOKEN_RIGHT_BRACE)) {
        appendStmt(defaultCase, statement(p));
      }
    }
  }

  consume(p, TOKEN_RIGHT_BRACE, "Expect '}'.");

  return createSwitchStmt(value, cases, defaultCase, previous(p).line, 0);
}

static Stmt *tryStmt(Parser *p) {
  consume(p, TOKEN_LEFT_BRACE, "Expect '{'.");
  StmtList *tryBlock = block(p);

  StmtList *catchBlock = createStmtList();
  char *catchVar = strdup("err");
  StmtList *finallyBlock = NULL;

  if (match(p, 1, TOKEN_CATCH)) {
    if (match(p, 1, TOKEN_LEFT_PAREN)) {
      Token varToken =
          consume(p, TOKEN_IDENTIFIER, "Expect error variable name.");
      free(catchVar);
      catchVar = tokenToString(varToken);
      consume(p, TOKEN_RIGHT_PAREN, "Expect ')'.");
    }
    consume(p, TOKEN_LEFT_BRACE, "Expect '{'.");
    catchBlock = block(p);
  }

  if (match(p, 1, TOKEN_FINALLY)) {
    consume(p, TOKEN_LEFT_BRACE, "Expect '{'.");
    finallyBlock = block(p);
  }

  Stmt *stmt = createTryCatchStmt(tryBlock, catchVar, catchBlock, finallyBlock,
                                  previous(p).line, 0);
  free(catchVar);
  return stmt;
}

static Stmt *returnStmt(Parser *p) {
  Token keyword = previous(p);
  Expr *value = NULL;
  if (!check(p, TOKEN_SEMICOLON)) {
    value = expression(p);
  }
  consume(p, TOKEN_SEMICOLON, "Expect ';'.");

  return createReturnStmt(value, keyword.line, 0);
}

static Stmt *breakStmt(Parser *p) {
  Token keyword = previous(p);
  consume(p, TOKEN_SEMICOLON, "Expect ';'.");
  return createBreakStmt(keyword.line, 0);
}

static Stmt *continueStmt(Parser *p) {
  Token keyword = previous(p);
  consume(p, TOKEN_SEMICOLON, "Expect ';'.");
  return createContinueStmt(keyword.line, 0);
}

static StmtList *block(Parser *p) {
  StmtList *statements = createStmtList();

  while (!check(p, TOKEN_RIGHT_BRACE) && !isAtEnd(p)) {
    Stmt *decl = declaration(p);
    if (decl) {
      appendStmt(statements, decl);
    }
  }

  consume(p, TOKEN_RIGHT_BRACE, "Expect '}'.");
  return statements;
}

static Stmt *printStmt(Parser *p) {
  Expr *value = expression(p);
  consume(p, TOKEN_SEMICOLON, "Expect ';' after value.");
  return createPrintStmt(value, previous(p).line, 0);
}

static Stmt *exprStmt(Parser *p) {
  Expr *expr = expression(p);
  consume(p, TOKEN_SEMICOLON, "Expect ';'.");
  return createExpressionStmt(expr, previous(p).line, 0);
}

// === Expressions ===

static Expr *expression(Parser *p) { return assignment(p); }

static Expr *assignment(Parser *p) {
  Expr *expr = ternary(p);

  if (match(p, 3, TOKEN_EQUAL, TOKEN_PLUS_EQUAL, TOKEN_MINUS_EQUAL)) {
    Token equals = previous(p);
    Expr *value = assignment(p);

    if (expr->type == EXPR_VARIABLE) {
      return createAssignExpr(expr->as.variable.name, value, equals.line, 0);
    }
  }

  return expr;
}

static Expr *ternary(Parser *p) {
  Expr *expr = orExpr(p);

  if (match(p, 1, TOKEN_QUESTION)) {
    Expr *trueBranch = expression(p);
    consume(p, TOKEN_COLON, "Expect ':'.");
    Expr *falseBranch = expression(p);
    return createTernaryExpr(expr, trueBranch, falseBranch, previous(p).line,
                             0);
  }

  return expr;
}

static Expr *orExpr(Parser *p) {
  Expr *expr = andExpr(p);

  while (match(p, 1, TOKEN_PIPE_PIPE)) {
    Token op = previous(p);
    char *opStr = tokenToString(op);
    Expr *right = andExpr(p);
    expr = createLogicalExpr(expr, opStr, right, op.line, 0);
    free(opStr);
  }

  return expr;
}

static Expr *andExpr(Parser *p) {
  Expr *expr = equality(p);

  while (match(p, 1, TOKEN_AMPERSAND_AMPERSAND)) {
    Token op = previous(p);
    char *opStr = tokenToString(op);
    Expr *right = equality(p);
    expr = createLogicalExpr(expr, opStr, right, op.line, 0);
    free(opStr);
  }

  return expr;
}

static Expr *equality(Parser *p) {
  Expr *expr = comparison(p);

  while (match(p, 2, TOKEN_EQUAL_EQUAL, TOKEN_BANG_EQUAL)) {
    Token op = previous(p);
    char *opStr = tokenToString(op);
    Expr *right = comparison(p);
    expr = createBinaryExpr(expr, opStr, right, op.line, 0);
    free(opStr);
  }

  return expr;
}

static Expr *comparison(Parser *p) {
  Expr *expr = term(p);

  while (match(p, 4, TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS,
               TOKEN_LESS_EQUAL)) {
    Token op = previous(p);
    char *opStr = tokenToString(op);
    Expr *right = term(p);
    expr = createBinaryExpr(expr, opStr, right, op.line, 0);
    free(opStr);
  }

  return expr;
}

static Expr *term(Parser *p) {
  Expr *expr = factor(p);

  while (match(p, 2, TOKEN_PLUS, TOKEN_MINUS)) {
    Token op = previous(p);
    char *opStr = tokenToString(op);
    Expr *right = factor(p);
    expr = createBinaryExpr(expr, opStr, right, op.line, 0);
    free(opStr);
  }

  return expr;
}

static Expr *factor(Parser *p) {
  Expr *expr = unary(p);

  while (match(p, 4, TOKEN_SLASH, TOKEN_STAR, TOKEN_PERCENT, TOKEN_STAR_STAR)) {
    Token op = previous(p);
    char *opStr = tokenToString(op);
    Expr *right = unary(p);
    expr = createBinaryExpr(expr, opStr, right, op.line, 0);
    free(opStr);
  }

  return expr;
}

static Expr *unary(Parser *p) {
  if (match(p, 2, TOKEN_BANG, TOKEN_MINUS)) {
    Token op = previous(p);
    char *opStr = tokenToString(op);
    Expr *right = unary(p);
    Expr *expr = createUnaryExpr(opStr, right, op.line, 0);
    free(opStr);
    return expr;
  }

  return call(p);
}

static Expr *call(Parser *p) {
  Expr *expr = primary(p);

  while (true) {
    if (match(p, 1, TOKEN_LEFT_PAREN)) {
      ExprList *arguments = createExprList();
      if (!check(p, TOKEN_RIGHT_PAREN)) {
        do {
          appendExpr(arguments, expression(p));
        } while (match(p, 1, TOKEN_COMMA));
      }
      consume(p, TOKEN_RIGHT_PAREN, "Expect ')'.");
      expr = createCallExpr(expr, arguments, previous(p).line, 0);
    } else if (match(p, 1, TOKEN_DOT)) {
      Token name = consume(p, TOKEN_IDENTIFIER, "Expect property name.");
      char *nameStr = tokenToString(name);
      expr = createGetExpr(expr, nameStr, name.line, 0);
      free(nameStr);
    } else if (match(p, 1, TOKEN_LEFT_BRACKET)) {
      Expr *index = expression(p);
      consume(p, TOKEN_RIGHT_BRACKET, "Expect ']'.");
      expr = createIndexExpr(expr, index, previous(p).line, 0);
    } else {
      break;
    }
  }

  return expr;
}

static Expr *primary(Parser *p) {
  if (match(p, 1, TOKEN_FALSE)) {
    return createLiteralExpr(BOOL_VAL(false), previous(p).line, 0);
  }
  if (match(p, 1, TOKEN_TRUE)) {
    return createLiteralExpr(BOOL_VAL(true), previous(p).line, 0);
  }
  if (match(p, 1, TOKEN_NULL)) {
    return createLiteralExpr(NULL_VAL, previous(p).line, 0);
  }

  if (match(p, 1, TOKEN_NUMBER)) {
    Token token = previous(p);
    char *numStr = tokenToString(token);
    double value = strtod(numStr, NULL);
    free(numStr);
    return createLiteralExpr(NUMBER_VAL(value), token.line, 0);
  }

  if (match(p, 1, TOKEN_STRING)) {
    Token token = previous(p);
    // Extract string without quotes
    int len = token.length - 2;
    char *str = (char *)malloc(len + 1);
    memcpy(str, token.start + 1, len);
    str[len] = '\0';
    Expr *expr =
        createLiteralExpr(OBJ_VAL(copyString(str, len)), token.line, 0);
    free(str);
    return expr;
  }

  if (match(p, 1, TOKEN_IDENTIFIER)) {
    Token token = previous(p);
    char *name = tokenToString(token);
    Expr *expr = createVariableExpr(name, token.line, 0);
    free(name);
    return expr;
  }

  if (match(p, 1, TOKEN_LEFT_PAREN)) {
    Expr *expr = expression(p);
    consume(p, TOKEN_RIGHT_PAREN, "Expect ')'.");
    return createGroupingExpr(expr, previous(p).line, 0);
  }

  if (match(p, 1, TOKEN_LEFT_BRACKET)) {
    ExprList *elements = createExprList();
    if (!check(p, TOKEN_RIGHT_BRACKET)) {
      do {
        appendExpr(elements, expression(p));
      } while (match(p, 1, TOKEN_COMMA));
    }
    consume(p, TOKEN_RIGHT_BRACKET, "Expect ']'.");
    return createListExpr(elements, previous(p).line, 0);
  }

  if (match(p, 1, TOKEN_LEFT_BRACE)) {
    DictPairList *pairs = createDictPairList();
    if (!check(p, TOKEN_RIGHT_BRACE)) {
      do {
        Expr *key = expression(p);
        consume(p, TOKEN_COLON, "Expect ':'.");
        Expr *value = expression(p);
        appendDictPair(pairs, key, value);
      } while (match(p, 1, TOKEN_COMMA));
    }
    consume(p, TOKEN_RIGHT_BRACE, "Expect '}'.");
    return createDictionaryExpr(pairs, previous(p).line, 0);
  }

  parserError(p, "Expect expression.");
  return NULL;
}
