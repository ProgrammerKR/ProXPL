// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/*
 * Module Importer for ProXPL
 * Handles "use" keyword and module loading
 *
 * STUB IMPLEMENTATION - Basic functionality only
 * Full implementation would include:
 * - Circular dependency detection
 * - Module caching
 * - Search path resolution
 * - Package management integration
 */

#ifndef PROX_IMPORTER_H
#define PROX_IMPORTER_H

#include "common.h"
#include "parser.h"

typedef struct {
  char **searchPaths;
  int pathCount;
  char **loadedModules;
  int loadedCount;
} Importer;

// Initialize importer with search paths
void initImporter(Importer *importer);

// Load a module by name (returns statements)
StmtList *loadModule(Importer *importer, const char *moduleName);

// Resolve module path
char *resolveModulePath(Importer *importer, const char *moduleName);

// Check if module is already loaded
bool isModuleLoaded(Importer *importer, const char *path);

// Free importer resources
void freeImporter(Importer *importer);

#endif
