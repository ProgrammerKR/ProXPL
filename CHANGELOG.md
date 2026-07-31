# ProXPL Changelog

All notable changes to the ProXPL programming language will be documented in this file.

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
