# ProXPL C Implementation

This directory contains the C-based implementation of the ProXPL programming language.

## Directory Structure

- `lexer/` - Lexical analysis (tokenization)
- `parser/` - Syntax analysis (AST generation)
- `runtime/` - Virtual machine and runtime system
- `stdlib/` - Standard library native functions
- `utils/` - Utility functions and helpers
- `main.c` - Entry point
- `Makefile` - Build system

## Build Instructions

```bash
cd csrc
make
```

## Status

🚧 **Under Development** 

This C implementation is currently under active development. The Python reference implementation is still the primary version.

## Translation Progress

### Core Compiler
- [x] Lexer (scanner.c)
- [x] Parser (parser.c)
- [x] AST (ast.c)
- [x] Type Checker (type_checker.c - partial)
- [x] Bytecode Compiler (compiler.c)

### Runtime & VM
- [x] Virtual Machine (vm.c)
- [x] Bytecode Chunks (chunk.c)
- [x] Value System (value.c)
- [x] Object System (object.c)
- [x] Memory Management (memory.c - partial)
- [x] Debug Utilities (debug.c)

### Standard Library
- [x] I/O Module (io_native.c) - 5 functions
- [x] Math Module (math_native.c) - 15 functions
- [x] String Module (string_native.c) - 9 functions
- [x] Convert Module (convert_native.c) - 8 functions
- [x] System Module (system_native.c) - 7 functions
- [x] StdLib Core (stdlib_core.c) - registry
- [ ] Collections Module (lists, dicts, sets)
- [ ] DateTime Module
- [ ] Runtime Utilities Module

### Advanced Features
- [ ] Garbage Collector (gc.c)
- [ ] Module Importer (importer.c)
- [ ] IR System (ir.c)
- [ ] Optimizer (optimizer.c)
- [ ] JIT Compiler (jit.c)

### CLI Tools
- [x] Main Entry Point (main.c - REPL + file execution)