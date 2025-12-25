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
    // New Module System
    registerModule(vm, "std.native.io", create_std_io_module());
    registerModule(vm, "std.native.fs", create_std_fs_module());
    registerModule(vm, "std.native.sys", create_std_sys_module());
    
    // Math
    extern ObjModule* create_std_math_module();
    registerModule(vm, "std.native.math", create_std_math_module());

    // String
    extern ObjModule* create_std_str_module();
    registerModule(vm, "std.native.str", create_std_str_module());

    // Time
    extern ObjModule* create_std_time_module();
    registerModule(vm, "std.native.time", create_std_time_module());

    // JSON
    extern ObjModule* create_std_json_module();
    registerModule(vm, "std.native.json", create_std_json_module());

    // OS
    extern ObjModule* create_std_os_module();
    registerModule(vm, "std.native.os", create_std_os_module());

    // Hash
    extern ObjModule* create_std_hash_module();
    registerModule(vm, "std.native.hash", create_std_hash_module());

    // Net
    extern ObjModule* create_std_net_module();
    registerModule(vm, "std.native.net", create_std_net_module());

    // Collections
    extern ObjModule* create_std_collections_module();
    registerModule(vm, "std.native.collections", create_std_collections_module());

    // Reflect
    extern ObjModule* create_std_reflect_module();
    registerModule(vm, "std.native.reflect", create_std_reflect_module());

    // std.core creator is usually registered as std.core
    extern ObjModule* create_std_core_module();
    registerModule(vm, "std.core", create_std_core_module());

    // Legacy (Global Scope) - keep for now or refactor later
    // register_math_natives(vm); // Moved to std.native.math
    // register_string_natives(vm); // Moved to std.native.str
    register_convert_natives(vm);
    // register_system_natives(vm); // Replaced by std.sys
}
