/*
 * ProXPL Standard Library - Main Registry
 * Registers all standard library modules with the VM
 */

#include "../include/common.h"
#include "../include/vm.h"

// Forward declarations for registration functions
void register_io_natives(VM* vm);
void register_math_natives(VM* vm);
void register_string_natives(VM* vm);
void register_convert_natives(VM* vm);
void register_system_natives(VM* vm);

/*
 * Register all standard library modules
 * Called during VM initialization
 */
void registerStdLib(VM* vm) {
    register_io_natives(vm);
    register_math_natives(vm);
    register_string_natives(vm);
    register_convert_natives(vm);
    register_system_natives(vm);
}
