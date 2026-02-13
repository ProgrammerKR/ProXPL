@echo off
clang -o bytecode_tests.exe -Iinclude -Isrc -D_CRT_SECURE_NO_WARNINGS -DPROX_STATIC ^
 tests/bytecode_tests.c ^
 src/vm/bytecode.c ^
 src/vm/disasm.c ^
 src/vm/vm_dispatch.c ^
 src/runtime/value.c ^
 src/runtime/object.c ^
 src/runtime/memory.c ^
 src/runtime/table.c ^
 src/runtime/chunk.c ^
 src/compiler/parser/ast.c ^
 src/runtime/vm_helpers.c
if %errorlevel% equ 0 echo Build complete.
