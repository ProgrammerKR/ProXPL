# Python to C Migration Blueprint

## Overview

This document outlines the complete strategy for migrating ProXPL from Python (reference implementation) to C (production implementation). This is already partially complete, and this blueprint provides a roadmap for finishing the migration and maintaining code quality.

## Current Status

### ✅ Completed (C Implementation)
- Core Lexer (Scanner)
- Core Parser
- Basic AST implementation
- Type Checker (partial)
- Bytecode Compiler
- Stack-based VM
- Runtime Value System
- Object System (partial)
- Memory Management (basic)
- Standard Library (partial - 44 functions)

### 🔄 In Progress
- Type Checker completion
- Object system refinement
- Standard library expansion
- Error handling system
- Module/Import system

### ⏳ Not Started (Future)
- Garbage Collector (advanced)
- Module system
- Package manager
- Debugger/Profiler
- Optimization passes
- WebAssembly target

---

## Migration Strategy

### Phase 1: Complete Core Language (Current)

**Target**: Fully functional language with all basic features

#### 1.1 Lexer (✅ Complete)
- File: `src/lexer/scanner.c`
- All 45 keywords recognized
- All 42 operators tokenized
- String/number parsing complete
- Position tracking for errors

**Status**: Production-ready

#### 1.2 Parser (✅ Complete)
- File: `src/parser/parser.c`
- All statement types parsed
- All expression types parsed
- Recursive descent implementation
- Error recovery

**Status**: Production-ready

#### 1.3 Type Checker (🔄 In Progress)
- File: `src/parser/type_checker.c`
- Type inference algorithm
- Type unification
- Scope management
- Error detection

**TODO**:
```
[ ] Complete inference rules for all expressions
[ ] Implement function type checking
[ ] Add class/interface checking
[ ] Add generic type support (future)
[ ] Better error messages
```

#### 1.4 Bytecode Compiler (✅ Complete)
- File: `src/runtime/compiler.c`
- All opcodes implemented (42+ instructions)
- Constant pooling
- Jump instruction handling
- Function compilation

**Status**: Production-ready

#### 1.5 Virtual Machine (✅ Complete)
- File: `src/runtime/vm.c`
- Stack-based execution
- Call stack management
- Native function dispatch
- Error handling

**Status**: Production-ready with improvements needed

**TODO**:
```
[ ] Better error messages at runtime
[ ] Stack trace on error
[ ] REPL improvements
[ ] Debugger integration points
```

---

### Phase 2: Runtime System & Memory (Next Priority)

#### 2.1 Value System (✅ Complete)
- File: `src/runtime/value.c`
- All 12 data types represented
- Type checking macros
- String interning

**Status**: Production-ready

#### 2.2 Object System (🔄 In Progress)
- File: `src/runtime/object.c`
- String objects
- Function objects
- Class objects
- Instance objects

**TODO**:
```
[ ] Method binding
[ ] Inheritance implementation
[ ] Static methods
[ ] Property access optimization
[ ] Garbage collection integration
```

#### 2.3 Memory Management (🔄 Partial)
- File: `src/runtime/memory.c`
- Basic allocation
- Deallocation

**TODO**:
```
[ ] Implement mark-and-sweep GC
[ ] String pool optimization
[ ] Memory profiling
[ ] Leak detection
```

#### 2.4 Collections (⏳ Not Started)
- Lists, Dictionaries, Sets

**Plan**:
```c
// src/runtime/collections.c
typedef struct {
    Value *elements;
    int count;
    int capacity;
} ObjList;

typedef struct {
    // Key-value pairs
    struct { Value key; Value value; } *entries;
    int count;
    int capacity;
} ObjDict;

typedef struct {
    // Unique values using hashtable
    Table table;
} ObjSet;
```

---

### Phase 3: Standard Library Expansion

#### Current Coverage (44 functions)

**I/O** (5 functions):
- print(), input(), open(), read(), write()

**Math** (15 functions):
- sqrt(), sin(), cos(), tan(), floor(), ceil(), abs(), min(), max(), pow(), etc.

**String** (9 functions):
- len(), substring(), toUpper(), toLower(), split(), trim(), contains(), etc.

**Convert** (8 functions):
- toInt(), toFloat(), toString(), toBool(), typeof(), etc.

**System** (7 functions):
- exit(), system(), getEnv(), setEnv(), etc.

#### Planned Additions (30+ functions)

**Collections** (12 functions):
```c
// map, filter, reduce, sort, reverse
// append, pop, push, insert, remove, find
```

**DateTime** (8 functions):
```c
// now, parse, format, sleep
// year, month, day, weekday
```

**File I/O** (6 functions):
```c
// exists, delete, rename, mkdir, listdir, getcwd
```

**Advanced** (6+ functions):
```c
// assert, hash, clone, freeze
// Custom error types
```

#### Implementation Plan

Each function follows pattern:

```c
// stdlib/category_native.c
Value category_function_name(int argc, Value *argv) {
    // Validate argument count and types
    if (argc != EXPECTED_COUNT) {
        return ERROR_VAL("Wrong number of arguments");
    }
    
    // Type checking
    if (!IS_NUMBER(argv[0])) {
        return ERROR_VAL("Expected number");
    }
    
    // Implementation
    double result = some_operation(AS_NUMBER(argv[0]));
    
    // Return result
    return NUMBER_VAL(result);
}

// Registration in stdlib_core.c
void register_category_functions(VM *vm) {
    define_native(vm, "function_name", category_function_name);
    // ... more functions
}
```

---

### Phase 4: Module System

#### Module Loading

```c
// src/runtime/importer.c
typedef struct {
    Table loaded_modules;  // Cache of loaded modules
    char search_paths[10][256];
    int path_count;
} Importer;

Value import_module(VM *vm, const char *name) {
    // 1. Check cache
    // 2. Search paths
    // 3. Parse and compile
    // 4. Execute
    // 5. Return exports
}
```

#### Usage
```javascript
use math;
use collections as col;
from io import print, input;
```

---

### Phase 5: Advanced Features

#### 5.1 Garbage Collector

**Algorithm**: Mark-and-sweep

```c
// src/runtime/gc.c
void collect_garbage(VM *vm) {
    // 1. Mark roots
    //    - Global variables
    //    - Stack values
    //    - Function closures
    
    // 2. Mark reachable objects
    //    - Follow pointers
    //    - Recursive traversal
    
    // 3. Sweep unmarked objects
    //    - Free memory
    //    - Update linked list
    
    // 4. Reset GC threshold
    vm->next_gc *= GC_GROWTH_FACTOR;
}
```

#### 5.2 Optimization Passes

**Constant Folding**:
```
5 + 3 → 8  (compile time)
```

**Dead Code Elimination**:
```
if (false) { unreachable code } → removed
```

**Jump Optimization**:
```
JUMP → forward → jump to target
```

**Peephole Optimization**:
```
Multiple instructions → single instruction
```

#### 5.3 Debugger

```c
// src/tools/debugger.c
typedef struct {
    bool step_mode;
    Table breakpoints;
    Value *watch_vars;
} Debugger;

void debug_hook(VM *vm, int opcode) {
    if (vm->debug) {
        // Check breakpoints
        // Display state
        // Read commands
    }
}
```

---

## File Structure Migration

### Python → C Mapping

```
Python                          C
────────────────────────────────────────────
cli/                            src/main.c (REPL + file execution)
  main.py

lexer/                          src/lexer/
  scanner.py                      scanner.c
                                  include/scanner.h

parser/                         src/parser/
  parser.py                       parser.c
                                  include/parser.h
  ast.py                          ast.c
                                  include/ast.h
  type_checker.py                 type_checker.c
                                  include/type_checker.h

compiler/                       src/runtime/
  compiler.py                     compiler.c
                                  include/compiler.h
  chunk.py                        chunk.c
                                  include/chunk.h

vm/                             src/runtime/
  vm.py                           vm.c
                                  include/vm.h
  value.py                        value.c
                                  include/value.h
  object.py                       object.c
                                  include/object.h

stdlib/                         src/stdlib/
  __init__.py                     stdlib_core.c
  io.py                           io_native.c
  math.py                         math_native.c
  string.py                       string_native.c
  convert.py                      convert_native.c
  system.py                       system_native.c

memory/                         src/runtime/
  memory.py                       memory.c
                                  include/memory.h
                                  gc.c (future)
```

---

## Build System

### Current CMake Setup

```cmake
# CMakeLists.txt (root)
cmake_minimum_required(VERSION 3.10)
project(ProXPL C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.c")
add_executable(prox ${SOURCES})
```

### Make Alternative

```makefile
# Makefile
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -std=c99
SOURCES = $(wildcard src/**/*.c) src/main.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = prox

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

test:
	# Run test suite
```

---

## Testing Strategy

### Unit Tests

```c
// tests/unit/test_lexer.c
#include "unity.h"
#include "scanner.h"

void test_tokenize_number() {
    Scanner scanner;
    init_scanner(&scanner, "42");
    Token token = next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_NUMBER, token.type);
    TEST_ASSERT_EQUAL(2, token.length);
}

void setUp(void) {
    // Setup before each test
}

void tearDown(void) {
    // Cleanup after each test
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_tokenize_number);
    // ... more tests
    return UNITY_END();
}
```

### Integration Tests

```c
// tests/integration/test_e2e.c
void test_fibonacci_program() {
    const char *source = R"(
        func fib(n) {
            if (n <= 1) return n;
            return fib(n-1) + fib(n-2);
        }
        print(fib(10));
    )";
    
    // Compile and run
    // Assert output matches expected
}
```

### Test Coverage

**Target**: 80% code coverage

```bash
# Generate coverage report
gcov src/**/*.c
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
```

---

## Performance Targets

### Benchmarks to Achieve

| Operation | Target | Method |
|-----------|--------|--------|
| Fibonacci(30) | < 1s | Time compiled code |
| Startup time | < 10ms | Time executable startup |
| Memory (simple program) | < 1MB | Measure heap size |
| Parsing large file | < 100ms | Parse 10K line file |
| Function call overhead | < 10us | Time 1M calls |

### Optimization Techniques

1. **Inline Functions**: Hot path functions
2. **String Interning**: Memory efficiency
3. **Bytecode Caching**: Avoid recompilation
4. **Jump Optimization**: Reduce instruction count
5. **Stack Allocation**: For temporary objects
6. **Object Pooling**: Reuse allocations

---

## Release Planning

### Version 0.9 (Current - December 2024)
- [x] Lexer, parser, compiler
- [x] Basic VM
- [x] Core stdlib (40+ functions)
- [ ] Complete type checker
- [ ] Module system (basic)
- [ ] Comprehensive tests

### Version 1.0 (Q2 2025)
- [ ] Feature-complete language
- [ ] 80%+ test coverage
- [ ] Production-ready documentation
- [ ] Performance optimizations
- [ ] Full standard library (75+ functions)
- [ ] Error recovery
- [ ] REPL improvements

### Version 1.1 (Q3 2025)
- [ ] Garbage collector
- [ ] Module system (complete)
- [ ] Package manager (ProXPL PM)
- [ ] Debugger
- [ ] IDE plugins (VS Code)

### Version 2.0 (2026)
- [ ] JIT compiler
- [ ] Native code generation
- [ ] WebAssembly target
- [ ] Advanced optimizations

---

## Code Quality Checklist

- [ ] No compiler warnings (with -Wall -Wextra)
- [ ] All functions documented (Doxygen style)
- [ ] Memory leaks checked (valgrind/sanitizers)
- [ ] All tests passing (100% pass rate)
- [ ] Code review completed
- [ ] Performance benchmarks met
- [ ] Security audit passed
- [ ] Documentation up to date

---

## Migration Metrics

**Progress Tracking**:
```
Lexer:           100% (✅ Complete)
Parser:          100% (✅ Complete)
Type Checker:    75% (🔄 In Progress)
Compiler:        100% (✅ Complete)
VM:              95% (✅ Nearly Complete)
Runtime:         70% (🔄 In Progress)
StdLib:          58% (🔄 In Progress, 44/75 functions)
Memory/GC:       20% (⏳ Not Started)
Module System:   10% (⏳ Not Started)
Debugger:        0% (⏳ Future)
────────────────────────────────
Overall:         ~62% Complete
```

---

## Development Priorities (Next 3 Months)

1. **Complete Type Checker** (2 weeks)
   - All expression types
   - Function signatures
   - Class definitions
   - Better error messages

2. **Finish Object System** (2 weeks)
   - Method binding
   - Inheritance
   - Property access

3. **Expand Standard Library** (3 weeks)
   - Collections (lists, dicts, sets)
   - File I/O
   - DateTime

4. **Implement Module System** (3 weeks)
   - Module loading
   - Circular dependency detection
   - Module caching

5. **Testing & Documentation** (Ongoing)
   - Unit tests for all components
   - Integration tests
   - API documentation
   - Tutorial updates

---

## Resources & References

### Language Implementation
- "Crafting Interpreters" by Robert Nystrom
- "Engineering a Compiler" by Cooper & Torczon
- LUA source code (reference implementation)

### C Programming
- C99 Standard
- "The C Programming Language" by Kernighan & Ritchie
- Linux kernel coding style

### Performance
- "Software Optimization" by Agner Fog
- Profiling with perf, gprof, valgrind

---

This migration blueprint provides a clear roadmap for completing ProXPL's transition to C and achieving production-ready quality.
