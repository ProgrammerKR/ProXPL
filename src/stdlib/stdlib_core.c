// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

/*
 * ProXPL Standard Library - Main Registry
 * Registers all standard library modules with the VM
 */

#include "../include/common.h"
#include "../include/vm.h"
#include "../include/object.h"
#include "../include/memory.h"

// Forward declarations for module creators
extern ObjModule* create_std_io_module();
extern ObjModule* create_std_fs_module();
extern ObjModule* create_std_sys_module();
extern ObjModule* create_std_core_module();
extern ObjModule* create_std_math_module();
extern ObjModule* create_std_str_module();
extern ObjModule* create_std_time_module();
extern ObjModule* create_std_json_module();
extern ObjModule* create_std_os_module();
extern ObjModule* create_std_hash_module();
extern ObjModule* create_std_net_module();
extern ObjModule* create_std_collections_module();
extern ObjModule* create_std_reflect_module();
extern ObjModule* create_std_gc_module();
extern ObjModule* create_std_buffer_module();
extern ObjModule* create_std_process_module();
extern ObjModule* create_std_path_module();

// Legacy
extern void register_math_natives(VM* vm);
extern void register_math_globals(VM* vm);
extern void register_string_natives(VM* vm);
extern void register_string_globals(VM* vm);
extern void register_convert_natives(VM* vm);
extern void register_system_natives(VM* vm);
extern void register_io_globals(VM* vm);

// Exposed natives
extern Value native_clock(int argCount, Value* args);
extern Value nativeLoadConfig(int argCount, Value *args);


static Value native_len(int argCount, Value* args) {
    if (argCount < 1) return NUMBER_VAL(0);
    if (IS_STRING(args[0])) return NUMBER_VAL((double)AS_STRING(args[0])->length);
    if (IS_LIST(args[0])) {
         return NUMBER_VAL((double)AS_LIST(args[0])->count);
    }
    if (IS_DICTIONARY(args[0])) return NUMBER_VAL((double)AS_DICTIONARY(args[0])->items.count);
    return NUMBER_VAL(0);
}

static ObjModule* create_empty_module(VM* pVM, const char* name) {
    ObjString* nameStr = copyString(name, (int)strlen(name));
    push(pVM, OBJ_VAL(nameStr));
    ObjModule* module = newModule(nameStr);
    pop(pVM);
    return module;
}

static void registerModule(VM* pVM, const char* name, ObjModule* module) {
    ObjString* nameObj = copyString(name, (int)strlen(name));
    push(pVM, OBJ_VAL(nameObj));
    push(pVM, OBJ_VAL(module));
    tableSet(&pVM->importer.modules, nameObj, peek(pVM, 0));
    pop(pVM);
    pop(pVM);
}

static Value native_push(int argCount, Value* args) {
    if (argCount < 2) return NIL_VAL;
    if (!IS_LIST(args[0])) return NIL_VAL;
    ObjList* list = AS_LIST(args[0]);
    Value item = args[1];
    if (list->capacity < list->count + 1) {
        int oldCapacity = list->capacity;
        list->capacity = GROW_CAPACITY(oldCapacity);
        list->items = GROW_ARRAY(Value, list->items, oldCapacity, list->capacity);
    }
    list->items[list->count++] = item;
    return item; 
}

static Value native_pop(int argCount, Value* args) {
    if (argCount < 1) return NIL_VAL;
    if (!IS_LIST(args[0])) return NIL_VAL;
    ObjList* list = AS_LIST(args[0]);
    if (list->count == 0) return NIL_VAL;
    return list->items[--list->count];
}

static Value native_substr(int argCount, Value* args) {
    if (argCount < 3) return NIL_VAL;
    if (!IS_STRING(args[0]) || !IS_NUMBER(args[1]) || !IS_NUMBER(args[2])) {
        return NIL_VAL;
    }
    
    ObjString* source = AS_STRING(args[0]);
    int start = (int)AS_NUMBER(args[1]);
    int length = (int)AS_NUMBER(args[2]);
    
    if (start < 0 || start >= source->length) {
        return OBJ_VAL(copyString("", 0));
    }
    if (length < 0) length = 0;
    if (start + length > source->length) length = source->length - start;
    
    ObjString* result = copyString(source->chars + start, length);
    return OBJ_VAL(result);
}

/*
 * Register all standard library modules
 * Called during VM initialization
 */
void registerStdLib(VM* pVM) {
    // New Module System + Aliases
    ObjModule* ioMod = create_std_io_module();
    registerModule(pVM, "std.native.io", ioMod);
    registerModule(pVM, "std.io", ioMod);

    ObjModule* fsMod = create_std_fs_module();
    registerModule(pVM, "std.native.fs", fsMod);
    registerModule(pVM, "std.fs", fsMod);

    ObjModule* sysMod = create_std_sys_module();
    registerModule(pVM, "std.native.sys", sysMod);
    registerModule(pVM, "std.sys", sysMod);
    
    ObjModule* mathMod = create_std_math_module();
    registerModule(pVM, "std.native.math", mathMod);
    registerModule(pVM, "std.math", mathMod);

    ObjModule* strMod = create_std_str_module();
    registerModule(pVM, "std.native.str", strMod);
    registerModule(pVM, "std.str", strMod);

    ObjModule* timeMod = create_std_time_module();
    registerModule(pVM, "std.native.time", timeMod);
    registerModule(pVM, "std.time", timeMod);

    ObjModule* jsonMod = create_std_json_module();
    registerModule(pVM, "std.native.json", jsonMod);
    registerModule(pVM, "std.json", jsonMod);

    ObjModule* osMod = create_std_os_module();
    registerModule(pVM, "std.native.os", osMod);
    registerModule(pVM, "std.os", osMod);

    ObjModule* hashMod = create_std_hash_module();
    registerModule(pVM, "std.native.hash", hashMod);
    registerModule(pVM, "std.hash", hashMod);

    ObjModule* netMod = create_std_net_module();
    registerModule(pVM, "std.native.net", netMod);
    registerModule(pVM, "std.net", netMod);

    ObjModule* colMod = create_std_collections_module();
    registerModule(pVM, "std.native.collections", colMod);
    registerModule(pVM, "std.collections", colMod);

    ObjModule* reflectMod = create_std_reflect_module();
    registerModule(pVM, "std.native.reflect", reflectMod);
    registerModule(pVM, "std.reflect", reflectMod);

    ObjModule* bufMod = create_std_buffer_module();
    registerModule(pVM, "std.native.buffer", bufMod);
    registerModule(pVM, "std.buffer", bufMod);

    ObjModule* procMod = create_std_process_module();
    registerModule(pVM, "std.native.process", procMod);
    registerModule(pVM, "std.process", procMod);

    ObjModule* pathMod = create_std_path_module();
    registerModule(pVM, "std.native.path", pathMod);
    registerModule(pVM, "std.path", pathMod);

    registerModule(pVM, "std.core", create_std_core_module());
    
    ObjModule* uiMod = create_empty_module(pVM, "UI");
    registerModule(pVM, "std.native.ui", uiMod);
    registerModule(pVM, "std.ui", uiMod);

    // Legacy
    register_convert_natives(pVM);

    // Setup global 'std' object
    ObjString* stdName = copyString("std", 3);
    push(pVM, OBJ_VAL(stdName));
    ObjModule* stdMod = newModule(stdName);
    push(pVM, OBJ_VAL(stdMod));
    
    // Bind sub-modules to 'std'
    Value ioVal;
    ObjString* ioKey = copyString("std.native.io", 13);
    push(pVM, OBJ_VAL(ioKey));
    if (tableGet(&pVM->importer.modules, ioKey, &ioVal)) {
        ObjString* field = copyString("io", 2);
        push(pVM, OBJ_VAL(field));
        tableSet(&stdMod->exports, field, ioVal);
        pop(pVM);
    }
    pop(pVM);
    
    Value fsVal;
    ObjString* fsKey = copyString("std.native.fs", 13);
    push(pVM, OBJ_VAL(fsKey));
    if (tableGet(&pVM->importer.modules, fsKey, &fsVal)) {
        ObjString* field = copyString("fs", 2);
        push(pVM, OBJ_VAL(field));
        tableSet(&stdMod->exports, field, fsVal);
        pop(pVM);
    }
    pop(pVM);

    Value sysVal;
    ObjString* sysKey = copyString("std.native.sys", 14);
    push(pVM, OBJ_VAL(sysKey));
    if (tableGet(&pVM->importer.modules, sysKey, &sysVal)) {
        ObjString* field = copyString("sys", 3);
        push(pVM, OBJ_VAL(field));
        tableSet(&stdMod->exports, field, sysVal);
        pop(pVM);
    }
    pop(pVM);

    ObjModule* gcMod = create_std_gc_module();
    registerModule(pVM, "std.native.gc", gcMod);
    registerModule(pVM, "std.gc", gcMod);

    Value gcVal;
    ObjString* gcKey = copyString("std.native.gc", 13);
    push(pVM, OBJ_VAL(gcKey));
    if (tableGet(&pVM->importer.modules, gcKey, &gcVal)) {
        ObjString* field = copyString("gc", 2);
        push(pVM, OBJ_VAL(field));
        tableSet(&stdMod->exports, field, gcVal);
        pop(pVM);
    }
    pop(pVM);

    Value hashVal;
    ObjString* hashKey = copyString("std.native.hash", 15);
    push(pVM, OBJ_VAL(hashKey));
    if (tableGet(&pVM->importer.modules, hashKey, &hashVal)) {
        ObjString* field = copyString("hash", 4);
        push(pVM, OBJ_VAL(field));
        tableSet(&stdMod->exports, field, hashVal);
        pop(pVM);
    }
    pop(pVM);
    
    Value coreVal;
    ObjString* coreKey = copyString("std.core", 8);
    push(pVM, OBJ_VAL(coreKey));
    if (tableGet(&pVM->importer.modules, coreKey, &coreVal)) {
        ObjString* field = copyString("core", 4);
        push(pVM, OBJ_VAL(field));
        tableSet(&stdMod->exports, field, coreVal);
        pop(pVM);
    }
    pop(pVM);

    tableSet(&pVM->globals, stdName, OBJ_VAL(stdMod));
    pop(pVM);
    pop(pVM);

    defineNative(pVM, "clock", native_clock);
    defineNative(pVM, "len", native_len);
    defineNative(pVM, "list_push", native_push);
    defineNative(pVM, "push", native_push); 
    defineNative(pVM, "limit_pop", native_pop); 
    defineNative(pVM, "list_pop", native_pop);
    defineNative(pVM, "pop", native_pop);   
    defineNative(pVM, "substr", native_substr);
    
    defineNative(pVM, "loadConfig", nativeLoadConfig);

    register_math_globals(pVM);
    register_string_globals(pVM);
    register_io_globals(pVM);
}
