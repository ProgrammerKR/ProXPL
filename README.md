# ProXPL Programming Language

<p align="center">
  <b>A Modern, Educational, Production-Ready Programming Language Compiler</b>
</p>

<p align="center">
  <strong>Clean Syntax • Strong Typing • Rich Standard Library • C-Based Runtime</strong>
</p>

---

## 📋 Table of Contents

1. [What is ProXPL?](#what-is-proxpl)
2. [Key Features](#key-features)
3. [Quick Start](#quick-start)
4. [Installation](#installation)
5. [Language Features](#language-features)
6. [Architecture Overview](#architecture-overview)
7. [Project Structure](#project-structure)
8. [Building from Source](#building-from-source)
9. [Documentation](#documentation)
10. [Contributing](#contributing)
11. [Roadmap](#roadmap)
12. [License](#license)

---

## What is ProXPL?

**ProXPL** is a modern programming language designed for:

- 🎓 **Educational Use**: Learn compiler design, interpreters, and language implementation
- 🏗️ **System Tools**: Build fast, efficient command-line tools and utilities
- 📚 **Language Exploration**: Experiment with language design paradigms
- ⚡ **Production**: Deploy with a powerful, type-safe runtime

ProXPL bridges the gap between **simplicity** (Python-like syntax) and **performance** (C-based runtime). It combines:
- **Familiar Syntax**: Inspired by Python and JavaScript
- **Strong Type System**: Compile-time and runtime type checking
- **Fast Runtime**: Bytecode-compiled and executed on a custom VM
- **Rich Standard Library**: 75+ built-in functions for common tasks
- **Modular Architecture**: Clear separation of lexer, parser, compiler, and VM

## 🚀 Key Features

*   **Clean & Modern Syntax**: Familiar curly-brace syntax with Python-like readability.
*   **Rich Standard Library**: **75+ built-in functions** for I/O, Math, Strings, Collections, Date/Time, and System utilities.
*   **Strong Core**: Supports functions, classes, async/await, and error handling.
*   **Zero-Dependency Runtime**: Compiles to standalone executables (via PyInstaller).
*   **Strict Specification**: Well-defined grammar with 45 keywords, 42 operators, and 12 core data types.

## 📥 Installation

### Option 1: Download Binaries (Recommended)
No Python installation required. Download the latest release for your OS:

- **Windows**: [Download `prox.exe`](https://github.com/ProgrammerKR/ProXPL/releases/latest)
- **Linux/macOS**: [Download `prox`](https://github.com/ProgrammerKR/ProXPL/releases/latest)

Add the downloaded file to your system `PATH` to run it from anywhere.

### Option 2: Install via Pip
If you have Python 3.8+ installed, you can install ProXPL as a package:

```bash
pip install .
```
*(Run this command in the root directory of the repository)*

### Option 3: Run from Source
Clone the repository and run directly:
```bash
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL
pip install -r requirements.txt  # If any
python -m cli.main examples/hello.prox
```

## ⚡ Getting Started

### 1. Your First Program
Create a file named `hello.prox`:

```javascript
// hello.prox
func main() {
    let name = input("Enter your name: ");
    print("Hello, " + name + "!");
}

main();
```

### 2. Run It
Open your terminal and run:

```bash
prox hello.prox
```

### 3. Build It (Coming Soon)
ProXPL handles immediate execution today. Future versions will support compiling to bytecode.

## 📚 Documentation

Detailed documentation is available to help you master ProXPL:

- **[Language Specification](docs/language-spec.md)**: Explore the complete syntax, keywords, types, and operator reference.
- **[Standard Library](docs/language-spec.md#standard-library)**: Reference for all built-in functions.

## 📂 Project Structure

```text
ProXPL/
├── cli/            # Command-line interface entry points
├── core/           # Compiler core (Lexer, Parser, AST, VM)
├── docs/           # Documentation and Specifications
├── examples/       # Example ProXPL programs
├── stdlib/         # Standard library implementation
├── tests/          # Unit and integration tests
└── scripts/        # Build and utility scripts
```

## Modules & Packages

ProXPL supports a robust module system using the `use` keyword.

```prox
// Import standard library module
use math;

// Import installed package
use utils/io;

// Import local file
use my_local_module;

print(PI);
```

Circular imports are detected and prevent execution. Modules are executed only once.

## Package Manager (PRM)

ProXPL includes **PRM (ProX Repository Manager)** to manage dependencies.

**Commands:**
- `prm install <package>`: Download and install a package.
- `prm list`: List installed packages.
- `prm search <keyword>`: Search the registry.
- `prm remove <package>`: Uninstall a package.

Example:
```bash
python cli/prm.py install utils/io
```
(Note: You can alias `prm` to run this script directly)

## 🛠️ Contributing

We welcome contributors! ProXPL is an educational project perfect for learning about compilers and interpreters.

1.  Fork the repository.
2.  Create your feature branch (`git checkout -b feature/AmazingFeature`).
3.  Commit your changes (`git commit -m 'Add some AmazingFeature'`).
4.  Push to the branch (`git push origin feature/AmazingFeature`).
5.  Open a Pull Request.

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct and development process.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
