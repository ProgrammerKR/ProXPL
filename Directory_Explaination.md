# ProXPL – Directory Overview

# ProXPL – Full Project Architecture  
A complete breakdown of the directory structure for the ProXPL programming language ecosystem.  
This document explains what every directory and file is responsible for so that any code editor or AI assistant can generate, extend, or maintain the system correctly.

---

## 📁 1. core/ – The Compiler Core

### `core/lexer/`
- **lexer.py** → Converts raw ProXPL source code into tokens.

### `core/parser/`
- **parser.py** → Builds the syntax tree from tokens.

### `core/ast/`
- **ast_nodes.py** → Definitions for all AST (Abstract Syntax Tree) node types.

### `core/semantic/`
- **type_checker.py** → Type validation, symbol table, constants, scopes.

### `core/ir/`
- **ir_format.py** → Defines the IR (Intermediate Representation) used by the compiler.

### `core/compiler/`
- **compiler_pipeline.py** → Full compilation pipeline from source → AST → IR → Bytecode.

### `core/vm/`
- **vm.py** → Virtual machine implementation to execute bytecode.

### `core/optimizer/`
- **optimizer.py** → Optimization passes (dead code, constant folding, inline functions).

### `core/main/`
- **main.py** → Compiler entry point.

---

## 📁 2. internal/ – Private Internal Systems (Not for Developers)

### `internal/rnd/`
- **experimental_notes.md** → Research & experimental features.

### `internal/security/`
- **encryption.py** → Secure code packaging.
- **integrity_check.py** → Hash/signature verification.

### `internal/licensing/`
- **license_system.py** → License validation and internal product security.

### `internal/telemetry/`
- **telemetry_client.py** → Anonymous usage/diagnostic tracking.

### `internal/codegen/`
- **cpu_codegen.py** → Converts IR into platform-specific instructions.

### `internal/sandbox/`
- **sandbox_test.py** → Security sandbox for unsafe code.

---

## 📁 3. stdlib/ – Standard Library (Written in ProXPL)

### `stdlib/core/`
- Basic types: strings, numbers.

### `stdlib/utils/`
- Helper utilities.

### `stdlib/io/`
- File system access.

### `stdlib/math/`
- Math functions.

### `stdlib/net/`
- Networking utilities.

---

## 📁 4. runtime/ – Execution Engine Components

### `runtime/gc/`
- **gc.py** → Garbage collector.

### `runtime/jit/`
- **jit_engine.py** → JIT compilation engine.

### `runtime/memory/`
- **memory_manager.py** → Heap, stack, memory allocator.

---

## 📁 5. cli/ – Command-Line Tools

### `cli/commands/`
- **build.py** → Build commands.
- **run.py** → Execution commands.

### `cli/build/`
- **builder.py** → Build pipeline helpers.

### `cli/watcher/`
- **watcher.py** → Auto-reload during development.

### `cli/scripts/`
- **clean.py** → Cleanup tool.

---

## 📁 6. tools/ – Developer Tools

### `tools/proxpm/`
- **proxpm.py** → Package manager.

### `tools/proxfmt/`
- **formatter.py** → Code formatter.

### `tools/proxlint/`
- **linter.py** → Code quality analyzer.

---

## 📁 7. sdk/ – Developer SDK

### `sdk/templates/`
- Templates for creating ProXPL apps.

### `sdk/bindings/`
- **ffi.c / ffi.h** → C interoperability.

### `sdk/compiler-api/`
- **compiler_api.py** → API to embed/use compiler.

---

## 📁 8. apps/ – Example Applications
- demo and system tests.

---

## 📁 9. qa/ – Tests & Quality Assurance

### `qa/compiler_tests/`
- Lexer/parser/compiler tests.

### `qa/stdlib_tests/`
- Standard library tests.

### `qa/performance/`
- Performance benchmarking.

### `qa/fuzzing/`
- Lightning-fast random input fuzzer.

---

## 📁 10. builds/ – Output Builds

### `builds/release/`
- Release builds.

### `builds/debug/`
- Debug builds.

---

## 📁 11. docs/ – Official Documentation

- Language syntax  
- Grammar  
- Compiler architecture  
- IR specification  
- Security model  
- Internal staff notes  

---

## 📁 12. configs/ – Config Files

- Compiler config  
- VM config  
- SDK config  

---

## 📁 13. scripts/ – Auto Scripts
- build.sh  
- release.sh  

---

# ✔ How AI Editors Should Use This Structure
AI code editors (Antigravity, Cursor, Windsurf, Copilot Workspace, etc.) should:

1. Follow this directory structure strictly.  
2. Keep compiler code inside `core/`.  
3. Keep VM, runtime, and JIT logic in `runtime/`.  
4. Generate new stdlib modules inside `stdlib/`.  
5. Use `sdk/` for integrations and templates.  
6. Never modify `internal/` unless requested: it is system-level only.

This README ensures consistent, scalable, and correct generation of the entire ProXPL language ecosystem.
