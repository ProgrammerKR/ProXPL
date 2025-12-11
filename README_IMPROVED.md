# ProXPL Programming Language

<p align="center">
  <b>A Modern, Production-Ready Programming Language with Bytecode VM Runtime</b>
</p>

<p align="center">
  <strong>Clean Syntax • Strong Type System • Rich Standard Library • C-Based Runtime</strong>
</p>

<p align="center">
  <a href="#quick-start">Quick Start</a> •
  <a href="#installation">Installation</a> •
  <a href="#documentation">Documentation</a> •
  <a href="#contributing">Contributing</a> •
  <a href="#license">License</a>
</p>

---

## Table of Contents

1. [About ProXPL](#about-proxpl)
2. [Key Features](#key-features)
3. [Language Goals](#language-goals)
4. [Quick Start](#quick-start)
5. [Installation & Building](#installation--building)
6. [Usage Examples](#usage-examples)
7. [Architecture Overview](#architecture-overview)
8. [Documentation](#documentation)
9. [Project Structure](#project-structure)
10. [Contributing](#contributing)
11. [Support & Community](#support--community)
12. [Roadmap](#roadmap)
13. [License](#license)

---

## About ProXPL

**ProXPL** is a modern, statically-typed programming language designed for:

- 🎓 **Educational Purpose**: Learn compiler design, interpreter implementation, and language architecture
- 🏗️ **System Tools**: Build efficient command-line utilities and system tools
- 💡 **Language Exploration**: Experiment with programming paradigms and language design
- ⚡ **Production Use**: Deploy type-safe, high-performance applications

### The ProXPL Philosophy

ProXPL bridges the gap between **Python-like simplicity** and **C-like performance**. It provides:

- **Familiar Syntax**: Curly-brace syntax inspired by JavaScript/Go with Python readability
- **Strong Type System**: Compile-time type checking with runtime validation
- **Fast Execution**: Bytecode compilation to a custom stack-based virtual machine
- **Rich Standard Library**: 75+ built-in functions for common programming tasks
- **Clean Architecture**: Clear separation of lexer, parser, compiler, and VM components

---

## Key Features

| Feature | Description |
|---------|-------------|
| 🔤 **Clean Syntax** | Familiar C-style syntax with Python-like readability |
| 🔒 **Strong Types** | Compile-time and runtime type checking for safety |
| ⚡ **Fast Runtime** | Bytecode-compiled and executed on a custom VM |
| 📦 **Rich Standard Library** | 75+ built-in functions for I/O, math, strings, collections, datetime, and system |
| 🎯 **Focused Design** | 45 keywords, 42 operators, 12 core data types |
| 📚 **Well-Documented** | Comprehensive guides, tutorials, and API documentation |
| 🔧 **Easy to Extend** | Modular architecture for adding new features |
| 🧪 **Testable** | Full test suite with unit and integration tests |

### Standard Library Highlights

- **I/O Functions**: print, input, read_file, write_file
- **Math Functions**: sqrt, pow, sin, cos, log, random, and more
- **String Functions**: split, join, replace, upper, lower, trim, substring
- **Collections**: Lists, dictionaries, sets with native operations
- **System**: Platform detection, file operations, process execution
- **DateTime**: Timestamps, date formatting, sleep timers
- **Type Conversion**: Automatic and explicit type conversion utilities

---

## Language Goals

ProXPL was designed with these core principles:

1. **Clarity First**: Code should be easy to read and understand
2. **Type Safety**: Catch errors at compile-time when possible
3. **Performance**: Bytecode VM provides fast execution
4. **Simplicity**: Learn the language in a weekend, not a year
5. **Modularity**: Components should be independent and reusable
6. **Community**: Foster learning and collaboration

---

## Quick Start

### 1. Your First Program

Create a file named `hello.prox`:

```javascript
// hello.prox - Your first ProXPL program
func main() {
    print("Welcome to ProXPL!");
    
    let name = input("What is your name? ");
    print("Hello, " + name + "!");
    
    let lucky = random(1, 100);
    print("Your lucky number is: " + to_string(lucky));
}

main();
```

### 2. Run It

**Using the executable:**
```bash
prox hello.prox
```

**Output:**
```
Welcome to ProXPL!
What is your name? Alice
Hello, Alice!
Your lucky number is: 42
```

### 3. More Examples

See the [examples/](examples/) directory for:
- [hello.prox](examples/hello.prox) - Hello World
- [fibonacci.prox](examples/fibonacci.prox) - Recursion demo
- [calculator.prox](examples/calculator.prox) - Interactive calculator
- [use_stdlib.prox](examples/use_stdlib.prox) - Standard library showcase

---

## Installation & Building

### Option 1: Download Pre-built Binaries

Pre-built executables are available for all major platforms:

- **Windows**: [Download prox.exe](https://github.com/ProgrammerKR/ProXPL/releases/latest)
- **Linux**: [Download prox](https://github.com/ProgrammerKR/ProXPL/releases/latest)
- **macOS**: [Download prox-macos](https://github.com/ProgrammerKR/ProXPL/releases/latest)

Add to your system `PATH` and run anywhere:
```bash
prox your_script.prox
```

### Option 2: Build from Source

#### Prerequisites

- **GCC or Clang** compiler (C99 standard)
- **CMake** 3.10+ (recommended)
- **Make** (for Makefile method)
- **Git** (to clone the repository)

#### Build Steps

**Using CMake (Recommended):**

```bash
# Clone the repository
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
make

# Optional: Install to system
sudo make install
```

**Using Make:**

```bash
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL/src

# Build
make

# Run directly
./prox ../examples/hello.prox
```

**Platform-Specific Notes:**

- **Linux**: Uses standard GCC toolchain
- **macOS**: Ensure Command Line Tools installed (`xcode-select --install`)
- **Windows**: Use MinGW or WSL2

See [BUILD_GUIDE.md](docs/BUILD_GUIDE.md) for detailed platform-specific instructions.

---

## Usage Examples

### Example 1: Variables and Types

```javascript
// Variables and type system
let message = "Hello";
let count = 42;
let pi = 3.14159;
let active = true;

print(message + " " + to_string(count));

let list = [1, 2, 3, 4, 5];
let dict = {"name": "Alice", "age": 30};
```

### Example 2: Functions and Control Flow

```javascript
// Function definition
func fibonacci(n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

func main() {
    for (let i = 0; i < 10; i = i + 1) {
        print("fib(" + to_string(i) + ") = " + to_string(fibonacci(i)));
    }
}

main();
```

### Example 3: Error Handling

```javascript
func safe_divide(a, b) {
    if (b == 0) {
        return null;
    }
    return a / b;
}

func main() {
    let result = safe_divide(10, 2);
    if (result != null) {
        print("Result: " + to_string(result));
    }
}

main();
```

### Example 4: String Manipulation

```javascript
func main() {
    let text = "  Hello, World!  ";
    
    print(len(text));                    // 17
    print(trim(text));                   // "Hello, World!"
    print(upper(text));                  // "  HELLO, WORLD!  "
    print(lower(text));                  // "  hello, world!  "
    
    let words = split(trim(text), " ");
    print(len(words));                   // 2
    print(words[0]);                     // "Hello,"
}

main();
```

### Example 5: Collections

```javascript
func main() {
    // Lists
    let nums = [10, 20, 30];
    push(nums, 40);
    print(nums);                         // [10, 20, 30, 40]
    
    // Dictionaries
    let person = {"name": "Bob", "age": 25};
    print(person["name"]);               // "Bob"
    print(keys(person));                 // ["name", "age"]
    
    // Range
    let range = range(1, 5);             // [1, 2, 3, 4]
    print(range);
}

main();
```

---

## Architecture Overview

ProXPL follows a classic **three-phase compiler architecture**:

```
SOURCE CODE
    ↓
LEXICAL ANALYSIS (Tokenization)
    ↓
PARSING (AST Construction)
    ↓
TYPE CHECKING
    ↓
COMPILATION (Bytecode Generation)
    ↓
EXECUTION (Virtual Machine)
    ↓
OUTPUT
```

### Core Components

| Component | Purpose | File(s) |
|-----------|---------|---------|
| **Lexer** | Tokenize source code | `src/lexer/scanner.c` |
| **Parser** | Build Abstract Syntax Tree | `src/parser/parser.c`, `src/parser/ast.c` |
| **Type Checker** | Validate types | `src/parser/type_checker.c` |
| **Compiler** | Generate bytecode | `src/runtime/compiler.c` |
| **VM** | Execute bytecode | `src/runtime/vm.c` |
| **Standard Library** | Built-in functions | `src/stdlib/*.c` |
| **Memory Manager** | GC and allocation | `src/runtime/memory.c` |

For detailed architecture, see [ARCHITECTURE.md](ARCHITECTURE.md).

---

## Documentation

### Core Documentation

- **[README.md](README.md)** - This file; overview and getting started
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Detailed system architecture
- **[LANGUAGE_SPEC.md](docs/language-spec/grammar.md)** - Complete language specification
- **[STDLIB_DOC.md](STDLIB_DOC.md)** - Standard library reference
- **[CODING_STANDARD.md](CODING_STANDARD.md)** - Development guidelines

### Getting Started Guides

- **[BUILD_GUIDE.md](docs/BUILD_GUIDE.md)** - Platform-specific build instructions
- **[Tutorials](docs/tutorials/)** - Step-by-step learning guides
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - How to contribute

### Advanced Topics

- **[Bytecode Format](docs/ir-spec/bytecode_format.md)** - VM bytecode specification
- **[Type System](docs/language-spec/reference_manual.md)** - Type checking details
- **[Security Model](docs/security/security_model.md)** - Security considerations

---

## Project Structure

```
ProXPL/
├── include/              # Header files
│   ├── vm.h             # Virtual machine interface
│   ├── compiler.h       # Compiler interface
│   ├── parser.h         # Parser interface
│   ├── scanner.h        # Lexer interface
│   ├── ast.h            # AST definitions
│   ├── value.h          # Value type system
│   ├── object.h         # Runtime objects
│   ├── memory.h         # Memory management
│   ├── stdlib_native.h  # Standard library
│   └── ...
│
├── src/                 # Implementation files
│   ├── main.c           # Entry point
│   ├── lexer/
│   │   └── scanner.c    # Tokenizer
│   ├── parser/
│   │   ├── parser.c     # Parser
│   │   ├── ast.c        # AST builder
│   │   └── type_checker.c
│   ├── runtime/
│   │   ├── vm.c         # Virtual machine
│   │   ├── compiler.c   # Bytecode compiler
│   │   ├── chunk.c      # Bytecode chunks
│   │   ├── value.c      # Values and types
│   │   ├── object.c     # Runtime objects
│   │   ├── memory.c     # Memory management
│   │   └── debug.c      # Debugging utilities
│   └── stdlib/
│       ├── stdlib_core.c       # Core functions
│       ├── io_native.c         # I/O functions
│       ├── math_native.c       # Math functions
│       ├── string_native.c     # String functions
│       ├── convert_native.c    # Type conversion
│       └── system_native.c     # System functions
│
├── examples/            # Example programs
│   ├── hello.prox
│   ├── fibonacci.prox
│   ├── calculator.prox
│   └── ...
│
├── tests/               # Test suite
│   ├── unit/            # Unit tests
│   ├── integration/      # Integration tests
│   └── fixtures/        # Test data
│
├── docs/                # Documentation
│   ├── BUILD_GUIDE.md
│   ├── language-spec/
│   ├── architecture/
│   ├── ir-spec/
│   ├── tutorials/
│   └── ...
│
├── CMakeLists.txt       # CMake configuration
├── Makefile             # Make configuration
├── ARCHITECTURE.md      # Architecture guide
├── LANGUAGE_SPEC.md     # Language specification
├── STDLIB_DOC.md        # Standard library docs
├── CODING_STANDARD.md   # Code guidelines
├── CONTRIBUTING.md      # Contribution guide
└── LICENSE              # MIT License
```

---

## Contributing

We welcome contributions! Whether you want to:

- 🐛 Report bugs or request features
- 📝 Improve documentation
- 💻 Contribute code
- 🧪 Add tests
- 📢 Help with community

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

### Quick Contribution Steps

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Make** your changes following [CODING_STANDARD.md](CODING_STANDARD.md)
4. **Test** thoroughly with `make test`
5. **Commit** with clear messages
6. **Push** to your fork
7. **Open** a Pull Request

### Development Setup

```bash
# Clone your fork
git clone https://github.com/YOUR_USERNAME/ProXPL.git
cd ProXPL

# Create development build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Run tests
make test

# Run linter
make lint
```

---

## Support & Community

### Getting Help

- 📖 **Documentation**: See [docs/](docs/) for comprehensive guides
- 🐛 **Issue Tracker**: [GitHub Issues](https://github.com/ProgrammerKR/ProXPL/issues)
- 💬 **Discussions**: [GitHub Discussions](https://github.com/ProgrammerKR/ProXPL/discussions)
- 📧 **Email**: contact@proxpl.dev

### Community Guidelines

- Be respectful and inclusive
- Follow [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md)
- Help others learn and grow
- Share your projects and ideas

---

## Roadmap

### Version 0.1.0 (Current)
- ✅ Core language features
- ✅ Standard library (75+ functions)
- ✅ Documentation and examples
- ✅ Build infrastructure

### Version 0.2.0 (Planned)
- 📋 Class-based OOP
- 📋 Module system
- 📋 Advanced error handling
- 📋 Performance optimizations

### Version 0.3.0 (Future)
- 📋 Async/await support
- 📋 Standard package manager
- 📋 FFI (Foreign Function Interface)
- 📋 Full Unicode support

### Version 1.0.0 (Long Term)
- 📋 Production stability guarantee
- 📋 Extensive ecosystem
- 📋 Enterprise features

For more details, see [CHANGELOG.md](CHANGELOG.md).

---

## License

ProXPL is released under the **MIT License**. See [LICENSE](LICENSE) for details.

```
MIT License

Copyright (c) 2024-2025 ProXPL Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
```

---

## Acknowledgments

ProXPL is built on principles learned from:

- **Lox** (Robert Nystrom's language learning project)
- **Python** (Clean syntax and standard library design)
- **Go** (Simplicity and pragmatism)
- **Rust** (Type safety and performance)

Special thanks to all contributors and the community!

---

**Happy coding with ProXPL!** 🚀

For the latest updates, visit: https://github.com/ProgrammerKR/ProXPL
