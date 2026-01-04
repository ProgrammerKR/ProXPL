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
static Stmt *funcDecl(Parser *p, const char *kind, bool isAsync, AccessLevel access, bool isStatic, bool isAbstract);
static Stmt *classDecl(Parser *p);
static Stmt *interfaceDecl(Parser *p);
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
static bool check(Parser *p, PxTokenType type);
static bool match(Parser *p, int count, ...);
static Token consume(Parser *p, PxTokenType type, const char *message);


// === Initialization ===

void initParser(Parser *parser, Token *tokens, int count, const char *source) {
  parser->tokens = tokens;
  parser->count = count;
  parser->current = 0;
  parser->panicMode = false;
  parser->hadError = false;
  parser->source = source;
}

static void printErrorContext(const char* source, int line, int column) {
    // Find line start
    const char* lineStart = source;
    int currentLine = 1;
    while (currentLine < line && *lineStart != '\0') {
        if (*lineStart == '\n') currentLine++;
        lineStart++;
    }
    
    // Find line end
    const char* lineEnd = lineStart;
    while (*lineEnd != '\0' && *lineEnd != '\n') {
        lineEnd++;
    }
    
    // Print line
    int len = (int)(lineEnd - lineStart);
    fprintf(stderr, "%.*s\n", len, lineStart);
    
    // Print caret
    for(int i = 1; i < column; i++) fprintf(stderr, " ");
    fprintf(stderr, "^\n");
}

void parserError(Parser *parser, const char *message) {
  if (parser->panicMode) return;
  parser->panicMode = true;
  parser->hadError = true;
  Token token = peek(parser);
  fprintf(stderr, "ParseError: %s at line %d, column %d\n", message, token.line, token.column);
  
  if (parser->source) {
      printErrorContext(parser->source, token.line, token.column);
  }
}

static void synchronize(Parser *p) {
  p->panicMode = false;

  while (!isAtEnd(p)) {
    if (previous(p).type == TOKEN_SEMICOLON) return;

    switch (peek(p).type) {
      case TOKEN_CLASS:
      case TOKEN_FUNC:
      case TOKEN_CONST:
      case TOKEN_LET:
      case TOKEN_FOR:
      case TOKEN_IF:
      case TOKEN_WHILE:
      case TOKEN_PRINT:
      case TOKEN_RETURN:
      case TOKEN_SWITCH:
      case TOKEN_TRY:
      case TOKEN_ASYNC: // Assuming this is also a sync point
        return;

      default:
        ;
    }

    advance(p);
  }
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

static bool check(Parser *p, PxTokenType type) {
  if (isAtEnd(p))
    return false;
  return peek(p).type == type;
}

static bool match(Parser *p, int count, ...) {
  va_list args;
  va_start(args, count);

  for (int i = 0; i < count; i++) {
    PxTokenType type = va_arg(args, PxTokenType);
    if (check(p, type)) {
      advance(p);
      va_end(args);
      return true;
    }
  }

  va_end(args);
  return false;
}

static Token consume(Parser *p, PxTokenType type, const char *message) {
  if (check(p, type))
    return advance(p);
  parserError(p, message);
  return peek(p); // Return current token on error
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
    } else if (parser->panicMode) {
      synchronize(parser);
      // Removed 'decl' but no need to free as it's null.
    }
  }

  return statements;
}

// === Declarations ===

static Stmt *externDecl(Parser *p);

static Stmt *declaration(Parser *p) {
  if (match(p, 1, TOKEN_ASYNC)) {
      if (match(p, 1, TOKEN_FUNC))
          return funcDecl(p, "function", true, ACCESS_PUBLIC, false, false);
      // TODO: Handle async arrow functions if supported later
      parserError(p, "Expect 'func' after 'async'.");
      return NULL;
  }
  if (match(p, 1, TOKEN_FUNC))
    return funcDecl(p, "function", false, ACCESS_PUBLIC, false, false);
  if (match(p, 1, TOKEN_CLASS))
    return classDecl(p);
  if (match(p, 1, TOKEN_EXTERN))
    return externDecl(p);
  if (match(p, 1, TOKEN_INTERFACE))
    return interfaceDecl(p);
  if (match(p, 1, TOKEN_USE))
    return useDecl(p);
  if (match(p, 2, TOKEN_LET, TOKEN_CONST))
    return varDecl(p);
  return statement(p);
}

static Stmt *funcDecl(Parser *p, const char *kind, bool isAsync, AccessLevel access, bool isStatic, bool isAbstract) {
  (void)kind;
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

  StmtList *body = NULL;
  // If abstract or interface method, body is optional (expect semicolon)
  if (isAbstract && match(p, 1, TOKEN_SEMICOLON)) {
      // No body
  } else {
      consume(p, TOKEN_LEFT_BRACE, "Expect '{' before body.");
      body = block(p);
  }

  Stmt *stmt = createFuncDeclStmt(name, params, body, isAsync, access, isStatic, isAbstract, nameToken.line, 0);
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
  
  StringList *interfaces = NULL;
  if (match(p, 1, TOKEN_IMPLEMENTS)) {
      interfaces = createStringList();
      do {
          Token interfaceName = consume(p, TOKEN_IDENTIFIER, "Expect interface name.");
          char *iName = tokenToString(interfaceName);
          appendString(interfaces, iName);
          free(iName);
      } while (match(p, 1, TOKEN_COMMA));
  }

  consume(p, TOKEN_LEFT_BRACE, "Expect '{'.");

  StmtList *methods = createStmtList();
  while (!check(p, TOKEN_RIGHT_BRACE) && !isAtEnd(p)) {
    AccessLevel access = ACCESS_PUBLIC;
    if (match(p, 1, TOKEN_PUBLIC)) access = ACCESS_PUBLIC;
    else if (match(p, 1, TOKEN_PRIVATE)) access = ACCESS_PRIVATE;
    else if (match(p, 1, TOKEN_PROTECTED)) access = ACCESS_PROTECTED;
    
    bool isStatic = false;
    if (match(p, 1, TOKEN_STATIC)) isStatic = true;
    
    bool isAbstract = false;
    if (match(p, 1, TOKEN_ABSTRACT)) isAbstract = true;

    bool isAsync = false; 
    if (match(p, 1, TOKEN_ASYNC)) {
        isAsync = true;
    }
    
    Stmt *method = funcDecl(p, "method", isAsync, access, isStatic, isAbstract);
    appendStmt(methods, method);
  }

  consume(p, TOKEN_RIGHT_BRACE, "Expect '}'.");

  Stmt *stmt =
      createClassDeclStmt(name, superclass, interfaces, methods, nameToken.line, 0);
  free(name);
  return stmt;
}

static Stmt *interfaceDecl(Parser *p) {
    Token nameToken = consume(p, TOKEN_IDENTIFIER, "Expect interface name.");
    char *name = tokenToString(nameToken);
    
    consume(p, TOKEN_LEFT_BRACE, "Expect '{'.");
    
    StmtList *methods = createStmtList();
    while (!check(p, TOKEN_RIGHT_BRACE) && !isAtEnd(p)) {
        // Interface methods are public abstract by default
        bool isAsync = false;
        if (match(p, 1, TOKEN_ASYNC)) isAsync = true;
        
        // Pass isAbstract = true to allow semicolon body
        Stmt *method = funcDecl(p, "method", isAsync, ACCESS_PUBLIC, false, true);
        appendStmt(methods, method);
    }
    
    consume(p, TOKEN_RIGHT_BRACE, "Expect '}'.");
    
    Stmt *stmt = createInterfaceDeclStmt(name, methods, nameToken.line, 0);
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

    while (match(p, 1, TOKEN_SLASH) || match(p, 1, TOKEN_DOT)) {
      part = consume(p, TOKEN_IDENTIFIER, "Expect module part after separator.");
      strcat(path, "."); // Normalize to dot internally? Or keep as is? 
      // The user uses dots (std.io). PROX might use dots or slashes.
      // Let's standardise to dots for internal representation if that's what `vm` expects?
      // `stdlib_core.c` registers "std.native.io".
      // If user types `use std.io`, we get `std.io`.
      // If user types `use std/io`, we get `std.io` if we normalize?
      // The original code was: `strcat(path, "/");`
      // Let's change it to just append the separator found? 
      // Actually `stdlib_core.c` uses dots. So let's normalize to dots.
      partStr = tokenToString(part);
      strcat(path, partStr);
      free(partStr);
    }

    appendString(modules, path);
  } while (match(p, 1, TOKEN_COMMA));

  consume(p, TOKEN_SEMICOLON, "Expect ';'.");

  return createUseDeclStmt(modules, previous(p).line, 0);
}

static Stmt *externDecl(Parser *p) {
    Token libToken = consume(p, TOKEN_STRING, "Expect library path string.");
    char *libPath = tokenToString(libToken);
    
    Token symToken = consume(p, TOKEN_STRING, "Expect symbol name string.");
    char *symName = tokenToString(symToken);
    
    consume(p, TOKEN_FUNC, "Expect 'func' after extern strings.");
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
    consume(p, TOKEN_SEMICOLON, "Expect ';' after extern declaration.");
    
    Stmt *stmt = createExternDeclStmt(libPath, symName, name, params, libToken.line, 0);
    free(libPath);
    free(symName);
    free(name);
    return stmt;
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
  if (match(p, 1, TOKEN_AWAIT)) {
      Token op = previous(p);
      Expr *right = unary(p);
      return createAwaitExpr(right, op.line, 0);
  }
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
      // Allow keywords as property names (e.g. std.io.print)
      Token name;
      if (check(p, TOKEN_IDENTIFIER)) {
          name = advance(p);
      } else if (check(p, TOKEN_CLASS) || check(p, TOKEN_THIS) ||
                 check(p, TOKEN_SUPER) || check(p, TOKEN_TRUE) ||
                 check(p, TOKEN_FALSE) || check(p, TOKEN_NULL) ||
                 check(p, TOKEN_OR) || check(p, TOKEN_AND) ||
                 check(p, TOKEN_LET) ||
                 check(p, TOKEN_CONST) || check(p, TOKEN_IF) ||
                 check(p, TOKEN_ELSE) || check(p, TOKEN_WHILE) ||
                 check(p, TOKEN_FOR) || check(p, TOKEN_BREAK) ||
                 check(p, TOKEN_CONTINUE) || check(p, TOKEN_RETURN) ||
                 check(p, TOKEN_FUNC) || check(p, TOKEN_PRINT) ||
                 check(p, TOKEN_USE) || check(p, TOKEN_FROM) ||
                 check(p, TOKEN_ASYNC) || check(p, TOKEN_AWAIT) ||
                 check(p, TOKEN_TRY) || check(p, TOKEN_CATCH) ||
                 check(p, TOKEN_FINALLY) || check(p, TOKEN_THROW) ||
                 check(p, TOKEN_SWITCH) || check(p, TOKEN_CASE) ||
                 check(p, TOKEN_DEFAULT) || check(p, TOKEN_EXTENDS)) {
          // It's a keyword, but we want to use it as an identifier here.
          name = advance(p);
      } else {
          parserError(p, "Expect property name.");
          name = peek(p); // Recover
      }
      
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
