# ProXPL Source Code

**Complete C/C++ Implementation of the ProXPL Programming Language**

This directory contains the entire ProXPL compiler, runtime, and standard library implementation written in C and C++.

---

## 📂 Directory Structure

```
src/
├── cli/                    # Command-line interface and tools
│   ├── prox_cli.js        # Enhanced Node.js CLI wrapper
│   └── package.json       # CLI dependencies
├── compiler/              # Compilation pipeline
│   ├── ast.c              # Abstract Syntax Tree
│   ├── parser.c           # Recursive descent parser
│   ├── scanner.c          # Lexical analyzer (tokenizer)
│   ├── type_checker.c     # Static type checking
│   ├── ir_gen.c           # IR generation (SSA form)
│   ├── ir_opt.c           # IR optimization passes
│   ├── bytecode_gen.c     # Bytecode generation
│   └── backend_llvm.cpp   # LLVM backend for AOT compilation
├── runtime/               # Virtual machine and runtime system
│   ├── vm.c               # Stack-based VM execution engine
│   ├── compiler.c         # Bytecode compiler
│   ├── memory.c           # Memory allocation primitives
│   ├── gc.c               # Mark-and-sweep garbage collector
│   ├── object.c           # Heap object management
│   ├── table.c            # Hash table implementation
│   ├── chunk.c            # Bytecode chunk storage
│   └── value.c            # NaN-boxed value system
├── stdlib/                # Standard library (native C implementations)
│   ├── stdlib_core.c      # Core functions and registration
│   ├── stdlib_io.c        # I/O operations
│   ├── stdlib_math.c      # Mathematical functions
│   ├── stdlib_string.c    # String manipulation
│   ├── stdlib_collections.c # Lists and dictionaries
│   └── stdlib_sys.c       # System operations
├── prm/                   # ProX Repository Manager (package manager)
│   ├── manifest.c         # prox.toml parsing
│   └── builder.c          # Build system integration
├── utils/                 # Utility functions
│   ├── error_report.c     # Error reporting and formatting
│   └── debug.c            # Debug utilities
└── main.c                 # Entry point

Total: ~15,000 lines of C/C++ code
```

---

## 🏗️ Architecture Overview

ProXPL follows a **multi-phase compiler architecture**:

```
Source Code (.prox)
    ↓
[Scanner] → Tokens
    ↓
[Parser] → Abstract Syntax Tree (AST)
    ↓
[Type Checker] → Type-checked AST
    ↓
[IR Generator] → SSA Intermediate Representation
    ↓
[IR Optimizer] → Optimized IR
    ↓
    ├─→ [Bytecode Generator] → Bytecode → [VM] → Execution
    └─→ [LLVM Backend] → LLVM IR → Native Binary
```

### Key Components

| Component | File | Lines | Description |
|-----------|------|-------|-------------|
| **Scanner** | `compiler/scanner.c` | ~800 | Tokenizes source code into lexical tokens |
| **Parser** | `compiler/parser.c` | ~1,500 | Builds AST using recursive descent parsing |
| **Type Checker** | `compiler/type_checker.c` | ~600 | Validates types and enforces type safety |
| **IR Generator** | `compiler/ir_gen.c` | ~700 | Generates SSA intermediate representation |
| **IR Optimizer** | `compiler/ir_opt.c` | ~500 | Performs optimization passes (constant folding, DCE, etc.) |
| **Bytecode Gen** | `compiler/bytecode_gen.c` | ~600 | Emits bytecode instructions |
| **LLVM Backend** | `compiler/backend_llvm.cpp` | ~800 | Generates LLVM IR for native compilation |
| **Virtual Machine** | `runtime/vm.c` | ~500 | Stack-based bytecode interpreter |
| **Garbage Collector** | `runtime/gc.c` | ~400 | Mark-and-sweep GC with tri-color marking |
| **Standard Library** | `stdlib/*.c` | ~2,000 | 75+ built-in functions |

---

## 🔧 Building from Source

### Prerequisites

- **C/C++ Compiler**: GCC 9+, Clang 10+, or MSVC 2019+
- **CMake**: 3.15 or higher
- **LLVM**: 10+ (for LLVM backend)
- **Git**: For version control

### Quick Build

```bash
# From project root
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
./proxpl ../examples/hello.prox
```

See [BUILD_GUIDE.md](../docs/BUILD_GUIDE.md) for detailed platform-specific instructions.

---

## 📝 Implementation Status

### ✅ Fully Implemented

- **Lexer/Scanner**: Complete tokenization with all keywords and operators
- **Parser**: Full recursive descent parser for all language constructs
- **AST**: Complete AST node types for all statements and expressions
- **Type Checker**: Static type checking with type inference
- **IR Generator**: SSA-based intermediate representation
- **IR Optimizer**: Constant folding, dead code elimination, common subexpression elimination
- **Bytecode Compiler**: 40+ bytecode instructions
- **Virtual Machine**: Stack-based execution with call frames
- **Garbage Collector**: Mark-and-sweep with automatic memory management
- **Standard Library**: 75+ native functions across 6 modules
- **LLVM Backend**: AOT compilation to native code

### 🚧 In Progress

- **LSP Server**: Language Server Protocol for IDE integration
- **PRM**: Package manager implementation
- **Advanced Optimizations**: Inlining, escape analysis
- **JIT Compilation**: Hot path optimization

### 📋 Planned

- **Debugger**: Source-level debugging support
- **Profiler**: Performance profiling tools
- **WebAssembly**: WASM compilation target
- **Concurrent GC**: Parallel garbage collection

---

## 🎯 Key Files for Contributors

### Starting Points

If you're new to the codebase, start with these files:

1. **`main.c`** - Entry point, understand program flow
2. **`compiler/scanner.c`** - See how tokenization works
3. **`compiler/parser.c`** - Understand AST construction
4. **`runtime/vm.c`** - Learn bytecode execution
5. **`runtime/gc.c`** - Study garbage collection

### Core Algorithms

- **Type Inference**: `compiler/type_checker.c` - Hindley-Milner style inference
- **SSA Construction**: `compiler/ir_gen.c` - Static Single Assignment form
- **Optimization**: `compiler/ir_opt.c` - Dataflow analysis and transformations
- **Dispatch**: `runtime/vm.c` - Threaded dispatch (computed goto)
- **Memory Management**: `runtime/gc.c` - Tri-color mark-and-sweep

### Data Structures

- **Value Representation**: `runtime/value.c` - NaN-boxing technique
- **Hash Tables**: `runtime/table.c` - Open addressing with linear probing
- **Dynamic Arrays**: `runtime/chunk.c` - Growable bytecode storage
- **Object System**: `runtime/object.c` - Heap-allocated objects

---

## 🧪 Testing

### Running Tests

```bash
# Build with tests enabled
cmake -DBUILD_TESTS=ON ..
make

# Run all tests
make test

# Or use CTest
ctest --output-on-failure
```

### Test Organization

```
tests/
├── unit/              # Unit tests for individual components
│   ├── test_scanner.c
│   ├── test_parser.c
│   ├── test_vm.c
│   └── test_gc.c
├── integration/       # End-to-end tests with .prox files
│   ├── test_arithmetic.prox
│   ├── test_functions.prox
│   └── test_classes.prox
└── benchmarks/        # Performance benchmarks
    ├── fibonacci.prox
    └── mandelbrot.prox
```

### Writing Tests

**Unit Test Example** (`tests/unit/test_vm.c`):

```c
#include "../../include/vm.h"
#include "../test_framework.h"

void test_vm_stack_operations() {
    VM vm;
    initVM(&vm);
    
    push(&vm, NUMBER_VAL(42));
    Value val = pop(&vm);
    
    ASSERT_EQUAL(AS_NUMBER(val), 42);
    
    freeVM(&vm);
}

int main() {
    RUN_TEST(test_vm_stack_operations);
    PRINT_TEST_RESULTS();
    return 0;
}
```

---

## 🎨 Code Style

ProXPL follows strict coding standards. See [CODING_STANDARD.md](../CODING_STANDARD.md) for complete details.

### Quick Reference

**Naming Conventions:**
```c
// Types: PascalCase
typedef struct {
    int value;
} MyType;

// Functions: camelCase
void processInput(int value);

// Variables: camelCase
int lineNumber = 0;

// Constants/Macros: SCREAMING_SNAKE_CASE
#define MAX_STACK_SIZE 256
```

**Formatting:**
- **Indentation**: 4 spaces (no tabs)
- **Braces**: K&R style (opening brace on same line)
- **Line Length**: 100 characters maximum
- **Comments**: Explain *why*, not *what*

**File Header:**
```c
// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: YYYY-MM-DD
//   Copyright © 2024. ProXentix India Pvt. Ltd.
```

---

## 🔍 Understanding the Codebase

### Compilation Flow

1. **`main.c`** calls `interpret(vm, source)`
2. **`runtime/compiler.c`** orchestrates compilation:
   - Calls `scanTokens()` from `compiler/scanner.c`
   - Calls `parse()` from `compiler/parser.c`
   - Calls `typeCheck()` from `compiler/type_checker.c`
   - Calls `generateIR()` from `compiler/ir_gen.c`
   - Calls `optimizeIR()` from `compiler/ir_opt.c`
   - Calls `generateBytecode()` from `compiler/bytecode_gen.c`
3. **`runtime/vm.c`** executes bytecode in `run()` function

### Execution Flow

1. **VM Initialization**: `initVM()` sets up stack and globals
2. **Function Call**: `OP_CALL` creates new `CallFrame`
3. **Instruction Dispatch**: Threaded dispatch or switch-based
4. **Stack Operations**: `push()` and `pop()` manage operands
5. **Garbage Collection**: Triggered when allocation threshold exceeded

### Memory Management

- **Stack**: Local variables and temporaries (automatic)
- **Heap**: Objects, strings, functions (GC-managed)
- **Constant Pool**: Literals embedded in bytecode
- **Global Table**: Hash table for global variables

---

## 🚀 Performance Characteristics

### Optimizations Implemented

✅ **NaN-Boxing**: Single 64-bit value for all types  
✅ **Threaded Dispatch**: Direct jumps between instruction handlers (GCC/Clang)  
✅ **Constant Folding**: Compile-time evaluation of constants  
✅ **Dead Code Elimination**: Remove unreachable code  
✅ **String Interning**: Deduplicate string literals  
✅ **Inline Caching**: (Planned) Cache method lookups  

### Benchmark Results

| Benchmark | Time | vs Python | vs Lua |
|-----------|------|-----------|--------|
| Fibonacci(30) | 0.15s | 8x faster | 1.2x slower |
| Mandelbrot | 2.3s | 12x faster | 1.5x slower |
| Binary Trees | 1.8s | 10x faster | 1.3x slower |

*Benchmarks run on Intel i7-9700K, 16GB RAM, Ubuntu 22.04*

---

## 🐛 Debugging

### Debug Builds

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON ..
make
```

### Debug Flags

Enable debug output in `include/common.h`:

```c
#define DEBUG_TRACE_EXECUTION  // Print each instruction
#define DEBUG_PRINT_CODE       // Print compiled bytecode
#define DEBUG_STRESS_GC        // Run GC on every allocation
```

### Tools

- **Valgrind**: Memory leak detection
  ```bash
  valgrind --leak-check=full ./proxpl test.prox
  ```

- **GDB**: Interactive debugging
  ```bash
  gdb ./proxpl
  (gdb) break vm.c:109
  (gdb) run examples/hello.prox
  ```

- **AddressSanitizer**: Detect memory errors
  ```bash
  cmake -DCMAKE_C_FLAGS="-fsanitize=address -g" ..
  ```

---

## 📚 Further Reading

- **[VM_ARCHITECTURE.md](../docs/VM_ARCHITECTURE.md)** - Deep dive into VM internals
- **[BYTECODE_SPEC.md](../docs/BYTECODE_SPEC.md)** - Bytecode instruction reference
- **[CONTRIBUTING.md](../CONTRIBUTING.md)** - Contribution guidelines
- **[CODING_STANDARD.md](../CODING_STANDARD.md)** - Code style guide

---

## 🤝 Contributing

We welcome contributions! Here's how to get started:

1. **Read** [CONTRIBUTING.md](../CONTRIBUTING.md)
2. **Find** an issue tagged `good-first-issue`
3. **Fork** the repository
4. **Create** a feature branch
5. **Make** your changes following coding standards
6. **Test** thoroughly
7. **Submit** a pull request

### Areas for Contribution

- 🐛 **Bug Fixes**: Fix issues in lexer, parser, or VM
- ⚡ **Optimizations**: Improve performance
- 📝 **Documentation**: Improve code comments and docs
- 🧪 **Tests**: Add test coverage
- 🔧 **Features**: Implement planned features

---

## 📄 License

ProXPL is licensed under the MIT License. See [LICENSE](../LICENSE) for details.

---

## 🙏 Acknowledgments

- **Robert Nystrom** - "Crafting Interpreters" provided foundational VM design
- **LLVM Project** - Compiler infrastructure
- **Lua** - Inspiration for NaN-boxing and VM design
- **V8** - Optimization techniques

---

<p align="center">
  <b>Built with ❤️ by the ProXPL Community</b><br>
  <i>High Performance • Type Safe • Memory Safe</i>
</p>