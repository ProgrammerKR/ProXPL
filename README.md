# ProXPL Programming Language

<<<<<<< HEAD
<p align="center">
  <b>A Professional & Modern Programming Language Compiler</b>
=======
<div align="center">

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/ProgrammerKR/ProXPL)
[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/ProgrammerKR/ProXPL/releases)
[![Platform](https://img.shields.io/badge/platform-win%20%7C%20linux%20%7C%20macos-lightgrey.svg)]()

<b>A Modern, Statically-Typed Programming Language with a Bytecode VM Runtime</b>

<p>
  <strong>Clean Syntax • Strong Type System • Rich Standard Library • C-Based Runtime</strong>
>>>>>>> feature/opcode-tests
</p>

[Quick Start](#-quick-start) •
[Installation](#-installation) •
[Documentation](#-documentation) •
[Architecture](#-architecture-overview) •
[Contributing](#-contributing)

</div>

---

## 📖 About ProXPL

**ProXPL** is a modern programming language designed to bridge the gap between the simplicity of interpreted languages like Python and the performance of C-based runtimes. It is built for a wide range of applications, from educational compiler theory to efficient system tooling.

### The Philosophy
*   **Clarity First**: Familiar curly-brace syntax (similar to JavaScript/Go) ensures a low learning curve.
*   **Type Safety**: A strong type system (Compile-time & Runtime) prevents common errors before they happen.
*   **Performance**: Source code is compiled to bytecode and executed on a custom, optimized stack-based Virtual Machine.
*   **Batteries Included**: A rich standard library allows you to build useful applications immediately without external dependencies.

---

<<<<<<< HEAD
## What is ProXPL?

**ProXPL** is a modern, general-purpose programming language designed for **clarity, performance, and long-term maintainability**.  
It combines a **clean, expressive syntax** with a **structured and strongly typed core**, making it suitable for building reliable software systems at any scale.

ProXPL is implemented with a **bytecode compiler and a C-based virtual machine**, providing predictable execution and a solid foundation for performance-oriented workloads.

### Design Goals

- **Readable & Expressive Syntax**  
  A clear, modern syntax inspired by high-level languages, focused on developer productivity and code clarity.

- **Strong Typing & Defined Semantics**  
  Explicit language rules and type safety to improve correctness and reduce runtime errors.

- **Efficient Runtime**  
  Bytecode compilation executed on a custom virtual machine for consistent and optimized execution.

- **Modular & Scalable Architecture**  
  Clean separation between lexer, parser, compiler, and VM, enabling maintainable and extensible implementations.

### Use Cases

ProXPL is well-suited for:

- Command-line tools and system utilities  
- Backend services and application logic  
- Automation and scripting with production reliability  
- Structured, long-lived software projects  

ProXPL aims to provide a **balanced programming model** that emphasizes simplicity without sacrificing control or performance.

---

ProXPL bridges the gap between **simplicity** (Python-like syntax) and **performance** (C-based runtime). It combines:
- **Familiar Syntax**: Inspired by Python and JavaScript
- **Strong Type System**: Compile-time and runtime type checking
- **Fast Runtime**: Bytecode-compiled and executed on a custom VM
- **Rich Standard Library**: 75+ built-in functions for common tasks
- **Modular Architecture**: Clear separation of lexer, parser, compiler, and VM

=======
>>>>>>> feature/opcode-tests
## 🚀 Key Features

| Feature | Description |
|---------|-------------|
| 🔤 **Modern Syntax** | Clean, readable syntax inspired by Python and JavaScript. |
| ⚡ **Fast Runtime** | Zero-dependency binaries compiled to bytecode, executed on a custom VM. |
| 📦 **Rich Stdlib** | **75+ built-in functions** for I/O, Math, Strings, Collections, and System tasks. |
| 🛡️ **Strong Typing** | Statically typed with support for type inference and explicit conversion. |
| 🧩 **Modularity** | robust `use` keyword system for importing modules and local files. |
| 🛠️ **PRM Included** | Integrated **ProX Repository Manager** for managing packages. |
| 🏗️ **Architecture** | Classic Compiler Design: Lexer → Parser (AST) → Compiler → Bytecode → VM. |

---

## ⚡ Quick Start

### 1. Your First Program
Create a file named `hello.prox`:

```javascript
// hello.prox
func main() {
    print("Welcome to ProXPL!");

    let name = input("Enter your name: ");
    print("Hello, " + name + "!");

    // Generate a random number using the standard library
    let lucky = random(1, 100);
    print("Your lucky number is: " + to_string(lucky));
}

main();
```

### 2. Run It
```bash
prox hello.prox
```

<<<<<<< HEAD
### 3. Build It (Coming Soon)
ProXPL supports compiling to bytecode.
=======
### 3. Output
```text
Welcome to ProXPL!
Enter your name: Alice
Hello, Alice!
Your lucky number is: 42
```
>>>>>>> feature/opcode-tests

---

## 📥 Installation

### Option 1: Pre-built Binaries (Recommended)
No compilation required. Download the latest release for your OS:

*   **Windows**: [Download `prox.exe`](https://github.com/ProgrammerKR/ProXPL/releases/latest)
*   **Linux**: [Download `prox`](https://github.com/ProgrammerKR/ProXPL/releases/latest)
*   **macOS**: [Download `prox-macos`](https://github.com/ProgrammerKR/ProXPL/releases/latest)

Add the executable to your system `PATH` to run it from anywhere.

### Option 2: Build from Source (CMake)
Requirements: **GCC/Clang** (C99+), **CMake** 3.10+, **Git**.

```bash
# Clone the repository
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL

# Create build directory
mkdir build && cd build

# Configure and build
cmake -DCMAKE_BUILD_TYPE=Release ..
make

# Install (Optional)
sudo make install
```

---

## 💻 Language Tour

### Variables & Collections
ProXPL supports 12 core data types, including Lists and Dictionaries.

```javascript
let message = "System Active";
let count = 42;
let list = [1, 2, 3, 4];
let config = {"host": "localhost", "port": 8080};

push(list, 5);
print(config["host"]);
```

### Functions & Control Flow
```javascript
func fibonacci(n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

func main() {
    for (let i = 0; i < 10; i = i + 1) {
        print("fib(" + to_string(i) + ") = " + to_string(fibonacci(i)));
    }
}
```

### Module System
ProXPL uses the `use` keyword for modularity. Circular imports are handled automatically.

```javascript
use math;           // Import standard library
use utils/io;       // Import installed package
use local_helper;   // Import local file

print(math.sqrt(16));
```

### Package Manager (PRM)
Manage dependencies using the built-in **ProX Repository Manager**.

```bash
prm install utils/io      # Install a package
prm list                  # List installed packages
prm search http           # Search the registry
```

---

## 🏗️ Architecture Overview

ProXPL follows a professional three-phase compiler architecture aimed at maintainability and performance.

```mermaid
graph LR
    A[Source Code] --> B[Lexer]
    B --> C[Parser / AST]
    C --> D[Type Checker]
    D --> E[Compiler]
    E --> F[Bytecode Chunk]
    F --> G[Virtual Machine]
    G --> H[Execution]
```

### Core Components
| Component | Location | Responsibility |
|-----------|----------|----------------|
| **Lexer** | `src/lexer/scanner.c` | Converts source code into tokens. |
| **Parser** | `src/parser/parser.c` | Constructs the Abstract Syntax Tree (AST). |
| **Compiler** | `src/runtime/compiler.c` | Emits optimized bytecode instructions. |
| **VM** | `src/runtime/vm.c` | Stack-based virtual machine that executes bytecode. |
| **Memory** | `src/runtime/memory.c` | Garbage collection and memory allocation. |

---

## 📂 Project Structure

```text
ProXPL/
├── include/              # Header files (Interfaces)
│   ├── vm.h
│   ├── compiler.h
│   └── stdlib_native.h
├── src/                  # Implementation Source
│   ├── main.c            # Entry point
│   ├── lexer/            # Tokenizer logic
│   ├── parser/           # AST and Type Checking
│   ├── runtime/          # VM and Garbage Collector
│   └── stdlib/           # Native Standard Library impl
├── cli/                  # PRM and CLI tools
├── examples/             # ProXPL code samples
├── docs/                 # Specifications and Guides
└── tests/                # Unit and Integration tests
```

---

## 📚 Documentation

Detailed documentation is available in the `docs/` directory:

*   **[Language Specification](docs/language-spec.md)**: Grammar, Keywords, and Operators.
*   **[Standard Library Reference](docs/stdlib.md)**: Documentation for all 75+ built-in functions.
*   **[Architecture Guide](docs/architecture.md)**: Deep dive into the VM and Compiler internals.
*   **[Build Guide](docs/BUILD_GUIDE.md)**: Platform-specific instructions (Windows/Linux/Mac).

---

## 🛣️ Roadmap

*   **v0.1.0** (Current): Core language, VM, Stdlib, Basic I/O.
*   **v0.2.0**: Class-based OOP, Advanced Error Handling.
*   **v0.3.0**: Native Foreign Function Interface (FFI).
*   **v1.0.0**: Production Stability, Async/Await Support.

---

## 🛠️ Contributing

We welcome contributors! ProXPL is an excellent project for learning compiler design.

1.  **Fork** the repository.
2.  Create a feature branch (`git checkout -b feature/AmazingFeature`).
3.  Commit your changes following the [Coding Standard](CODING_STANDARD.md).
4.  Push to the branch and Open a **Pull Request**.

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct.

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

<p align="center">
  Built with ❤️ by the ProXPL Community
</p>