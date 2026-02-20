@echo off
clang -o proxpl.exe -Iinclude -Isrc -D_CRT_SECURE_NO_WARNINGS -DPROX_STATIC ^
 src/main.c ^
 src/compiler/lexer/scanner.c ^
 src/compiler/parser/ast.c ^
 src/compiler/parser/parser.c ^
 src/compiler/bytecode_gen.c ^
 src/compiler/importer.c ^
 src/compiler/ir.c ^
 src/compiler/ir_gen.c ^
 src/compiler/ir_opt.c ^
 src/compiler/type_checker.c ^
 src/runtime/chunk.c ^
 src/runtime/compiler.c ^
 src/runtime/debug.c ^
 src/runtime/ffi_bridge.c ^
 src/runtime/gc.c ^
 src/runtime/memory.c ^
 src/runtime/object.c ^
 src/runtime/scheduler.c ^
 src/runtime/table.c ^
 src/runtime/value.c ^
 src/runtime/vm.c ^
 src/runtime/vm_helpers.c ^
 src/utils/error_report.c ^
 src/stdlib/collections_native.c ^
 src/stdlib/convert_native.c ^
 src/stdlib/core_native.c ^
 src/stdlib/fs_native.c ^
 src/stdlib/gc_native.c ^
 src/stdlib/hash_native.c ^
 src/stdlib/io_native.c ^
 src/stdlib/json_native.c ^
 src/stdlib/math_native.c ^
 src/stdlib/net_native.c ^
 src/stdlib/os_native.c ^
 src/stdlib/reflect_native.c ^
 src/stdlib/stdlib_core.c ^
 src/stdlib/string_native.c ^
 src/stdlib/sys_native.c ^
 src/stdlib/system_native.c ^
 src/stdlib/time_native.c ^
 src/utils/md5.c ^
 src/utils/sha256.c ^
 src/prm/manifest.c ^
 src/prm/builder.c ^
 src/prm/commands/cmd_core.c
if %errorlevel% equ 0 (
    echo Build complete.
) else (
    echo Build FAILED. Check errors above.
    exit /b 1
)
