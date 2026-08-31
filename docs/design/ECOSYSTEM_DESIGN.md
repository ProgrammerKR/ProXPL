# Ecosystem Design

Standard Library and PRM (ProX Repository Manager) architecture.

## Standard Library Architecture

### Module Structure

```text
lib/std/
├── core/           # Built-in types, memory primitives
├── io/             # Input/Output streams
├── fs/             # Filesystem operations
├── net/            # Networking
├── math/           # Advanced math
├── time/           # High-precision clocks
├── sys/            # OS interactions
├── collection/     # Advanced data structures
└── text/           # String manipulation, regex
```

### Naming Conventions

- **Modules**: `snake_case` (e.g., `std.http_client`)
- **Functions/Methods**: `camelCase` (e.g., `readFile`)
- **Types/Classes**: `PascalCase` (e.g., `FileStream`)
- **Constants**: `SCREAMING_SNAKE_CASE` (e.g., `MAX_BUFFER_SIZE`)

### Import System

```proxpl
import std.io;                    // Absolute import
import .utils;                    // Relative import
import std.collection as coll;     // Aliased import
from std.math import (sin, cos);  // Selective import
```

### Native vs Pure Split

| Module | Implementation | Rationale |
|--------|---------------|-----------|
| `std.core` | Native | VM-level access |
| `std.math` | Native (C) | Performance |
| `std.io` | Native (C) | Direct syscalls |
| `std.fs` | Native (C) | Platform-specific |
| `std.string` | Mixed | Basic native, formatting pure |
| `std.collection` | Pure ProXPL | Built on core |
| `std.test` | Pure ProXPL | No C needed |

## PRM (ProX Resource Manager)

### Commands

| Command | Description |
|---------|-------------|
| `prm init <name>` | Scaffold new project |
| `prm build` | Compile project |
| `prm run` | Build and run |
| `prm test` | Run test suite |
| `prm add <pkg>` | Add dependency |
| `prm update` | Update dependencies |
| `prm remove <pkg>` | Remove dependency |

### Design Principles

1. **Determinism**: Lockfile for reproducible builds
2. **Explicit error handling**: Result types over exceptions
3. **Zero hidden allocations**: Obvious API behavior
4. **Immutable default**: Strings immutable, clear clone methods

---

**Status**: Stable (core design), Planned (ecosystem expansion)
