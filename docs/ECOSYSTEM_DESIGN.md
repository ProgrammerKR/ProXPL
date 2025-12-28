# ProXPL Ecosystem Design Specification

## 1. Executive Summary

This document defines the architecture for the next evolutionary phase of ProXPL. Moving beyond the core compiler and VM, this phase focuses on making the language "batteries-included," operationally robust, and developer-friendly. The design prioritizes **determinism**, **explicit error handling**, and **clean abstractions** over raw feature bloat.

---

## 2. Standard Library Architecture

The ProXPL Standard Library (`std`) serves as the foundation for all applications. It avoids the "kitchen sink" problem by providing high-quality, essential building blocks rather than niche implementations.

### 2.1 Directory Structure & Naming

The library assumes a hierarchical module system rooted at `std`.

```text
lib/
└── std/
    ├── core/           # Built-in types, basic memory, primitives
    │   ├── types.prox    # Int, Float, Bool extensions
    │   └── mem.prox      # Manual memory helpers (unsafe)
    ├── io/             # Input/Output streams
    │   ├── console.prox
    │   └── stream.prox
    ├── fs/             # Filesystem operations
    │   ├── path.prox
    │   └── file.prox
    ├── net/            # Networking (Sockets, HTTP client)
    ├── math/           # Advanced math & Complex numbers
    ├── time/           # High-precision clocks & Date
    ├── sys/            # OS interactions (Env, Exit, PID)
    ├── collection/     # Advanced data structures (Queue, Stack, BST)
    └── text/           # String manipulation, Regex, Encodings
```

**Naming Conventions:**
- **Modules**: `snake_case` (e.g., `std.http_client`)
- **Functions/Methods**: `camelCase` (e.g., `readFile`, `toString`)
- **Types/Classes**: `PascalCase` (e.g., `FileStream`, `TcpSocket`)
- **Constants**: `SCREAMING_SNAKE_CASE` (e.g., `MAX_BUFFER_SIZE`)

### 2.2 Import System Semantics

ProXPL uses a path-based import system with support for aliasing.

- **Absolute Imports**: `import std.io;` resolves to `lib/std/io.prox` (or native equivalent).
- **Relative Imports**: `import .utils;` resolves to `./utils.prox` relative to current file.
- **Aliasing**: `import std.collection as coll;`
- **Selective Import**: `from std.math import (sin, cos, PI);`

**Resolution Logic**:
1.  Is it a core module (e.g., `std`)? -> Load from internal registry/stdlib path.
2.  Is it a local path (starts with `.`)? -> Load relative to current file.
3.  Is it a package (e.g., `import requests`)? -> Look in `prox_modules/` or global cache.

### 2.3 Core Modules & APIs

#### `std.core`
Essential runtime utilities. Auto-imported in most contexts.
```prox
// Assertions with clear messages
func assert(condition, message)

// Type checking
func typeOf(value) -> String

// Memory safety (Panic if null dereference happens)
func unwrap(optionalValue) -> Value
```

#### `std.io` & `std.fs`
Explicit separation between stream I/O and Filesystem manipulations.
```prox
// std.fs
module std.fs {
  class File {
    static func open(path, mode) -> Result<File>
    func readAll() -> String
    func close()
  }
  
  func exists(path) -> Bool
}

// std.io
module std.io {
  func print(msg)      // Buffered STDOUT
  func eprint(msg)     // Unbuffered STDERR
  func scanln() -> String
}
```

#### `std.sys`
System-level operations.
```prox
module std.sys {
  const OS_NAME: String
  const ARCH: String
  
  func exit(code)
  func env(key) -> Option<String>
  func args() -> List<String>
}
```

### 2.4 Native vs Pure Implementation Split

| Module | Implementation | Rationale |
| :--- | :--- | :--- |
| `std.core` | **Intrinsic/Native** | Basic types require VM-level access. |
| `std.math` | **Native (C)** | Performance critical; maps to `<math.h>`. |
| `std.io` | **Native (C)** | Direct syscalls needed. |
| `std.fs` | **Native (C)** | Platform specific syscalls. |
| `std.net` | **Native (C)** | Platform specific sockets. |
| `std.string` | **Mixed** | Basic ops native; formatting/templating pure. |
| `std.collection` | **Pure ProXPL** | Built on top of core arrays/maps. |
| `std.test` | **Pure ProXPL** | Test runner logic doesn't need C. |

### 2.5 Design Principles
1.  **Zero Hidden Allocations**: APIs that return new objects should be obvious. Prefer explicitly passing generic buffers for I/O.
2.  **Result Types over Exceptions**: Most I/O and System calls should return a `Result<T, E>` or `Option<T>` equivalent to force error handling (mimicking Rust/Go patterns).
3.  **Immutable Default**: Strings are immutable. Collections passed by reference but should have clear `clone()` methods.

---

## 3. ProX Resource Manager (PRM)

PRM is the official CLI toolchain for ProXPL, handling dependency management, building, testing, and publishing.

### 3.1 Package Format (`prox.toml`)
Every project is defined by a TOML manifest.

```toml
[package]
name = "my-web-server"
version = "1.0.0"
authors = ["Jane Doe <jane@example.com>"]
edition = "2025"
description = "A fast web server for ProXPL"
license = "MIT"

[dependencies]
std = "0.1.0"  # Core stdlib is versioned too!
http_parser = { git = "https://github.com/...", tag = "v1.2" }
json = "2.1.0"

[build]
target = "native" # or "bytecode"
optimize = true
```

### 3.2 Command Suite

| Command | Description |
| :--- | :--- |
| `prm init <name>` | Scaffolds a new project with `prox.toml` and `src/main.prox`. |
| `prm build` | Compiles the project. Supports `--dev` (fast, debug info) and `--release` (opt, stripped). |
| `prm run` | Builds and runs the binary (or interprets in dev mode). |
| `prm test` | Discovers and runs tests in `tests/` or files ending in `_test.prox`. |
| `prm add <pkg>` | Adds a dependency to `prox.toml` and updates lockfile. |
| `prm update` | Updates dependencies within semantic versioning bounds. |
| `prm install` | Downloads dependencies defined in `prox.lock`. |

### 3.3 Dependency Resolution & Cache
- **Lockfile (`prox.lock`)**: Pin exact versions of entire dependency tree. Checked into git.
- **Global Cache**: `~/.prox/cache/registry` stores downloaded packages to avoid re-downloading.
- **Structure**:
    - `~/.prox/bin/`: Installed global binaries.
    - `~/.prox/registry/`: Source code of packages.

---

## 4. Runtime Services & Tooling

To be production-ready, the runtime needs observability and robust error reporting.

### 4.1 Error System
ProXPL moves away from generic "Runtime Error" to structured errors.

**Proposed Error Structure (C-Side):**
```c
typedef struct {
    ErrorType type;       // FATAL, LOGIC, IO, FORMAT
    const char* message;
    const char* file;
    int line;
    CallFrame* stackTrace; // Captured at error creation
} RuntimeError;
```
**User-Facing Panic:**
When a panic occurs, the VM prints:
1.  Error Type & Message.
2.  Formatted Stack Trace (File:Line Function).
3.  Hint (if available).

### 4.2 Logging & Diagnostics
- **`std.log`**: A structured logging interface.
    - Levels: `DEBUG`, `INFO`, `WARN`, `ERROR`, `FATAL`.
    - Output: Defaults to stderr, but configurable to file or JSON stream.
- **Trace Hooks**: The VM will expose hooks for function entry/exit, allowing for flamegraph generation and profiling in the future.

### 4.3 Testing Framework (`std.test`)
Built-in support for unit testing without external dependencies.
```prox
import std.test;

test "addition works" {
    assert(1 + 1 == 2, "Math is broken");
}

test "failure case" {
    // Expect panic
    test.should_panic(func() { 1 / 0 });
}
```
**`prm test`** runner will:
1.  Recursively find test files.
2.  Execute them in a sandbox.
3.  Report Pass/Fail statistics and timing.

---

## 5. Ecosystem Roadmap

### Phase 1: Foundation (Weeks 1-4)
*Goal: Minimum usable Standard Library and Build System.*
- [ ] **Stdlib v0.1**: Implement `std.core`, `std.io`, `std.fs`, `std.sys` (Native bindings).
- [ ] **PRM v0.1**: Basic `prox.toml` parsing and `prm build` (local files only).
- [ ] **Docs**: Initial `stdlib` API reference.

### Phase 2: Package Management (Weeks 5-8)
*Goal: Allow sharing code.*
- [ ] **PRM v0.5**: Git dependency support. `prm install` & `prox.lock`.
- [ ] **Stdlib v0.2**: Add `std.net` (basic sockets) and `std.json`.

### Phase 3: Tooling & Stability (Weeks 9-12)
*Goal: Developer Experience.*
- [ ] **Testing**: Implement `std.test` and `prm test` harness.
- [ ] **LSP**: Language Server Protocol basic implementation (Jump to def, simple completion).
- [ ] **Debugger**: Source-map support for LLVM backend debugging.

### Phase 4: Expansion (4 Months+)
*Goal: Enterprise Readiness.*
- [ ] **Registry**: Centralized package registry (web service).
- [ ] **Concurrency**: Go-style coroutines or Actor model in `std.task`.
- [ ] **Sandboxing**: Wasm target or capabilities-based security.

---

## 6. Language Ergonomics (Developer Experience)

- **Formatting**: `prox fmt` is the single source of truth. Style is K&R braces, 4-space indent (or tab, configurable but standard is spaces).
- **Documentation**: Markdown support in doc-comments `///`. `prox doc` generates HTML.
- **Visibility**:
    - `pub func` explicitly exports symbols.
    - Default visibility is module-private.
- **FFI**: Simple C-interop.
    ```prox
    foreign func MessageBoxA(hwnd, text, caption, type);
    ```

---

## 7. Trade-offs & Rejected Alternatives

1.  **Running `npm` / `pip` for packages**:
    - *Rejected*: ProXPL needs deterministic, domain-specific build logic (AOT vs JIT switch). Relying on Python/Node logic introduces bloat and dependency hell.
2.  **Classes for everything (Java style)**:
    - *Rejected*: `std.core` uses functions acting on data for primitives suitable for SSA optimization. Classes reserved for complex state (Files, Sockets).
3.  **Exceptions**:
    - *Rejected*: Silent control flow makes systems programming hard. Result/Option types are explicit and force handling.
