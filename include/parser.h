// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#ifndef PROX_PARSER_H
#define PROX_PARSER_H

#include "ast.h"
#include "common.h"
#include "scanner.h"


// Parser structure
typedef struct {
  Token *tokens;
  int count;
  int current;
  bool panicMode;
  bool hadError;
  const char *source;
} Parser;

// Initialize parser with token array and source code
void initParser(Parser *parser, Token *tokens, int count, const char *source);

// Main parse function - returns list of statements (program)
StmtList *parse(Parser *parser);

// Error handling
void parserError(Parser *parser, const char *message);

#endif
