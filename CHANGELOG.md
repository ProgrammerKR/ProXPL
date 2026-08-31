# ProXPL Changelog

All notable changes to the ProXPL programming language will be documented in this file.

## [1.6.4] - 2027-07-20
### Added
- **Channels & Structured Concurrency**: First-class asynchronous communication channels with `task.group`, `task.spawn`, and cooperative cancellation.
- **Actor Model**: Lightweight message-passing actor concurrency model with `actor`, `receive`, `!`, and `?` operators.
- **Database Connectivity (`std.db`)**: Universal database driver interface with connection pooling and support for SQLite, PostgreSQL, MySQL, and Redis.
- **Serialization (`std.encoding`)**: Standardized serialization modules for JSON, TOML, CSV, and Base64.
- **LSP v2**: Semantic highlighting, find references, rename symbol, inlay hints, and code actions in the Language Server.
- **Compile-Time Evaluation (`comptime`)**: Compile-time constant expressions and compile-time function execution.

### Changed
- Project version bumped to 1.6.4 ("Concurrency Core") across all build configurations and manifests.
- Keywords `actor`, `receive`, and `comptime` are now reserved.

## [1.6.3] - 2027-04-15
### Added
- **WebAssembly Target**: Full WebAssembly (.wasm) compiler backend with JS glue code generation and `prm serve --wasm`.
- **Standard Web Libraries**: Added `std.web.dom`, `std.web.event`, `std.web.http`, and `std.web.canvas`.
- **String Templates**: Backtick-delimited template literals (`` `Hello, ${name}!` ``) with expression interpolation, multiline templates, and `\${` escaping.
- **Operator Overloading**: Support for `+ - * / % @ == != < > <= >= [] []= () - !` methods in user classes with inheritance.
- **Closures v2 & Escape Analysis**: Detection of non-escaping closures for zero-heap stack execution, upvalue deduplication, and loop variable capture safety.
- **Code Formatter (`prox fmt`)**: Official syntax-aware code formatter supporting `.proxfmt.pxcf` configurations and `--check` mode.
- **Enhanced Diagnostics**: Structured error codes (`E0412`, `E0308`), caret pointers, snippets, and Levenshtein typo suggestions (`Did you mean 'name'?`).

### Changed
- Backtick (`` ` ``) is now reserved for string template literals.
- `operator` is now a reserved keyword.
- Project version bumped to 1.6.3 ("Wasm World") across all build configurations and manifests.

## [1.6.2] - 2026-08-13
### Changed
- IR optimization and backend improvements to remove unnecessary object generation.
- Reduced language binary size.


## [1.6.0] - 2026-08-08
### Added
- **Intent-Oriented Programming**: Full bytecode generation and VM opcode dispatch for `intent` and `resolver` paradigms.
- **Context-Aware Polymorphism**: Static type validation and dynamic VM scoping for `context`, `layer`, and `activate` mechanisms.
- **Autonomic Self-Healing (ASR)**: Operational `resilient` exception handling mapped natively onto the VM's `setjmp`/`longjmp` infrastructure.
- Added extensive performance benchmarks for intents, context switches, and ASR recovery.
- Added comprehensive testing scripts for validation of these core pillars.

### Changed
- `activate` statement now requires a scoped block.
- `intent` declarations now require a trailing semicolon.
- Project version bumped to 1.6.0 across all relevant compiler configurations.
## [1.5.0] - 2026-07-31
### Added
- Added parsing for generic parameters and trait constraints in functions and classes.
- Implemented trait resolution in the Type Checker.
- The Type Checker now enforces that classes implement all required methods for the traits they adopt.
- The compiler now verifies that generic trait bounds reference valid, known traits.

## [1.4.0] - 2026-07-30
### Added
- Integration of `mimalloc` memory allocator for optimized runtime performance.
- SwissTable dictionary optimizations using `ctrl` bytes for linear probing.
- Zero-cost stack unwinding for Autonomous Self-Healing (ASR) resilient blocks using `ExceptionHandlerTable`.
- LLVM PassManager configured for O3 (Vectorization and Inlining) and Tail-Call Optimization (TCO).

### Changed
- Project versions bumped to 1.4.0 across CLI, extensions, CMake, and installers.

For older releases, see the `releases/` directory.
