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

bool addSearchPath(Importer *importer, const char *path) {
    char** newPaths = (char**)realloc(importer->searchPaths, sizeof(char*) * (importer->pathCount + 1));
    if (!newPaths) return false;
    importer->searchPaths = newPaths;
    
    // Duplicate string
    size_t len = strlen(path);
    char* pathCopy = (char*)malloc(len + 1);
    if (!pathCopy) return false;
    strcpy(pathCopy, path);
    
    importer->searchPaths[importer->pathCount++] = pathCopy;
    return true;
}

bool loadModule(Importer *importer, const char *moduleName, void** result) {
    // 1. Strings must be interned for table lookups
    ObjString* nameObj = copyString(moduleName, strlen(moduleName));
    push(&vm, OBJ_VAL(nameObj)); // GC Protection

    // 2. Check Cache
    Value cached;
    if (tableGet(&importer->modules, nameObj, &cached)) {
        if (IS_MODULE(cached)) {
            *result = AS_MODULE(cached);
            pop(&vm); // nameObj
            return true;
        }
    }

    // 3. Module not in cache - try to create it as a native module
    // Create an empty module for native modules registered at startup
    ObjModule* mod = newModule(nameObj);
    if (mod != NULL) {
        tableSet(&importer->modules, nameObj, OBJ_VAL(mod));
        *result = mod;
        pop(&vm); // nameObj
        return true;
    }

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
