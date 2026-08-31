# ProXPL Documentation

Welcome to the ProXPL documentation. This directory contains comprehensive guides and references for the ProXPL programming language.

## Documentation Structure

```
docs/
├── README.md                  # This file - documentation hub
├── getting-started/           # Quick start guides
├── language/                 # Language specification and guides
├── standard-library/          # Standard library reference
├── tooling/                  # CLI, LSP, VS Code integration
├── packages/                 # PRM package manager
├── compiler/                 # Compiler architecture
├── runtime/                  # VM, GC, execution
├── tutorials/                # Step-by-step tutorials
├── reference/                # Quick reference guides
├── contributing/             # Developer guides
├── design/                   # Design documents and proposals
├── roadmap/                  # Future plans
├── releases/                 # Release notes
└── security/                 # Security documentation
```

## Quick Navigation

### For New Users
- [Getting Started](getting-started/README.md) - Installation and first program
- [Language Guide](language/README.md) - Core language concepts
- [Tutorials](tutorials/README.md) - Step-by-step learning

### For Developers
- [Compiler Architecture](compiler/README.md) - How ProXPL compiles code
- [Runtime System](runtime/README.md) - VM and memory management
- [Standard Library](standard-library/README.md) - Built-in functions

### For Contributors
- [Contributing Guide](contributing/README.md) - How to contribute
- [Code Standards](contributing/STANDARDS.md) - Coding guidelines
- [Testing Guide](contributing/TESTING.md) - Writing tests

### Reference
- [Language Reference](language/REFERENCE.md) - Complete language specification
- [Standard Library Reference](standard-library/REFERENCE.md) - Function documentation
- [Bytecode Reference](compiler/BYTECODE.md) - VM instruction set

## Feature Status

| Feature | Status | Notes |
|---------|--------|-------|
| Core Language | **Stable** | Variables, functions, control flow |
| OOP | **Stable** | Classes, inheritance, methods |
| Closures | **Stable** | Upvalues, escape analysis |
| Intent/Context | **Stable** | IOP and COP paradigms |
| ASR (Self-Healing) | **Stable** | `resilient`/`recovery` blocks |
| Type System | **Stable** | Static typing with inference |
| GC | **Stable** | Mark-and-sweep |
| FFI | **Stable** | C interop via `extern` |
| Tensor/Matrix | **Stable** | `@` operator for matrix math |
| Templates | **Stable** | Backtick string interpolation |
| Operator Overloading | **Stable** | Custom operators |
| Formatter | **Stable** | `prox fmt` |
| WebAssembly | **Stable** | Browser and edge deployment |
| Taint Analysis | **Partial** | Static tracking only |
| AI/Tensor Training | **Stub** | Tensors work, training not implemented |
| Chrono-Native | **Frontend** | Parsed but no runtime support |
| Quantum/GPU | **Stub** | Reserved keywords only |

## Key Resources

| Resource | Description |
|----------|-------------|
| [Implementation Status](../IMPLEMENTATION_STATUS.md) | Ground-truth feature audit |
| [Build Guide](BUILD_GUIDE.md) | Platform-specific build instructions |
| [Release Notes](releases/) | Version history and changes |
| [Security Model](security/SECURITY.md) | Security architecture |

## Documentation Index

For detailed navigation, see:
- [Language Documentation](language/)
- [Standard Library](standard-library/)
- [Compiler Internals](compiler/)
- [Runtime Internals](runtime/)
- [Tooling](tooling/)
- [Package Manager](packages/)

---

*Last Updated: August 2026*
*ProXPL v1.6.4 - "Concurrency Core"*
