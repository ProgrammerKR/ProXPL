# ProXPL v1.0.0 Alpha Release Notes

**Build Status**: Passing (Windows/Linux/macOS)
**Version**: 1.0.0 (Alpha)
**Date**: January 2026

## 🚀 Major Features: Object-Oriented Programming

ProXPL v1.0.0 introduces fundamental support for Class-based Object-Oriented Programming, marking a significant milestone in the language's evolution.

### Core OOP Concepts
- **Classes**: Define blueprints for objects using the `class` keyword.
- **Instantiation**: Create objects using the `new` keyword (e.g., `let obj = new MyClass()`).
- **Methods**: Define behavior attached to classes.
- **Properties**: runtime-dynamic property access on instances.
- **Inheritance**: Single inheritance supported via `class Dog extends Animal`.
- **`this` Keyword**: Explicit access to the current instance within methods.

### New Keywords
- `class`
- `new`
- `this`
- `extends`
- `interface` (Parser support only, runtime behavior in progress)
- `static` (Parser support only)
- `pub` / `priv` (Parser support only)

## 🛠️ Runtime & VM Enhancements
- **Optimized Object Model**: New `ObjClass`, `ObjInstance`, and `ObjBoundMethod` runtime structures.
- **New OpCodes**:
  - `OP_CLASS`, `OP_INHERIT`, `OP_METHOD` for class definitions.
  - `OP_GET_PROPERTY`, `OP_SET_PROPERTY` for field access.
  - `OP_INVOKE` for optimized method calls.
- **Stack-based Instantiation**: Efficient object creation and initialization.

## 📦 Standard Library Updates (from v0.9.0)
- **`std.gc`**: Manual garbage collection control and stats (`gc.collect()`, `gc.stats()`).
- **`std.fs`**: Added `move()`, `abspath()`.
- **`std.time`**: Added `strftime()`, `timestamp()`.

## 🐛 Bug Fixes
- Fixed compiler state management during class recompilation.
- Resolved build system issues with CMake across different platforms.
- improved memory safety in `ObjInstance` property access.

## 🔜 Next Steps (v1.1.0)
- Access Control enforcement (`pub`/`priv`).
- `try`/`catch` exception handling.
- `init` constructors.
- Expanded standard library for file manipulation.

---

*Thank you to all contributors who made this release possible!*
