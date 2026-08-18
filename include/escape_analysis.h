#ifndef PROX_ESCAPE_ANALYSIS_H
#define PROX_ESCAPE_ANALYSIS_H

#include "ast.h"
#include <stdbool.h>

// Escape Analysis info for closures and functions
typedef enum {
    ESCAPE_NONE,      // Non-escaping: stack allocation eligible, zero GC pressure
    ESCAPE_ARGUMENT,  // Passed as non-escaping argument to local function
    ESCAPE_GLOBAL,    // Escapes to global variable
    ESCAPE_RETURN,    // Escapes via return value
    ESCAPE_HEAP       // Stored into heap object (instance, list, dictionary)
} EscapeState;

// Analyzes an AST expression or function declaration to determine its escape state
EscapeState analyzeClosureEscape(Stmt* funcDecl);
EscapeState analyzeLambdaEscape(Expr* lambdaExpr, StmtList* enclosingBody);

// Returns true if the closure never escapes its lexical scope
bool isClosureStackEligible(Stmt* funcDecl);

#endif // PROX_ESCAPE_ANALYSIS_H
