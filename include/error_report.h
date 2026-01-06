#ifndef PROXPL_ERROR_REPORT_H
#define PROXPL_ERROR_REPORT_H

#include "common.h"
#include "scanner.h"

// Report a compile-time error with code context
void reportCompileError(const char* source, Token token, const char* message);

// Report a runtime error with code context
void reportRuntimeError(const char* source, int line, const char* message);

#endif // PROXPL_ERROR_REPORT_H
