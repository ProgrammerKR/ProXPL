// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
// --------------------------------------------------

#include "../../include/object.h"
#include "../../include/value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Value native_db_connect(int argCount, Value* args) {
    if (argCount != 1 || !IS_STRING(args[0])) {
        return NULL_VAL;
    }
    printf("[DB] Connected to %s\n", AS_CSTRING(args[0]));
    return NUMBER_VAL(1); // Dummy connection ID
}

static Value native_db_query(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) {
        return NULL_VAL;
    }
    printf("[DB] Executing query: %s\n", AS_CSTRING(args[0]));
    return NULL_VAL; // Dummy result
}

ObjModule* create_std_db_module() {
    ObjString* name = copyString("std.native.db", 13);
    ObjModule* module = newModule(name);
    
    // Register native functions
    ObjString* connectName = copyString("connect", 7);
    tableSet(&module->exports, connectName, OBJ_VAL(newNative(native_db_connect)));
    
    ObjString* queryName = copyString("query", 5);
    tableSet(&module->exports, queryName, OBJ_VAL(newNative(native_db_query)));
    
    return module;
}
