# ProXPL Changelog

All notable changes to the ProXPL programming language will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.5.2] - 2026-08-08

### Security
- **CRITICAL**: Fixed command injection vulnerability in `sys.exec`, `OS.exec`, and `system.exec` by adding `isSafeArg()` input validation to reject shell metacharacters (`;`, `|`, `&`, `$()`, `` ` ``, `&&`, `||`, `>`, `<`).
- **HIGH**: Fixed FFI library handle leak in `ffi_bridge.c` — `dlclose()` is now called when `dlsym()` fails during symbol lookup, preventing native library descriptor exhaustion.
- **HIGH**: Fixed FFI return type handling from `ffi_type_sint` (32-bit) to `ffi_type_pointer` (pointer-sized) to correctly support `double` and pointer return values from foreign functions.
- **MEDIUM**: Added `realloc` null check in `buffer_native.c` (`native_buf_write_byte`) to prevent null pointer dereference on memory allocation failure.

### Runtime / VM
- **CRITICAL**: Fixed exception handler stack underflow in `vm.c` — exception handler IP calculation now uses `frame->ip[-1]` instead of `frame->ip[-2]`, and the active context stack is properly popped before `DISPATCH()` to prevent stack corruption during exception unwinding.
- **CRITICAL**: Fixed stack underflow vulnerability in `OP_MAKE_TENSOR` bytecode handler — added bounds checks ensuring the stack has sufficient operands before tensor construction, preventing memory corruption from malformed bytecode.
- **HIGH**: Fixed traceback line lookup bounds check in `vm.c` — added validation that `lineIndex` is within `chunk->lines` array bounds before accessing, preventing out-of-bounds reads during runtime errors.
- **HIGH**: Replaced `exit(1)` calls in `vm.c` `push()`/`pop()` with recoverable error propagation — stack overflow/underflow now returns `INTERPRET_RUNTIME_ERROR` instead of crashing the process, enabling graceful error handling in production.
- **MEDIUM**: Fixed nursery memory accounting in `gc.c` — manual byte-by-byte copy loop in object promotion replaced with `memcpy()` for correctness and performance.
- **MEDIUM**: Added missing `#include <string.h>` in `gc.c` to ensure `memcpy()` and `memmove()` declarations are available.
- **LOW**: Fixed supervisor task registration NULL check in `supervisor.c` — `registerTask()` now validates the `task` pointer before creating child spec, preventing null dereference.

### Compiler / Type System
- **HIGH**: Connected the existing type checker to the main compilation pipeline — `interpretAST()` and `interpret()` now invoke `initTypeChecker()` / `checkTypes()` / `freeTypeChecker()` before bytecode generation, catching type mismatches at compile time rather than silently generating incorrect bytecode.
- **HIGH**: Added bytecode verification pass — new `src/vm/verifier.c` implements `verifyChunk()` which validates opcode operands, stack depth, jump targets, and constant indices before execution. This prevents crashes from malformed or malicious bytecode.
- **MEDIUM**: Integrated bytecode verifier into `interpret()` and `interpretAST()` — all compiled code is now verified before the VM executes it.

### Standard Library - Native C
- **HIGH**: Completed `JSON.parse()` implementation in `json_native.c` — replaced stub with full recursive descent parser supporting strings, numbers, booleans, null, arrays, and objects with proper escape handling.
- **HIGH**: Completed `JSON.stringify()` implementation in `json_native.c` — added proper JSON escaping for control characters, quotes, backslashes, and unicode, with recursive serialization of lists and dictionaries.
- **MEDIUM**: Added `Base64.decode()` implementation in `crypto.prox` — proper base64 decoding with padding support and invalid character handling.
- **MEDIUM**: Completed `Hex.decode()` implementation in `crypto.prox` — proper hex string decoding with validation.
- **MEDIUM**: Added `Base64._charFromCode()` helper in `crypto.prox` to correctly map byte values 0-63 to base64 characters.
- **MEDIUM**: Added `Crypto._charFromCode()` helper in `crypto.prox` for general character code to string conversion.
- **MEDIUM**: Added `net.http_get()` and `net.http_post()` stub implementations in `net_native.c` to match the API expected by `std/lib/net.prox`.
- **LOW**: Fixed `Queue.dequeue()` O(n) performance issue in `collections_native.c` — replaced full array copy with head-index tracking, making dequeue amortized O(1).
- **LOW**: Added `Collections.sort()` native implementation in `collections_native.c` using `qsort()` for O(n log n) sorting.
- **LOW**: Added `Collections.dictKeys()` native function in `collections_native.c` to extract dictionary keys as a list.
- **LOW**: Added native `charCode()` string function in `string_native.c` to return ASCII code of the first character.
- **LOW**: Fixed `Set.toList()` in `collections.prox` to return actual elements instead of keys.

### Standard Library - ProXPL Layer
- **HIGH**: Fixed infinite recursion in `std/lib/math.prox` — all math function wrappers now route to `native.math.*` instead of calling themselves recursively. Added `use std.native.math` import.
- **HIGH**: Fixed infinite recursion in `std/lib/str.prox` — all string function wrappers now route to `native.str.*` instead of calling themselves recursively. Added `use std.native.str` import.
- **MEDIUM**: Fixed duplicate `_hexValue` function definition in `std/lib/crypto.prox` — removed redundant implementation that overwrote the correct character-based hex parser.
- **MEDIUM**: Fixed `Base64.decode()` padding handling — properly skips `=` padding characters and adjusts output length.
- **LOW**: Added `Collections.slice()` utility function in `collections.prox` for extracting sublists.
- **LOW**: Fixed `StringUtils.trimStart()` and `trimEnd()` to use `native.str.substr()` with correct parameters.

### Documentation
- **HIGH**: Completely rewrote `SECURITY.md` from template stub to actual security policy with vulnerability reporting流程, supported versions, and security best practices.
- **MEDIUM**: Fixed README.md license badge from MIT to PPL (ProX Professional License).
- **MEDIUM**: Fixed README.md version badge from 1.5.0 to 1.5.1.
- **MEDIUM**: Fixed `CODE_OF_CONDUCT.md` contact email from placeholder to `conduct@proxentix.com`.
- **MEDIUM**: Fixed `CONTRIBUTING.md` license reference from MIT to PPL.
- **LOW**: Updated `Doxyfile` `PROJECT_NUMBER` from stale 1.1.0 to 1.5.1.
- **LOW**: Updated `docs/VERSIONING.md` current version from 1.2.0 to 1.5.1.
- **LOW**: Updated `setup.iss` version string from 1.5.0 to 1.5.1.
- **LOW**: Updated `proxconfig.pxcf` version and license fields to 1.5.1 / PPL.

### CI/CD
- **MEDIUM**: Updated `.github/dependabot.yml` with actual package ecosystems (`github-actions`, `npm`) instead of empty configuration.
- **MEDIUM**: Fixed `.github/workflows/release.yml` artifact download paths — split duplicate `proxlang-prod` artifact name into separate `proxlang-windows` and `proxlang-linux` names.
- **MEDIUM**: Fixed `.github/workflows/build.yml` artifact naming to use dynamic version from `git describe --tags` instead of hardcoded `v1.2.0`.
- **LOW**: Fixed `.github/workflows/codacy.yml` by setting `max-allowed-issues` to 50 (was disabled/empty).
- **LOW**: Fixed `.github/workflows/snyk-security.yml` by removing `|| true` which silently ignored scan failures, and added severity threshold of `high`.

### Infrastructure
- **LOW**: Updated `Makefile` with deprecation notice redirecting users to CMake build system.
- **LOW**: Updated `CMakeLists.txt` to include new `src/vm/verifier.c` in the library build.

## [1.5.1] - 2026-08-07

### Fixed
- Fixed exception handler stack underflow in `vm.c` runtime error path.
- Fixed stack underflow vulnerability in `OP_MAKE_TENSOR` bytecode handler.
- Fixed missing `string.h` include in `gc.c` (manual byte copy replaced with `memcpy`).
- Fixed FFI library handle leak in `ffi_bridge.c`.
- Fixed unchecked `realloc` return in `buffer_native.c`.
- Fixed `Queue.dequeue()` O(n) performance issue in `collections.prox`.
- Fixed README.md license badge (MIT -> PPL).
- Fixed version inconsistencies (1.5.0 -> 1.5.1 across 6 files).
- Fixed release.yml artifact download paths.
- Fixed build.yml artifact naming to use dynamic version.
- Fixed CODE_OF_CONDUCT.md contact email.

### Added
- Added `Collections.sort()` with native `qsort` backend.
- Added `Collections.dictKeys()` native function.
- Added native `charCode()` string function.
- Added security input validation to `sys.exec` and `OS.exec`.

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
