# ProXPL Changelog

All notable changes to the ProXPL programming language will be documented in this file.

## [1.4.0] - 2026-07-30
### Added
- Integration of `mimalloc` memory allocator for optimized runtime performance.
- SwissTable dictionary optimizations using `ctrl` bytes for linear probing.
- Zero-cost stack unwinding for Autonomous Self-Healing (ASR) resilient blocks using `ExceptionHandlerTable`.
- LLVM PassManager configured for O3 (Vectorization and Inlining) and Tail-Call Optimization (TCO).

### Changed
- Project versions bumped to 1.4.0 across CLI, extensions, CMake, and installers.

For older releases, see the `releases/` directory.
