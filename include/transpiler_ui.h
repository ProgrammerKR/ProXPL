#ifndef PROXPL_TRANSPILER_UI_H
#define PROXPL_TRANSPILER_UI_H

#include "ast.h"
#include <stdio.h>

// Transpile a UI App node to HTML/JS files
void transpileUIApp(Stmt *appStmt, const char *outputDir);

#endif
