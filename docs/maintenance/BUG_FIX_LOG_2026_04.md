# ProXPL VM Bug Fix Log - April 2026

This document details the comprehensive audit and subsequent fixes applied to the ProXPL Virtual Machine (`src/runtime/vm.c`) to improve memory safety, runtime stability, and semantic correctness.

## Summary of Changes

A total of 15 bugs were addressed, ranging from critical memory corruption issues to logic errors in opcode implementations.

### 🔴 Critical Memory Safety Fixes

1.  **Stack Overflow Protection**: Added explicit `exit(1)` in the `push()` function to halt execution upon stack overflow, preventing out-of-bounds memory writes.
2.  **Stack Underflow Guard**: Implemented bounds checking in `pop()` to prevent the stack pointer from decrementing below the stack base.
3.  **Runtime Error Frame Guard**: Added a check in `runtimeError()` to ensure `frameCount > 0` before accessing call frames, preventing crashes during early initialization or edge-case errors.
4.  **Tensor Dimension Overflow**: Increased the internal `dims` array size in `OP_MAKE_TENSOR` from 16 to 256 to accommodate the full range of `uint8_t` dimension counts.
5.  **Sign Extension UB**: Fixed undefined behavior in 32-bit integer reconstruction from bytecode by casting `uint8_t` reads to `uint32_t` before bitwise shifting.
6.  **Use-After-Free in freeVM**: Ensured `initString` is set to `NULL` before freeing objects to prevent the GC or free-loop from accessing a dangling pointer.
7.  **resolveContextualMethod Tombstone Detection**: Fixed a regression where tombstone entries in the context method table could cause crashes by restoring the `IS_BOOL` check.

### 🟠 Logic and Semantic Improvements

7.  **Numeric Type Guards**: Added `IS_NUMBER` checks to `OP_GREATER`, `OP_LESS`, `OP_MODULO`, and all bitwise opcodes (`AND`, `OR`, `XOR`, `LSHIFT`, `RSHIFT`) to ensure type safety before numeric extraction.
8.  **Global Variable Semantics**: Replaced the "insert-then-delete" hack in `OP_SET_GLOBAL` with a proper existence check via `tableGet`, ensuring standard Lox-style global variable behavior.
9.  **Tombstone Handling**: Fixed the tombstone detection logic in `resolveContextualMethod()` to correctly identify empty slots by checking for `NULL` keys.
10. **Module Loading Fix**: Corrected `OP_USE` to actually push the loaded module onto the stack, allowing user scripts to interact with imported modules.
11. **Inheritance Stack Management**: Fixed `OP_INHERIT` to only pop the subclass, keeping the superclass on the stack as required for `super` method lookups.
12. **Division/Modulo Safety**: Implemented runtime error reporting for division and modulo by zero in scalar operations, matching existing tensor safety.
13. **Bitwise Shift Safety**:
    - Added checks to `OP_LEFT_SHIFT` and `OP_RIGHT_SHIFT` to ensure shift amounts are between 0 and 31.
    - Implemented unsigned casts in `OP_LEFT_SHIFT` to prevent undefined behavior when shifting negative signed integers.
14. **Tensor Build Safety**:
    - Implemented integer overflow protection for `totalSize` calculation in `OP_MAKE_TENSOR`.
    - Added stack underflow bounds checking before adjusting the stack pointer during tensor initialization.
    - Removed redundant dead code guards.

### 🟡 Structural and Minor Fixes

15. **Dead Code Elimination**: Removed an unreachable `DISPATCH()` call at the end of the `OP_CALL` implementation.
16. **interpretChunk Safety**: 
    - Enhanced documentation and ensured `initChunk` calls in `interpretChunk()` safely detach transient function objects from caller-owned memory.
    - Added a GC guard (`pvm->nextGC = SIZE_MAX`) during `interpretChunk` execution to prevent the collector from freeing transient chunk data during the run.
17. **NaN Equality**: Updated `OP_EQUAL` to respect IEEE 754 NaN semantics (`NaN != NaN`) for NaN-boxed numeric values.
18. **Foreign Function Safety**: Added `IS_STRING` guards to `OP_MAKE_FOREIGN` to prevent crashes when non-string values are passed as library or symbol names.
19. **List Build Safety**: Added a stack underflow check to `OP_BUILD_LIST` to prevent memory corruption from malformed bytecode.

## System-Wide Fixes (CLI, API, & Build)

### 🔴 CLI & Lifecycle Security (`src/main.c`)
20. **Token Array Bounds**: Implemented strict look-ahead bounds checking for token arrays in both REPL (256 tokens) and file execution (4096 tokens) to prevent buffer overflows.
21. **Buffer Security**: Replaced unsafe `sprintf` calls with `snprintf` when generating UI transpilation output directories.
22. **Error Path Cleanup**: Corrected the order of resource deallocation in `runFile` to ensure `TypeChecker`, `StmtList`, and source buffers are freed even on failure paths.
23. **CLI Extension Guard**: Added length verification to `argv` indexing to prevent out-of-bounds reads on short filenames.
24. **ftell Sign Error**: Added checks for negative return values from `ftell()` to prevent massive erroneous memory allocations on file I/O failure.

### 🟠 Public API & Integration (`src/proxpl_api.c`)
25. **Lifecycle Guards**: Added NULL pointer verification to `proxpl_vm_init`, `proxpl_vm_free`, and `proxpl_interpret_file` for integration safety.
26. **Include Standardization**: Reordered headers to ensure system libraries are prioritized over local project headers, preventing macro conflicts.

### 🟡 Build System & Workspace (`Makefile`, `.gitignore`)
27. **Strict Compilation**: Enabled `-Wpedantic` and `-Wno-unused-parameter` to catch standards violations and silence intentional suppression warnings.
28. **Missing Sources**: Added `src/proxpl_api.c` to the `SOURCES` list to ensure the public API is correctly linked into the binary.
29. **Workspace Hygiene**: Updated `.gitignore` to track UI transpiler output (`dist_*/`) and PRM caches (`.prm-cache/`), and verified correct file naming.

---
**Version Update**: These fixes are included in ProXPL version **1.3.2**.
**Date**: April 26, 2026
**Lead Engineer**: Antigravity (AI Assistant)
