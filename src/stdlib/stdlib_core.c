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

// Forward declarations for module creators
ObjModule* create_std_io_module();
ObjModule* create_std_fs_module();
ObjModule* create_std_sys_module();
ObjModule* create_std_core_module();

// Legacy
void register_math_natives(VM* vm);
void register_string_natives(VM* vm);
void register_convert_natives(VM* vm);
void register_system_natives(VM* vm);

static void registerModule(VM* vm, const char* name, ObjModule* module) {
    ObjString* nameObj = copyString(name, (int)strlen(name));
    push(vm, OBJ_VAL(nameObj));
    push(vm, OBJ_VAL(module));
    tableSet(&vm->importer.modules, nameObj, peek(vm, 0));
    pop(vm);
    pop(vm);
}

/*
 * Register all standard library modules
 * Called during VM initialization
 */
void registerStdLib(VM* vm) {
    // New Module System + Aliases
    ObjModule* ioMod = create_std_io_module();
    registerModule(vm, "std.native.io", ioMod);
    registerModule(vm, "std.io", ioMod);

    ObjModule* fsMod = create_std_fs_module();
    registerModule(vm, "std.native.fs", fsMod);
    registerModule(vm, "std.fs", fsMod);

    ObjModule* sysMod = create_std_sys_module();
    registerModule(vm, "std.native.sys", sysMod);
    registerModule(vm, "std.sys", sysMod);
    
    // Math
    extern ObjModule* create_std_math_module();
    ObjModule* mathMod = create_std_math_module();
    registerModule(vm, "std.native.math", mathMod);
    registerModule(vm, "std.math", mathMod);

    // String
    extern ObjModule* create_std_str_module();
    ObjModule* strMod = create_std_str_module();
    registerModule(vm, "std.native.str", strMod);
    registerModule(vm, "std.str", strMod);

    // Time
    extern ObjModule* create_std_time_module();
    ObjModule* timeMod = create_std_time_module();
    registerModule(vm, "std.native.time", timeMod);
    registerModule(vm, "std.time", timeMod);

    // JSON
    extern ObjModule* create_std_json_module();
    ObjModule* jsonMod = create_std_json_module();
    registerModule(vm, "std.native.json", jsonMod);
    registerModule(vm, "std.json", jsonMod);

    // OS
    extern ObjModule* create_std_os_module();
    ObjModule* osMod = create_std_os_module();
    registerModule(vm, "std.native.os", osMod);
    registerModule(vm, "std.os", osMod);

    // Hash
    extern ObjModule* create_std_hash_module();
    ObjModule* hashMod = create_std_hash_module();
    registerModule(vm, "std.native.hash", hashMod);
    registerModule(vm, "std.hash", hashMod);

    // Net
    extern ObjModule* create_std_net_module();
    ObjModule* netMod = create_std_net_module();
    registerModule(vm, "std.native.net", netMod);
    registerModule(vm, "std.net", netMod);

    // Collections
    extern ObjModule* create_std_collections_module();
    ObjModule* colMod = create_std_collections_module();
    registerModule(vm, "std.native.collections", colMod);
    registerModule(vm, "std.collections", colMod);

    // Reflect
    extern ObjModule* create_std_reflect_module();
    ObjModule* reflectMod = create_std_reflect_module();
    registerModule(vm, "std.native.reflect", reflectMod);
    registerModule(vm, "std.reflect", reflectMod);

    // std.core creator is usually registered as std.core
    extern ObjModule* create_std_core_module();
    registerModule(vm, "std.core", create_std_core_module());

    // Legacy
    register_convert_natives(vm);

    // Setup global 'std' object
    ObjString* stdName = copyString("std", 3);
    push(vm, OBJ_VAL(stdName));
    ObjModule* stdMod = newModule(stdName);
    push(vm, OBJ_VAL(stdMod));
    
    // Bind sub-modules to 'std'
    // std.io
    Value ioVal;
    ObjString* ioKey = copyString("std.native.io", 13);
    push(vm, OBJ_VAL(ioKey));
    if (tableGet(&vm->importer.modules, ioKey, &ioVal)) {
        ObjString* field = copyString("io", 2);
        push(vm, OBJ_VAL(field));
        tableSet(&stdMod->exports, field, ioVal);
        pop(vm); // field
    }
    pop(vm); // ioKey
    
    // std.fs
    Value fsVal;
    ObjString* fsKey = copyString("std.native.fs", 13);
    push(vm, OBJ_VAL(fsKey));
    if (tableGet(&vm->importer.modules, fsKey, &fsVal)) {
        ObjString* field = copyString("fs", 2);
        push(vm, OBJ_VAL(field));
        tableSet(&stdMod->exports, field, fsVal);
        pop(vm);
    }
    pop(vm);

    // std.sys
    Value sysVal;
    ObjString* sysKey = copyString("std.native.sys", 14);
    push(vm, OBJ_VAL(sysKey));
    if (tableGet(&vm->importer.modules, sysKey, &sysVal)) {
        ObjString* field = copyString("sys", 3);
        push(vm, OBJ_VAL(field));
        tableSet(&stdMod->exports, field, sysVal);
        pop(vm);
    }
    // GC
    extern ObjModule* create_std_gc_module();
    ObjModule* gcMod = create_std_gc_module();
    registerModule(vm, "std.native.gc", gcMod);
    registerModule(vm, "std.gc", gcMod);

    // std.gc (bind to main std object)
    Value gcVal;
    ObjString* gcKey = copyString("std.native.gc", 13);
    push(vm, OBJ_VAL(gcKey));
    if (tableGet(&vm->importer.modules, gcKey, &gcVal)) {
        ObjString* field = copyString("gc", 2);
        push(vm, OBJ_VAL(field));
        tableSet(&stdMod->exports, field, gcVal);
        pop(vm); // field
    }
    pop(vm); // gcKey

    // Register 'std' as global
    tableSet(&vm->globals, stdName, OBJ_VAL(stdMod));
    pop(vm); // stdMod
    pop(vm); // stdName
}
