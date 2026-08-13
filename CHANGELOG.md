# ProXPL Changelog

All notable changes to the ProXPL programming language will be documented in this file.

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
