# Testing Guide

How to write and run tests for ProXPL.

## Running Tests

```bash
# Build with tests enabled
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON ..
make

# Run all tests
make test

# Run specific test
./build/tests/lexer_test
./build/tests/parser_test
./build/tests/vm_test
```

## Test Categories

| Category | Location | Description |
|----------|----------|-------------|
| Language | `tests/language/` | Core language feature tests |
| Integration | `tests/integration/` | E2E integration tests |
| VM | `tests/vm/` | Runtime unit tests |
| IOP | `tests/iop/` | Intent, Context, ASR tests |
| Benchmarks | `tests/benchmarks/` | Performance benchmarks |

## Writing Tests

### ProXPL Test Files

```javascript
// tests/language/test_functions.prox
func test_fibonacci() {
    assert(fib(10) == 55, "fibonacci(10) should be 55");
}
```

### C/C++ Unit Tests

```c
void test_lexer_numbers() {
    Scanner scanner;
    initScanner(&scanner, "42 3.14");
    Token t1 = nextToken(&scanner);
    assert(t1.type == TOKEN_NUMBER);
}
```

---

**Status**: Stable
