# ProXPL Programming Language

ProXPL is a robust, beginner-friendly, and powerful educational programming language designed to bridge the gap between simple scripting and full-scale application development. It offers a clean syntax, strong typing discipline (where helpful), and a rich standard library out of the box.

## 🚀 Features

- **Clean Syntax**: Inspired by Python, JavaScript, and C, offering a familiar yet distinct experience.
- **Rich Standard Library**: **75+ built-in functions** covering I/O, Math, Strings, Collections, Date/Time, and System utilities.
- **Modern Paradigms**: Supports functions, classes, async/await, and error handling.
- **Strict Specification**: Defined by a clear specification of 45 keywords, 42 operators, and 12 core data types.
- **Extensible**: Designed with a modular architecture (Lexer, Parser, VM) suitable for educational study and extension.

## 📦 Installation

To execute ProXPL code, you need Python 3.8+ installed.

1. Clone the repository:
   ```bash
   git clone https://github.com/ProgrammerKR/ProXPL.git
   cd ProXPL
   ```

2. Run the tests to verify setup:
   ```bash
   python tests/test_specs.py
   ```

## ⚡ Quick Start

Create a file named `hello.prox` with the following content:

```javascript
// hello.prox
func main() {
    let name = input("Enter your name: ");
    print("Hello, " + name + "!");
    
    // Use standard library
    let r = random(1, 100);
    print("Your lucky number is: " + to_string(r));
}

main();
```

Run it (assuming CLI tool is set up, or via python runner):
```bash
python -m core.main hello.prox
```

## 📚 Documentation

Detailed documentation is available in the `docs/` directory:

- [Language Specification](docs/language-spec.md): Keywords, Types, Operators, and Built-ins.

## 🛠️ Contributing

We welcome contributors! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for details on how to get started.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
