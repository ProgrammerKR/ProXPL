# ProXPL Coding Standards

**Version: 1.0**  
**Last Updated: December 2024**

This document defines the coding standards and best practices for the ProXPL project. All contributors must follow these guidelines to maintain code quality, consistency, and maintainability.

---

## Table of Contents

1. [General Principles](#general-principles)
2. [C Code Style](#c-code-style)
3. [Naming Conventions](#naming-conventions)
4. [Header Files](#header-files)
5. [Source Files](#source-files)
6. [Memory Management](#memory-management)
7. [Error Handling](#error-handling)
8. [Comments and Documentation](#comments-and-documentation)
9. [Code Organization](#code-organization)
10. [Testing Standards](#testing-standards)
11. [Git Workflow](#git-workflow)

---

## General Principles

### Clarity Over Cleverness

Write code that is easy to understand. A colleague should be able to read and understand your code without needing extensive comments.

```c
// Good
if (count > 0) {
    process_items(items, count);
}

// Avoid - Too clever
if (count && process_items(items, count)) { }
```

### Consistency

Maintain consistency in style, naming, and patterns. Read existing code and follow established patterns.

### Performance Matters

Write efficient code, but not at the expense of clarity. Profile before optimizing.

```c
// Good - Clear and efficient
for (int i = 0; i < len; i++) {
    process(data[i]);
}

// Avoid - Premature optimization
void *p = d; while (*(char*)p++) process(*(int*)p);
```

### Comments Over Code

Use comments to explain *why*, not *what*. The code shows what; comments explain why.

```c
// Good - Explains the intent
// Use insertion sort for small arrays (< 10 elements) as it's faster than quicksort
if (len < 10) {
    insertion_sort(arr, len);
}

// Avoid - Comment explains what code does
// Loop through array
for (int i = 0; i < len; i++) {
```

---

## C Code Style

### Indentation

Use **4 spaces** per indentation level. Never use tabs.

```c
if (condition) {
    if (nested_condition) {
        do_something();
    }
}
```

### Line Length

Keep lines to **80 characters** maximum. Break long lines at logical points.

```c
// Good
if (very_long_condition_name && another_condition &&
    yet_another_condition) {
    function_call_with_many_args(arg1, arg2, arg3,
                                  arg4, arg5, arg6);
}
```

### Braces

Always use braces for blocks, even single statements.

```c
// Good
if (condition) {
    return;
}

// Avoid
if (condition) return;
```

Opening brace on the same line (1TBS style):

```c
if (condition) {
    // body
} else if (other) {
    // body
} else {
    // body
}

for (int i = 0; i < 10; i++) {
    // body
}

func example() {
    // body
}
```

### Spacing

- 1 space around binary operators: `a + b`, `x = 5`
- 1 space after keywords: `if (x)`, `for (i = 0)`
- No space between function name and parentheses: `printf(...)`, `func()`
- No space inside parentheses: `func(a, b)` not `func( a, b )`

```c
// Good
int result = a + b * c;
if (x > 5 && y < 10) {
    printf("Value: %d\n", result);
}

// Avoid
int result=a+b*c;
if( x>5&&y<10 ){
    printf ( "Value: %d\n" , result ) ;
}
```

### Function Declarations

Place return type on same line as function name:

```c
// Good
int add(int a, int b) {
    return a + b;
}

void process_data(char* data, int len) {
    // implementation
}

// Avoid
int
add(int a, int b)
{
    return a + b;
}
```

---

## Naming Conventions

### Variables

Use **snake_case** for variables. Be descriptive.

```c
// Good
int user_count = 0;
char* file_path = NULL;
bool is_active = true;
struct Token* current_token = NULL;

// Avoid
int uc;           // Too short
int UserCount;    // Should be snake_case
int count_of_users_in_the_system;  // Too verbose
```

### Constants

Use **UPPER_SNAKE_CASE** for constants.

```c
#define MAX_BUFFER_SIZE 1024
#define DEFAULT_TIMEOUT 5000
#define PI 3.14159
const int MIN_VALUE = 0;
```

### Functions

Use **snake_case** for function names. Prefix internal functions with underscore.

```c
// Public functions
void parse_source_code(const char* source);
char* read_file_contents(const char* path);

// Internal/static functions
static void _validate_token(Token* token);
static int _hash_string(const char* str);
```

### Types

Use **PascalCase** for type names (structs, enums, typedefs).

```c
typedef struct {
    int value;
    char* name;
} IntegerValue;

typedef enum {
    TOKEN_INT,
    TOKEN_STRING,
    TOKEN_EOF
} TokenType;

typedef int (*CompareFunc)(const void*, const void*);
```

### File Names

Use **snake_case** for file names, lowercase.

```
scanner.c       // Lexer component
parser.c        // Parser component
type_checker.c  // Type checking
value.h         // Value type definitions
```

---

## Header Files

### Header Guards

Use `#ifndef` style guards with ALL_CAPS naming.

```c
#ifndef PROX_PARSER_H
#define PROX_PARSER_H

// Declarations

#endif  // PROX_PARSER_H
```

### File Organization

```c
/*
 * scanner.h
 * Lexical analysis for ProXPL source code
 */

#ifndef PROX_SCANNER_H
#define PROX_SCANNER_H

#include <stddef.h>
#include <stdbool.h>

// Forward declarations
typedef struct Token Token;

// Type definitions
typedef enum {
    TOKEN_INT,
    TOKEN_STRING,
    // ... more
} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int length;
} Token;

// Function declarations (grouped by functionality)
void scanner_init(Scanner* scanner, const char* source);
Token scanner_next_token(Scanner* scanner);
void scanner_reset(Scanner* scanner);

#endif  // PROX_SCANNER_H
```

### Include Organization

Order includes as follows:

```c
#include <stdio.h>      // Standard C library
#include <stdlib.h>
#include <string.h>

#include "common.h"     // Project headers
#include "vm.h"
#include "value.h"
```

### No Implementation in Headers

Never put function implementations in headers (except static inline).

```c
// Good - Declaration only
void process_data(int* data, int len);

// Bad - Implementation in header
void process_data(int* data, int len) {
    for (int i = 0; i < len; i++) {
        // ...
    }
}

// OK - Static inline is acceptable for small functions
static inline int get_size(struct Array* arr) {
    return arr ? arr->size : 0;
}
```

---

## Source Files

### File Structure

```c
/*
 * parser.c
 * 
 * Implements recursive descent parser for ProXPL.
 * Converts token stream into Abstract Syntax Tree (AST).
 */

#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "ast.h"
#include "common.h"

// Type definitions (if not in header)
typedef struct {
    Token* tokens;
    int count;
    int current;
} Parser;

// Static helper functions
static ASTNode* _parse_expression(Parser* p);
static void _skip_whitespace(Parser* p);

// Public function implementations
ASTNode* parse(const char* source) {
    // implementation
}

// Helper function implementations
static ASTNode* _parse_expression(Parser* p) {
    // implementation
}
```

### Function Documentation

Document public functions with comment blocks:

```c
/*
 * Parses a string of source code into an Abstract Syntax Tree.
 * 
 * Parameters:
 *   source - Pointer to source code string (must be null-terminated)
 * 
 * Returns:
 *   Pointer to root AST node, or NULL if parse failed
 * 
 * Notes:
 *   - Caller is responsible for freeing returned AST
 *   - Sets global error if parsing fails
 */
ASTNode* parse(const char* source);
```

---

## Memory Management

### Allocation

Always check allocation results:

```c
// Good
char* buffer = (char*)malloc(size);
if (buffer == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return false;
}

// Avoid
char* buffer = (char*)malloc(size);  // No error check
```

### Freeing

Always free allocated memory:

```c
// Good
void cleanup_parser(Parser* p) {
    if (p == NULL) return;
    
    free(p->tokens);
    free(p->source);
    free(p);
}

// Make sure to set pointers to NULL after freeing in some contexts
```

### Memory Patterns

```c
// Pattern: Safe allocation with error handling
void* allocate(size_t size) {
    if (size == 0) return NULL;
    
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

// Pattern: Safe string duplication
char* duplicate_string(const char* str) {
    if (str == NULL) return NULL;
    
    size_t len = strlen(str) + 1;
    char* copy = (char*)malloc(len);
    if (copy == NULL) return NULL;
    
    strcpy(copy, str);
    return copy;
}
```

---

## Error Handling

### Return Codes

Functions should return meaningful status:

```c
// Good - Clear return values
typedef enum {
    PARSE_OK = 0,
    PARSE_ERROR = 1,
    PARSE_EOF = 2
} ParseStatus;

ParseStatus parse_expression(Parser* p) {
    if (is_at_end(p)) return PARSE_EOF;
    if (invalid_token(p)) return PARSE_ERROR;
    return PARSE_OK;
}
```

### Error Messages

Print informative error messages:

```c
// Good - Specific error message
fprintf(stderr, "Error at line %d: unexpected token '%s'\n", 
        token->line, token->value);

// Avoid - Generic messages
fprintf(stderr, "ERROR\n");
```

### Assert vs Check

Use assertions for programmer errors; checks for user/data errors:

```c
// Assertion - Programmer error (should never happen)
assert(ptr != NULL);
assert(index >= 0 && index < size);

// Check - Possible runtime error
if (file == NULL) {
    fprintf(stderr, "Cannot open file: %s\n", path);
    return false;
}
```

---

## Comments and Documentation

### Comment Style

Use `/* */` for block comments and `//` for line comments:

```c
/*
 * Multi-line comment for explaining
 * complex logic or documenting
 * public APIs
 */

// Single line comment for simple explanations
int x = 5;  // Line-end comment
```

### What to Comment

**DO comment:**
- Non-obvious algorithms or tricks
- Why a decision was made
- Important assumptions
- Complex interactions

**DON'T comment:**
- What the code obviously does
- Trivial statements

```c
// Good - Explains the why
// Use binary search instead of linear search
// because input array is guaranteed sorted
for (int left = 0, right = size - 1; left <= right; ) {
    int mid = (left + right) / 2;
    // ...
}

// Bad - Explains the obvious
i++;  // Increment i
```

### Documentation Comments

```c
/*
 * Computes the sum of two integers.
 * 
 * Parameters:
 *   a - First integer
 *   b - Second integer
 * 
 * Returns:
 *   Sum of a and b
 * 
 * Note:
 *   Does not handle integer overflow
 */
int add(int a, int b) {
    return a + b;
}
```

---

## Code Organization

### Module Structure

```
component/
├── public header     (component.h)
├── internal header   (component_internal.h - optional)
└── implementation    (component.c)
```

### Function Order in Source Files

```c
// 1. Includes
#include "required_headers.h"

// 2. Defines and constants
#define BUFFER_SIZE 256

// 3. Type definitions (if not in header)
typedef struct { /* ... */ } InternalType;

// 4. Forward declarations of static functions
static void _helper_function(int x);

// 5. Public function implementations
void public_function(void) { /* ... */ }

// 6. Static helper function implementations
static void _helper_function(int x) { /* ... */ }
```

### Module Dependencies

Keep dependencies acyclic:

```
common
├── value
├── memory
├── object
└── ...

parser
├── scanner
├── ast
└── type_checker

runtime
├── compiler
├── vm
└── chunk

stdlib
└── runtime
```

Avoid circular dependencies between modules.

---

## Testing Standards

### Test File Naming

```
tests/
├── unit/
│   ├── test_scanner.c
│   ├── test_parser.c
│   └── test_value.c
└── integration/
    ├── test_compile_run.c
    └── test_stdlib.c
```

### Test Structure

```c
#include <stdio.h>
#include "unity.h"           // Testing framework
#include "../include/module.h"

void setUp(void) {
    // Run before each test
}

void tearDown(void) {
    // Run after each test
}

void test_function_basic(void) {
    int result = add(3, 4);
    TEST_ASSERT_EQUAL_INT(7, result);
}

void test_function_edge_cases(void) {
    TEST_ASSERT_EQUAL_INT(0, add(0, 0));
    TEST_ASSERT_EQUAL_INT(-5, add(-3, -2));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_function_basic);
    RUN_TEST(test_function_edge_cases);
    return UNITY_END();
}
```

### Test Coverage

- Unit tests for all public functions
- Integration tests for system components
- Edge case testing (null, empty, boundary values)
- Error condition testing

---

## Git Workflow

### Commit Messages

Use clear, descriptive commit messages:

```
feat: Add new feature
- Implement X functionality
- Refactor Y for clarity
- Update documentation

fix: Fix bug in parser
- Resolve issue with token recognition
- Add test case covering regression

docs: Update README and contribution guide

refactor: Reorganize memory management module
```

### Branch Naming

```
feature/new-feature-name
bugfix/issue-description
refactor/component-reorganization
docs/documentation-update
test/test-coverage-improvement
```

### Pull Request Standards

- Clear title and description
- Link related issues
- All tests pass
- Code review approval required
- Follow code style guidelines

---

## Code Review Checklist

Before submitting a pull request, verify:

- [ ] Code follows naming conventions
- [ ] Indentation is 4 spaces (no tabs)
- [ ] Lines are under 80 characters
- [ ] Memory is properly allocated and freed
- [ ] Error handling is complete
- [ ] Comments explain *why*, not *what*
- [ ] No compiler warnings
- [ ] All tests pass
- [ ] Functions are documented
- [ ] Consistent with existing code style

---

## Tools and Automation

### Compiler Flags

Always compile with warnings enabled:

```bash
gcc -Wall -Wextra -std=c99 -Werror *.c
```

### Code Formatting

Format code consistently:

```bash
clang-format -i *.c *.h
```

### Linting

Check code quality:

```bash
clang-tidy *.c -- -I./include
```

### Memory Checking

Use valgrind to detect memory issues:

```bash
valgrind --leak-check=full ./program
```

---

## Conclusion

These standards ensure ProXPL codebase remains:
- **Maintainable** - Easy to read and modify
- **Reliable** - Consistent error handling and testing
- **Efficient** - Thoughtful performance without sacrificing clarity
- **Professional** - Industry-standard practices

When in doubt, prioritize **clarity and consistency** over cleverness.
