<div align="center"><h1> ProXPL - ProX Programming Language  </h1></div>

<p align="center">
  <b>A Modern, High-Performance Programming Language</b>
</p>

<div align="center">

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![ProXPL CI](https://github.com/ProgrammerKR/ProXPL/actions/workflows/build.yml/badge.svg)](https://github.com/ProgrammerKR/ProXPL/actions/workflows/build.yml)
[![Version](https://img.shields.io/badge/version-1.3.3-green.svg)](https://github.com/ProgrammerKR/ProXPL/releases)
[![Platform](https://img.shields.io/badge/platform-win%20%7C%20linux%20%7C%20macos-lightgrey.svg)]()


**Clean Syntax • Static Typing • Stack-Based VM • C-Level Performance**

[Quick Start](#-quick-start) •
[Installation](#-installation) •
[Documentation](#-documentation) •
[Architecture](#-architecture-overview) •
[Contributing](#-contributing)

</div>

---

## 📖 Introduction

**ProXPL** (ProX Programming Language) is a modern, statically-typed **multi-paradigm systems programming language** that seamlessly integrates **Object-Oriented**, **Intent-Oriented**, and **Context-Oriented** features for **clarity, performance, and reliability**. Born from a vision to combine Python's readability with C's execution speed, ProXPL features a professional **compiler architecture**, a custom **stack-based bytecode VM**, a robust **static type system**, and an integrated **package manager (PRM)**.

ProXPL is implemented entirely in **C/C++** with zero runtime dependencies, making it ideal for **high-performance systems**, **embedded applications**, **game development**, and **backend services**. It serves as an excellent reference for learning **compiler design** and **interpreter implementation**.

### Why ProXPL?

- **🎯 Familiar Syntax**: Clean, expressive syntax inspired by JavaScript and Python
- **⚡ True Performance**: Bytecode compilation to a stack-based VM with LLVM backend for AOT compilation
- **🛡️ Type Safety**: Static typing with intelligent type inference prevents entire classes of runtime errors
- **🔧 Batteries Included**: 75+ built-in standard library functions covering I/O, math, strings, collections, and system operations
- **📦 Integrated Tooling**: Built-in package manager (PRM), CLI tools, and LSP support
- **🏗️ Professional Architecture**: Clean separation between lexer, parser, type checker, compiler, and VM

---

## ✨ Key Features

| Feature | Description |
|---------|-------------|
| 🔤 **Modern Syntax** | JavaScript-like syntax with curly braces, familiar control flow, and clean function definitions |
| 🎨 **ProXPL Icons** | **1100+ File Icons** support via the official extension (Material Icon Theme integration) |
| ⚡ **Fast Execution** | Custom stack-based VM executing optimized bytecode with LLVM AOT compilation support |
| 📦 **Rich Standard Library** | **75+ native functions** for I/O, mathematics, string manipulation, collections, and system tasks |
| 🛡️ **Static Type System** | Compile-time type checking with type inference reduces runtime errors |
| 🧩 **Module System** | Robust `use` keyword for importing standard libraries, packages, and local files |
| 🔧 **PRM Package Manager** | Integrated **ProX Repository Manager** for dependency management and project scaffolding |
| 🏗️ **Multi-Phase Compiler** | Lexer → Parser (AST) → Type Checker → IR Optimizer → Bytecode/LLVM |
| ⏩ **Async/Await** | Native asynchronous programming with LLVM Coroutines support |
| 🔄 **Multi-Paradigm** | Native support for **Object-Oriented**, **Intent-Oriented**, and **Context-Oriented** programming |
| 🔍 **Developer Tools** | CLI with watch mode, LSP for IDE integration, comprehensive error reporting |
| 🎯 **Memory Safety** | Built-in garbage collector with mark-and-sweep algorithm |
| 🌐 **Cross-Platform** | First-class support for Windows, Linux, and macOS |

## 🏛️ The 10 Operational Pillars (v1.3.0)

ProXPL introduces 10 revolutionary concepts that redefine modern systems programming:

1.  **[Intent-Oriented Programming](docs/pillars/01_intent_oriented.md)**: Define *what* you want (`intent`), not just *how* to do it (`resolver`).
2.  **[Context-Aware Polymorphism](docs/pillars/02_context_aware.md)**: Adapt function behavior dynamically based on execution context (`context`, `layer`, `activate`).
3.  **[Autonomic Self-Healing (ASR)](docs/pillars/03_asr.md)**: Built-in failure recovery with `resilient` and `recovery` blocks.
4.  **[Intrinsic Security](docs/pillars/04_intrinsic_security.md)**: Taint analysis and `sanitize()` primitives baked into the type system.
5.  **[Chrono-Native Logic](docs/pillars/05_chrono_native.md)**: Data with expiration dates (`temporal`, `decay after`).
6.  **[Event-Driven Concurrency](docs/pillars/06_distributed_primitives.md)**: Distributed nodes and types (`distributed`, `node`) as first-class citizens.
7.  **[AI-Native Integration](docs/pillars/07_ai_native.md)**: Define, train, and run ML models (`model`, `train`, `predict`) natively.
8.  **[Quantum-Ready Syntax](docs/pillars/08_quantum_ready.md)**: Future-proof syntax for quantum operations (`quantum`, `superpose`, `entangle`).
9.  **[Hardware-Accelerated Math](docs/pillars/09_hardware_math.md)**: GPU kernel offloading (`gpu`, `kernel`) and tensor math.
10. **[Zero-Trust Security](docs/pillars/10_zero_trust.md)**: Mandatory identity verification blocks (`verify identity`) and crypto primitives.

---

## ⚡ Quick Start

### Your First Program

Create a file named `hello.prox`:

```javascript
// hello.prox
// Your first ProXPL program

func main() {
    print("Welcome to ProXPL!");
    
    let name = input("What is your name? ");
    print("Hello, " + name + "!");
    
    // Generate a random lucky number
    let lucky = random(1, 100);
    print("Here is a lucky number for you: " + to_string(lucky));
}

main();
```

### Run It

Using the ProXPL CLI:
```bash
prm run hello.prox
```

Or using the compiled executable:
```bash
./proxpl hello.prox
```

### Output

```text
Welcome to ProXPL!
What is your name? Alice
Hello, Alice!
Here is a lucky number for you: 42
```

---

## 📥 Installation

### Option 1: Pre-built Binaries (Recommended)

Download the latest release for your operating system:

- **Windows**: [Download `proxpl.exe`](https://github.com/ProgrammerKR/ProXPL/releases/latest)
- **Linux**: [Download `proxpl`](https://github.com/ProgrammerKR/ProXPL/releases/latest)
- **macOS**: [Download `proxpl-macos`](https://github.com/ProgrammerKR/ProXPL/releases/latest)

Add the executable to your system `PATH` for global access.

### Option 2: Build from Source

**Requirements**: 
- C/C++ Compiler (GCC 9+, Clang 10+, or MSVC 2019+)
- CMake 3.15+
- LLVM 10+ (for AOT compilation support)
- Git

**Build Instructions**:

```bash
# Clone the repository
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build the project
make

# Optional: Install system-wide
sudo make install
```

**Windows (Visual Studio)**:
```bash
mkdir build && cd build
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config Release
```

### Option 3: CLI via Node.js (Enhanced Developer Experience)

The ProXPL CLI provides watch mode, better logging, and development conveniences:

```bash
cd src/cli
npm install
npm link
```

Now use the `prox` command globally with enhanced features.

---

## 💻 Language Tour

### Variables & Data Types

ProXPL supports 12 core data types with static type checking:

```javascript
// Primitives
let count = 42;              // Integer
let price = 19.99;           // Float
let active = true;           // Boolean
let message = "Hello!";      // String

// Collections
let numbers = [1, 2, 3, 4, 5];                    // List
let config = {"host": "localhost", "port": 8080}; // Dictionary

// Type inference works automatically
let auto = 100;  // Inferred as Integer
```

### Functions & Control Flow

```javascript
// Function definition
func fibonacci(n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// Loops and iteration
func main() {
    for (let i = 0; i < 10; i = i + 1) {
        print("fib(" + to_string(i) + ") = " + to_string(fibonacci(i)));
    }
    
    // While loops
    let count = 0;
    while (count < 5) {
        print("Count: " + to_string(count));
        count = count + 1;
    }
}

main();
```

### Working with Collections

```javascript
func demonstrate_collections() {
    // Lists
    let items = [1, 2, 3];
    push(items, 4);           // Add element
    let first = items[0];     // Access by index
    let size = length(items); // Get size
    
    // Dictionaries
    let user = {"name": "Alice", "age": 30};
    user["email"] = "alice@example.com";  // Add key
    let name = user["name"];              // Access value
    
    // Iteration
    for (let i = 0; i < length(items); i = i + 1) {
        print(to_string(items[i]));
    }
}
```

### Tensor Operations (AI/Math)

ProXPL supports native tensor operations for AI and scientific computing:

```javascript
func tensor_demo() {
    // Define tensors using nested bracket syntax
    let matrix = [[1, 2], [3, 4]];
    let identity = [[1, 0], [0, 1]];
    
    // Matrix multiplication using @ operator
    let result = matrix @ identity;
    print(result);  // <tensor 2x2>
    
    // Dot product for 1D tensors
    let v1 = [1, 2, 3];
    let v2 = [4, 5, 6];
    let dot = v1 @ v2;
    print(dot);     // 32
}
```

### Module System

ProXPL uses the `use` keyword for modular programming:

```javascript
// Import standard library module
use std.math;

// Import from installed package
use http.client;

// Import local file (relative path)
use local_helper;

func main() {
    let result = std.math.sqrt(16);

    print("Square root of 16: " + to_string(result));
}
```

### Async/Await

ProXPL supports native asynchronous programming:

```javascript
async func fetchUser(id) {
    // Simulate non-blocking operation
    return {"id": id, "name": "User" + to_string(id)};
}

async func main() {
    print("Fetching user...");
    let user = await fetchUser(42);
    print("Got user: " + user["name"]);
}
```

### Standard Library Examples

```javascript
use std.io;
use std.fs;
use std.sys;

func showcase_stdlib() {
    // File I/O
    let content = read_file("data.txt");
    write_file("output.txt", "Hello from ProXPL!");
    
    // String operations
    let text = "ProXPL is awesome";
    let upper = to_upper(text);
    let parts = split(text, " ");
    
    // Math operations
    let result = sqrt(144);
    let power = pow(2, 8);
    let random_num = random(1, 100);
    
    // System operations
    let env_var = env("PATH");
    let current_time = time();
}
```

### Foreign Function Interface (FFI)

ProXPL can invoke native C functions from dynamic libraries (`.dll`, `.so`) using the `extern` keyword.

```javascript
// Load C standard library
extern "msvcrt.dll" "puts" func c_puts(text);
extern "msvcrt.dll" "abs" func c_abs(n);

c_puts("Hello from C!");
let dist = c_abs(-100);
```

---

## 📦 Package Manager (PRM)

ProXPL includes **PRM** (ProX Repository Manager), a built-in package manager for dependency management and project scaffolding.

### Basic Commands

```bash
# Initialize a new project
prm init my-project

# Install a package
prm install http-server

# List installed packages
prm list

# Search for packages
prm search json

# Update dependencies
prm update

# Remove a package
prm remove old-package
```

### Project Structure (`project.pxcf`)

```javascript
// project.pxcf
project {
    name: "my-web-server"
    version: "1.1.0"
    author: "Your Name <you@example.com>"
    license: "MIT"
}

compiler {
    optimize: true
    debug: false
    target: "native"
}

paths {
    src: "./src"
    build: "./build"
    entry: "src/main.prox"
}

dependencies {
    http: "1.3.0"
    json: "1.1.0"
}

runtime {
    threads: 8
    memory_limit: "1GB"
}
```

---

## 🏗️ Architecture Overview

ProXPL follows a professional multi-phase compiler architecture designed for maintainability, extensibility, and performance.

```mermaid
graph LR
    A[Source Code .prox] --> B[Scanner/Lexer]
    B --> C[Parser]
    C --> D[AST]
    D --> E[Type Checker]
    E --> F[IR Generator]
    F --> G[SSA Optimizer]
    G --> H{Compilation Mode}
    H -->|Bytecode| I[Bytecode Generator]
    H -->|AOT| J[LLVM Backend]
    I --> K[Bytecode Chunk]
    J --> L[Native Binary]
    K --> M[Stack-Based VM]
    L --> N[Direct Execution]
    M --> O[Runtime Execution]
    N --> O
```

### Core Components

| Component | Location | Responsibility |
|-----------|----------|----------------|
| **Scanner/Lexer** | [`src/lexer/scanner.c`](src/lexer/scanner.c) | Tokenizes source code into lexical tokens |
| **Parser** | [`src/parser/parser.c`](src/parser/parser.c) | Builds Abstract Syntax Tree (AST) from tokens |
| **Type Checker** | [`src/compiler/type_checker.c`](src/compiler/type_checker.c) | Validates types and enforces type safety |
| **IR Generator** | [`src/compiler/ir_gen.c`](src/compiler/ir_gen.c) | Generates intermediate representation (SSA form) |
| **IR Optimizer** | [`src/compiler/ir_opt.c`](src/compiler/ir_opt.c) | Performs optimizations on SSA IR |
| **Bytecode Compiler** | [`src/compiler/bytecode_gen.c`](src/compiler/bytecode_gen.c) | Emits optimized bytecode instructions |
| **LLVM Backend** | [`src/compiler/backend_llvm.cpp`](src/compiler/backend_llvm.cpp) | Generates LLVM IR for AOT native compilation |
| **Virtual Machine** | [`src/runtime/vm.c`](src/runtime/vm.c) | Stack-based VM that executes bytecode |
| **Garbage Collector** | [`src/runtime/gc.c`](src/runtime/gc.c) | Mark-and-sweep GC for automatic memory management |
| **Memory Manager** | [`src/runtime/memory.c`](src/runtime/memory.c) | Low-level memory allocation and tracking |
| **Standard Library** | [`src/stdlib/`](src/stdlib/) | Native implementations of 75+ built-in functions |

### Compilation Pipeline

1. **Lexical Analysis**: Source code is tokenized into meaningful symbols
2. **Syntax Analysis**: Tokens are parsed into an Abstract Syntax Tree
3. **Semantic Analysis**: Type checking and semantic validation
4. **IR Generation**: AST is lowered to SSA-based intermediate representation
5. **Optimization**: IR optimizations (constant folding, dead code elimination, etc.)
6. **Code Generation**: 
   - **Bytecode Path**: Generate bytecode for VM execution
   - **Native Path**: Generate LLVM IR → native binary via LLVM
7. **Execution**: Run on the stack-based VM or execute native binary

---

## 📂 Project Structure

```text
ProXPL/
├── include/                  # Public header files
│   ├── vm.h                  # Virtual machine interface
│   ├── compiler.h            # Compiler interface
│   ├── ast.h                 # AST node definitions
│   ├── stdlib_native.h       # Standard library declarations
│   └── gc.h                  # Garbage collector interface
├── src/
│   ├── main.c                # Entry point
│   ├── lexer/                # Lexical analysis
│   │   └── scanner.c
│   ├── parser/               # Syntax analysis
│   │   ├── parser.c
│   │   └── ast.c
│   ├── compiler/             # Code generation
│   │   ├── type_checker.c
│   │   ├── ir_gen.c
│   │   ├── ir_opt.c
│   │   ├── bytecode_gen.c
│   │   └── backend_llvm.cpp
│   ├── runtime/              # Runtime system
│   │   ├── vm.c              # Virtual machine
│   │   ├── gc.c              # Garbage collector
│   │   ├── memory.c          # Memory management
│   │   ├── chunk.c           # Bytecode storage
│   │   └── object.c          # Runtime object system
│   ├── stdlib/               # Standard library (native)
│   │   ├── stdlib_core.c
│   │   ├── stdlib_io.c
│   │   ├── stdlib_math.c
│   │   └── stdlib_string.c
│   ├── prm/                  # Package manager
│   │   ├── manifest.c
│   │   └── builder.c
│   └── utils/                # Utilities
│       └── error_report.c
├── lib/std/                  # Standard library (ProXPL)
├── tools/
│   ├── lsp/                  # Language Server Protocol
│   ├── bench/                # Benchmarking tools
│   └── prm_main.c            # PRM CLI entry
├── examples/                 # Example programs
├── tests/                    # Test suite
├── docs/                     # Documentation
│   ├── language-spec/        # Language specification
│   ├── stdlib/               # Standard library docs
│   └── architecture/         # Architecture guides
├── benchmarks/               # Performance benchmarks
├── CMakeLists.txt            # Build configuration
├── Makefile                  # Alternative build system
└── README.md                 # This file
```

---

## 📚 Documentation

Comprehensive documentation is available in the [`docs/`](docs/) directory:

- **[Language Specification](docs/spec.md)**: A detailed guide to ProXPL grammar, keywords, operators, data types, and core semantics.
- **[Standard Library Reference](docs/stdlib/)**: Detailed documentation for all built-in functions and modules.
- **[Architecture Guide](docs/architecture/)**: A deep dive into the compiler design and Virtual Machine (VM) internals.
- **[IR Specification](docs/ir-spec/)**: Documentation for the SSA (Static Single Assignment) intermediate representation.
- **[Build Guide](CONTRIBUTING.md)**: Platform-specific instructions for building ProXPL from source.
- **[Coding Standards](CODING_STANDARD.md)**: Code style guidelines and contribution workflow.
- **[Benchmarks](BENCHMARKS.md)**: Performance metrics, comparisons, and optimization notes.
- **[Ecosystem Design](ECOSYSTEM_DESIGN.md)**: Overview of the Standard Library and PRM (ProX Package Manager) architecture.

---


---

## 🧪 Testing

Run the comprehensive test suite:

```bash
# Build with tests enabled
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON ..
make

# Run all tests
make test

# Run specific test
./build/tests/lexer_test
./build/tests/parser_test
./build/tests/vm_test
```

---

## 🚀 Performance

ProXPL is designed for high performance through multiple optimization layers:

- **Zero-cost abstractions**: High-level features compile to efficient low-level code
- **SSA-based optimizations**: Constant folding, dead code elimination, common subexpression elimination
- **Bytecode JIT potential**: Foundation for future JIT compilation
- **LLVM backend**: Leverages industry-standard optimizer for native performance
- **Efficient GC**: Mark-and-sweep with tri-color marking (planned)

See [BENCHMARKS.md](BENCHMARKS.md) for detailed performance comparisons.

---

## 🗺️ Roadmap

- **v0.5.0 (Alpha)**: Core language features (variables, functions, control flow). ✅
- **v0.8.0**: Advanced memory management, closures, upvalues. ✅
- **v0.9.0**: Standard Library (fs, time, gc), IO improvements. ✅
- **v1.0.0 (Current)**:
  - **Object-Oriented Programming**: Classes, Methods, Inheritance, Properties. ✅
  - **Keywords**: `class`, `new`, `this`, `extends`, `interface`. ✅
  - **Runtime**: Optimized VM with Object Support. ✅
- **v1.3.0 (Current)**:
- **v1.3.3**:
  - **The 10 Operational Pillars**: Full frontend implementation of Intent-Oriented, Context-Aware, ASR, Intrinsic Security, Chrono-Native, Event-Concurreny, AI-Native, Quantum-Ready, Hardware-Math, and Zero-Trust features. ✅
  - **Frontend Feature Complete**: Lexer, Parser, AST, and Type Checker support all 10 pillars. ✅

### Unreleased (Development)
**Status**: In Progress
- ✅ **AI-Native Primitives**: Native `tensor` type support with multi-dimensional declaration (e.g., `tensor<float, 2x2>`).
- ✅ **Syntax**: Enhanced parser for tensor literals and dimension syntax.

### v1.3.3
**Status**: Released (Frontend)
- ✅ **10 Pillars Implementation**: Correctly parsing and semantically validating all revolutionary syntax constructs.
- ✅ **AI & Quantum**: `model`, `train`, `quantum`, `qubit` primitives.
- ✅ **Security**: `policy`, `tainted`, `encrypt`, `verify` support.
- ✅ **Distributed**: `distributed type`, `node` declarations.

### v1.1.0
**Status**: Released
- ✅ **Class-based OOP**: First-class support for Classes, Objects, Inheritance, and Interfaces.
- ✅ **Runtime Architecture**: Enhanced VM with Class, Instance, and BoundMethod support.
- ✅ **New Keywords**: `class`, `new`, `this`, `extends`, `interface`, `static`.
- ✅ **Inheritance**: Single inheritance model with superclass method lookup.
- ✅ **Access Control**: `pub`/`priv` visibility enforcement.
- ✅ **Constructors**: `init` constructor method.
- ✅ **Exception Handling**: `try`/`catch` blocks.
- ✅ **Verification**: Verified via `tests/oop_v1.0.0.prox`.

### Future Roadmap (2026–2028)
- 📋 **[v1.3.0](docs/releases/v1.3.0.md)** — Pattern Matching, Enums, Generics, FFI Stability.
- 📋 **[v1.4.0](docs/releases/v1.4.0.md)** — Async/Await Runtime, Generators, Networking (`std.net`), JSON.
- 📋 **[v1.5.0](docs/releases/v1.5.0.md)** — Traits, Result/Option Types, Generational GC, Debugger (DAP).
- 📋 **[v1.6.0](docs/releases/v1.6.0.md)** — ProX Studio Alpha, PRM Registry, Testing Framework, Macros.
- 📋 **[v1.7.0](docs/releases/v1.7.0.md)** — WebAssembly Target, String Templates, Operator Overloading, Formatter.
- 📋 **[v1.8.0](docs/releases/v1.8.0.md)** — Channels, Actors, Database Connectivity, LSP v2, Compile-Time Eval.
- 📋 **[v1.9.0](docs/releases/v1.9.0.md)** — Cross-Compilation, Embedded API, Security Hardening, API Freeze.
- 🚀 **[v2.0.0](docs/releases/v2.0.0.md)** — Self-Hosting Compiler, JIT Compiler, Effect System, Production Stable.

## 🛠️ Contributing

We warmly welcome contributions! ProXPL is an excellent project for learning compiler design, language implementation, and systems programming.

### How to Contribute

1. **Fork** the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Follow the [Coding Standards](CODING_STANDARD.md)
4. Write tests for new features
5. Commit your changes (`git commit -m 'Add amazing feature'`)
6. Push to the branch (`git push origin feature/amazing-feature`)
7. Open a **Pull Request**

### Areas for Contribution

- 🐛 Bug fixes and stability improvements
- ✨ New standard library functions
- 📝 Documentation and tutorials
- 🧪 Test coverage expansion
- ⚡ Performance optimizations
- 🎨 IDE and editor plugins
- 📦 Community packages

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines and [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) for community standards.

---

## 📄 License

This project is licensed under the ProXPL Professional License - see the [LICENSE](LICENSE) file for details.

---

<p align="center">
  <b>Built with ❤️ by the ProXPL Community</b><br>
  <i>Making programming easy, accessible and enjoyable</i>
</p>

<p align="center">
  <sub>ProXPL - A Modern Programming Language for the Future</sub>
</p>

