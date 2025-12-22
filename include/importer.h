// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#ifndef PROX_IMPORTER_H
#define PROX_IMPORTER_H

#include "common.h"
#include "parser.h"
#include "table.h"

// Importer State
typedef struct {
  char **searchPaths;
  int pathCount;
  
  // Cache of loaded modules (Name -> ObjModule*)
  Table modules; 
} Importer;

// Initialize importer with default search paths
void initImporter(Importer *importer);

// Load a module by name (e.g., "std.io" or "utils")
// Returns true if successful, populates *result with the Module object
bool loadModule(Importer *importer, const char *moduleName, void** result);

// Add a directory to the module search path
void addSearchPath(Importer *importer, const char *path);

// Free importer resources
void freeImporter(Importer *importer);

#endif
