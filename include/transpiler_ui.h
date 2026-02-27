#ifndef PROXPL_TRANSPILER_UI_H
#define PROXPL_TRANSPILER_UI_H

#include "ast.h"
#include <stdio.h>

// Transpile a UI App node to HTML/JS files (basic mode, used by runFile)
void transpileUIApp(Stmt *appStmt, const char *outputDir);

// Transpile a UI App node to a full web dist folder: index.html + style.css + app.js
// Used by: prm build web [--output <dir>]
void transpileUIAppWeb(Stmt *appStmt, const char *outputDir);

#endif
