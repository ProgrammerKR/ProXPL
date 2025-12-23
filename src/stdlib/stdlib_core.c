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
    registerModule(vm, "std.io", create_std_io_module());
    registerModule(vm, "std.fs", create_std_fs_module());
    registerModule(vm, "std.sys", create_std_sys_module());
    
    // std.core creator is usually registered as std.core
    extern ObjModule* create_std_core_module();
    registerModule(vm, "std.core", create_std_core_module());

    // Legacy (Global Scope) - keep for now or refactor later
    register_math_natives(vm);
    register_string_natives(vm);
    register_convert_natives(vm);
    // register_system_natives(vm); // Replaced by std.sys
}
