#ifndef PROXPL_ERROR_REPORT_H
#define PROXPL_ERROR_REPORT_H

#include "common.h"
#include "scanner.h"

// Standard Error Codes
#define ERR_SYNTAX            "E0001"
#define ERR_UNEXPECTED_TOKEN  "E0002"
#define ERR_TYPE_MISMATCH     "E0308"
#define ERR_UNDEFINED_VAR     "E0412"
#define ERR_UNKNOWN_SYMBOL    "E0425"
#define ERR_NO_METHOD         "E0599"
#define ERR_RUNTIME           "E0999"

// Find closest matching candidate name for typo suggestions
const char* findTypoSuggestion(const char* target, const char** candidates, int candidateCount);

// Report a structured diagnostic with error code, location, snippet, caret and suggestion
void reportDiagnostic(const char* code, const char* source, int line, int col, int length, const char* message, const char* suggestion);

// Report a compile-time error with code context
void reportCompileError(const char* source, Token token, const char* message);

// Report a runtime error with code context
void reportRuntimeError(const char* source, int line, const char* message);

#endif // PROXPL_ERROR_REPORT_H
