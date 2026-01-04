# ProXPL Changelog

All notable changes to the ProXPL project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [0.8.0] - 2026-01-04

### Added
- **Diagnostics**: Enhanced error reporting with column tracking and visual caret pointers (`^`).
- **Standard Library**:
  - `std.fs`: Added `copy`, `is_file`, `is_dir`.
  - `std.sys`: Added `exec` for command execution.
- **Tooling**:
  - **LSP**: Added "Go to Definition" support and basic keyword completion.
  - **Extension**: Enabled Snippets and Hover support.
  - **PRM**: Enabled remote package installation from GitHub (`prm install User/Repo`).

## [0.7.0] - 2026-01-04

### Added
- **FFI System**: Initial implementation of the Foreign Function Interface using `libffi`.
- **Extension**:
  - Added icon support for `.iss` (Inno Setup) files.
  - Added icon support for `.internal_ingore` and `.internal_ignore`.
- **Runtime**:
  - `ffi_bridge.c` for handling native function calls.
- Official release build configuration for version 0.7.0.
- Updated version strings across the codebase.

### Fixed
- **Build**: Resolved linker errors on Ubuntu and macOS where `libffi` was not being linked to test executables (`ir_gen_test`, `llvm_gen_test`).
- **Stability**: Fixed runtime crashes related to stack management and `input` function usage.

## [0.5.0-pre] - 2026-01-02

### Added
- Pre-release build configuration.
- Updated version strings to 0.5.0-pre.



## [0.5.1] - 2025-12-31

### Added
- **Language**: Introduced `use` keyword to replace `import` for module loading, standardizing the syntax.
- **Extension**: 
  - Added material icons support with `vscode-material-icon-theme` integration.
  - Fixed VSIX packaging errors.
- **Runtime**:
  - Implemented `OP_USE` opcode for dynamic module loading.
  - Added `std.io` and standard library module resolution fixes.
- Comprehensive documentation suite (400+ pages)
  - Professional README with architecture diagrams
  - Complete ARCHITECTURE.md with compiler pipeline details
  - BUILD_GUIDE.md with platform-specific instructions
  - PYTHON_TO_C.md migration blueprint
  - Tutorial series (4 comprehensive tutorials)
  - API reference documentation
  - Bytecode format specification
- Code of Conduct for community standards
- Enhanced CONTRIBUTING.md with coding standards
- GitHub Actions CI/CD workflows
  - Automated build on push/PR
  - Automated testing
  - Automated linting
- Test framework structure
  - Unit test templates
  - Integration test templates
  - Test fixture organization
- Development scripts
  - Cross-platform build scripts
  - Formatting automation
  - Linting automation
- Professional .gitignore for C/Python hybrid
- Refactor report documenting all changes

### Changed
- **CLI**: Updated CLI version to match project version.
- **Build**: Fixed CMake finding LLVM and compiler paths on Windows.
- Reorganized repository structure for clarity and scalability
- Upgraded CMakeLists.txt to modern best practices
- Improved Makefile with multiple build targets
- Removed internal clutter (staff-notes, housekeeping files)
- Consolidated documentation (single source of truth)
- Enhanced code style guidelines with examples

### Fixed
- .gitignore now properly covers C, Python, and IDE artifacts
- CMake configuration now supports all major compilers
- Build instructions now platform-specific and accurate

### Removed
- `docs/staff-notes/` (internal development notes)
- `.internal_ignore` (housekeeping file)
- Redundant documentation files
- Incomplete security documentation

---

## [0.1.0] - 2024-12-11

### Initial C-Based Release

#### Added
- **Complete Lexer** (src/lexer/scanner.c)
  - Tokenization of all language constructs
  - 45 keywords support
  - 42 operators support
  - String and number parsing
  - Position tracking (line, column)

- **Complete Parser** (src/parser/parser.c)
  - Recursive descent parsing
  - All statement types
  - All expression types
  - Operator precedence
  - Error recovery

- **Type Checker** (src/parser/type_checker.c)
  - Type inference algorithm
  - Scope management
  - Symbol resolution
  - Type compatibility checking

- **Bytecode Compiler** (src/runtime/compiler.c)
  - AST to bytecode translation
  - 42+ bytecode instructions
  - Constant pooling
  - Jump instruction handling
  - Optimization passes (constant folding, dead code elimination)

- **Stack-Based Virtual Machine** (src/runtime/vm.c)
  - Bytecode execution engine
  - Function call handling
  - Native function dispatch
  - Error handling
  - REPL support

- **Runtime System**
  - Value type system (value.c)
  - Object system (object.c)
  - Memory management (memory.c)
  - Debug utilities (debug.c)
  - String interning

- **Standard Library** (44 functions)
  - **I/O**: print, input, open, read, write (5 functions)
  - **Math**: sqrt, sin, cos, tan, floor, ceil, abs, min, max, pow, log, exp, etc. (15 functions)
  - **String**: len, substring, toUpper, toLower, split, trim, contains, etc. (9 functions)
  - **Conversion**: toInt, toFloat, toString, toBool, typeof, etc. (8 functions)
  - **System**: exit, system, getEnv, setEnv, etc. (7 functions)

- **Language Features**
  - Variables (let, const)
  - Functions (parameters, return, closures)
  - Classes and objects
  - Control flow (if/else, while, for, switch)
  - Error handling (try/catch/finally)
  - Collections (lists, dictionaries, sets)
  - First-class functions
  - Type annotations (optional)

- **Build System**
  - CMake (3.10+) support
  - Makefile for Unix/Linux/macOS
  - Cross-compiler support (GCC, Clang, MSVC)
  - Debug and Release builds

- **Documentation**
  - Language specification
  - Project architecture overview
  - Example programs

- **Example Programs**
  - hello.prox
  - fibonacci.prox
  - calculator.prox
  - classes_demo.prox
  - stdlib_usage.prox

---

## Development Roadmap

### Phase 2: Optimization (Q1 2025)
- Constant propagation
- Dead code elimination
- Tail call optimization
- Jump optimization
- Peephole optimization
- Performance profiler

### Phase 3: Advanced Features (Q2 2025)
- Async/await implementation
- Pattern matching
- Generic types
- Module system (Initial support in v0.5.0)
- Debugger integration

### Phase 4: Ecosystem (Q3 2025)
- Package manager (ProXPL PM)
- Standard library expansion
- IDE plugins (VS Code, Sublime)
- Language Server Protocol (LSP)

### Phase 5: Production (Q4 2025)
- Comprehensive error messages
- Garbage collector (advanced)
- Memory profiler
- Security audit
- Performance benchmarks
- Stable API

### Phase 6: Future (2026+)
- JIT compiler
- WebAssembly target
- Native code generation
- Cross-platform optimization
- Advanced runtime features

---

## Known Issues

### Current Limitations

1. **Type Checker**: Partial implementation, needs refinement for complex types
2. **Object System**: Basic inheritance, needs method binding optimization
3. **Memory Management**: Mark-and-sweep GC not yet implemented
4. **Module System**: Partial implementation (`use` keyword support added, std lib in progress)
5. **Standard Library**: 44/75 functions (58% complete)
6. **Error Messages**: Could be more detailed with suggestions
7. **REPL**: Basic implementation, no history or autocompletion

### Workarounds

- For GC: Short-lived programs work fine with basic allocation
- For modules: Use the new `use` keyword or compile with single-file strategy
- For missing functions: Implement in ProXPL or use system() calls

---

## Migration Guide

### From Python Implementation (if existed)

The C implementation aims to be **100% compatible** with any previous Python version. If migrating code:

1. Syntax is identical
2. All core features supported
3. Standard library functions have same signatures
4. Error behavior is consistent

---

## Performance Notes

### Benchmarks (Development Build)

| Operation | Time | Notes |
|-----------|------|-------|
| Startup | ~5-10ms | Cold start |
| Compilation | Fast | Bytecode compilation |
| Fibonacci(20) | <100ms | Recursive |
| String operations | Native C speed | Highly optimized |

### Optimization Targets (v1.0)

- Startup time: <5ms
- Compilation: <100ms for typical files
- Runtime: Native C performance baseline

---

## Security

### Current Status
- Basic input validation
- No buffer overflows (C-managed)
- String bounds checking
- Type safety through type checker

### Audit Status
- Self-reviewed (developer team)
- No external audit yet
- Planned for v1.0 release

### Recommendations
- Run with memory sanitizers in development
- Use ASLR and DEP (OS-level)
- Validate user input scripts

---

## Contributors

ProXPL is developed by the ProXPL development team.

### Acknowledgments
- Inspired by Python, JavaScript, Go, Rust, Lua
- Based on compiler design principles from "Crafting Interpreters"
- Thanks to the open-source community

---

## License

This project is licensed under the MIT License.

---

## How to Report Issues

### Bugs
1. Check existing issues
2. Provide minimal reproducible example
3. Include OS, compiler version
4. Describe expected vs actual behavior

### Features
1. Describe use case
2. Explain why it's needed
3. Provide implementation hints (if possible)
4. Link related issues/discussions

---

## Version Numbering Scheme

**ProXPL** uses semantic versioning: `MAJOR.MINOR.PATCH`

- **MAJOR**: Breaking changes
- **MINOR**: New features (backward compatible)
- **PATCH**: Bug fixes
- **PRE-RELEASE**: 0.x.x indicates development

---

**Current Version**: 0.8.0
**Last Updated**: January 04, 2026
**Next Release**: 1.1.0 (Q1 2026)
