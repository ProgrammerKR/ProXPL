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
} Parser;

// Initialize parser with token array
void initParser(Parser *parser, Token *tokens, int count);

// Main parse function - returns list of statements (program)
StmtList *parse(Parser *parser);

// Error handling
void parserError(Parser *parser, const char *message);

#endif
