// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-22
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/common.h"
#include "../../include/importer.h"
#include "../../include/memory.h"
#include "../../include/object.h"
#include "../../include/vm.h"

// Access VM for string interning and memory
extern VM vm;

void initImporter(Importer *importer) {
  importer->searchPaths = NULL;
  importer->pathCount = 0;
  initTable(&importer->modules);
  
  // Add default search path "."
  addSearchPath(importer, ".");
}

void addSearchPath(Importer *importer, const char *path) {
    importer->searchPaths = (char**)realloc(importer->searchPaths, sizeof(char*) * (importer->pathCount + 1));
    
    // Duplicate string
    size_t len = strlen(path);
    char* pathCopy = (char*)malloc(len + 1);
    strcpy(pathCopy, path);
    
    importer->searchPaths[importer->pathCount++] = pathCopy;
}

bool loadModule(Importer *importer, const char *moduleName, void** result) {
    // 1. Strings must be interned for table lookups
    ObjString* nameObj = copyString(moduleName, strlen(moduleName));
    push(&vm, OBJ_VAL(nameObj)); // GC Protection
    
    // 2. Check Cache
    Value cached;
    if (tableGet(&importer->modules, nameObj, &cached)) {
        *result = AS_MODULE(cached);
        pop(&vm); // nameObj
        return true;
    }
    
    // 3. Create new module (Stub for now, or real logic)
    // For Phase 1, we assume everything is native registered elsewhere 
    // OR we create empty modules here to be populated.
    
    // We expect native modules to be PRE-REGISTERED in the cache during startup.
    // If not found, we try file resolution (TODO).
    
    pop(&vm); // nameObj
    return false;
}

void freeImporter(Importer *importer) {
    for (int i = 0; i < importer->pathCount; i++) {
        free(importer->searchPaths[i]);
    }
    free(importer->searchPaths);
    freeTable(&importer->modules);
}
