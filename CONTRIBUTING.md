# Contributing to ProXPL

Thank you for your interest in contributing to ProXPL! We welcome contributions from developers of all skill levels. Whether you're fixing a bug, adding a feature, improving documentation, or just asking questions, your help is appreciated! 🎉

This guide will help you understand how to contribute effectively to the ProXPL project.

## Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [Getting Started](#getting-started)
3. [How Can I Contribute?](#how-can-i-contribute)
4. [Development Workflow](#development-workflow)
5. [Coding Standards](#coding-standards)
6. [Testing Guidelines](#testing-guidelines)
7. [Documentation](#documentation)
8. [Submitting Changes](#submitting-changes)
9. [Review Process](#review-process)
10. [Getting Help](#getting-help)

---

## Code of Conduct

This project adheres to a [Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code. Please report unacceptable behavior to the project maintainers.

**Quick Summary:**
- Be respectful and inclusive
- Welcome newcomers and help them learn
- Focus on constructive feedback
- Assume good intentions

---

## Getting Started

### Prerequisites

Before you begin, make sure you have:

- **C/C++ Compiler**: GCC 9+, Clang 10+, or MSVC 2019+
- **CMake**: Version 3.15 or higher
- **LLVM**: Version 10+ (for LLVM backend development)
- **Git**: For version control
- **Text Editor/IDE**: VSCode, CLion, Vim, or your preference

### Setting Up Your Development Environment

1. **Fork the repository** on GitHub
2. **Clone your fork**:
   ```bash
   git clone https://github.com/YOUR_USERNAME/ProXPL.git
   cd ProXPL
   ```

3. **Add the upstream repository**:
   ```bash
   git remote add upstream https://github.com/ProgrammerKR/ProXPL.git
   ```

4. **Build the project**:
   ```bash
   mkdir build && cd build
   cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON ..
   make
   ```

5. **Run tests** to verify everything works:
   ```bash
   make test
   ```

6. **Try running an example**:
   ```bash
   ./proxpl ../examples/hello.prox
   ```

---

## How Can I Contribute?

### 🐛 Reporting Bugs

Found a bug? Please help us fix it!

1. **Check existing issues** to avoid duplicates
2. **Use the bug report template** when creating a new issue
3. **Include**:
   - ProXPL version (`proxpl --version`)
   - Operating system and version
   - Minimal code to reproduce the bug
   - Expected vs actual behavior
   - Error messages or stack traces

**Example Bug Report:**
```markdown
**ProXPL Version**: 1.0.0
**OS**: Windows 10 (64-bit)
**Description**: Crash when dividing by zero in function

**Code to Reproduce**:
```javascript
func test() {
    let x = 10 / 0;
}
test();
```

**Expected**: Runtime error with helpful message
**Actual**: Segmentation fault
```

### 💡 Suggesting Features

Have an idea for a new feature?

1. **Check the roadmap** in `README.md` - it might already be planned!
2. **Open a discussion** in [GitHub Discussions](https://github.com/ProgrammerKR/ProXPL/discussions)
3. **Describe**:
   - The problem this feature solves
   - Your proposed solution
   - Alternative approaches you considered
   - Example use cases

### 📝 Improving Documentation

Documentation is crucial! You can help by:

- Fixing typos or unclear explanations
- Adding code examples
- Writing tutorials or guides
- Improving API documentation
- Translating documentation (future)

**Areas needing documentation:**
- Standard library functions (`docs/stdlib/`)
- Architecture deep-dives (`docs/architecture/`)
- Language specification (`docs/language-spec/`)
- Example programs (`examples/`)

### 🔧 Contributing Code

We welcome code contributions in these areas:

**Compiler & Runtime:**
- Bug fixes in lexer, parser, or VM
- Performance optimizations
- Memory leak fixes
- New optimizations in IR optimizer

**Standard Library:**
- New built-in functions
- Performance improvements
- Better error handling

**Tooling:**
- LSP server improvements
- PRM package manager features
- CLI enhancements
- Debugging tools

**Tests:**
- Unit tests for core components
- Integration tests
- Benchmark programs
- Fuzzing harnesses

---

## Development Workflow

### Branch Naming Convention

Use descriptive branch names:

- `feature/add-string-interpolation`
- `bugfix/fix-parser-crash`
- `docs/improve-readme`
- `refactor/clean-vm-dispatch`
- `test/add-gc-tests`

### Workflow Steps

1. **Create a branch** from `main`:
   ```bash
   git checkout main
   git pull upstream main
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes** following our [coding standards](#coding-standards)

3. **Write tests** for your changes

4. **Build and test**:
   ```bash
   cd build
   make
   make test
   ```

5. **Commit your changes** with clear messages:
   ```bash
   git add .
   git commit -m "feat: add string interpolation support"
   ```

6. **Push to your fork**:
   ```bash
   git push origin feature/your-feature-name
   ```

7. **Create a Pull Request** on GitHub

### Commit Message Guidelines

We follow [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <description>

[optional body]

[optional footer]
```

**Types:**
- `feat:` New feature
- `fix:` Bug fix
- `docs:` Documentation changes
- `style:` Code style/formatting (no logic change)
- `refactor:` Code restructuring (no behavior change)
- `perf:` Performance improvements
- `test:` Adding or updating tests
- `build:` Build system changes
- `ci:` CI/CD changes
- `chore:` Maintenance tasks

**Examples:**
```bash
feat(parser): add support for ternary operator
fix(vm): prevent segfault on stack overflow
docs(stdlib): document math functions
refactor(gc): simplify mark phase
perf(compiler): optimize constant folding
test(lexer): add tests for string literals
```

---

## Coding Standards

We follow strict coding standards to maintain code quality. Please read [CODING_STANDARD.md](CODING_STANDARD.md) for complete details.

### C/C++ Style Guide (Summary)

**Formatting:**
- **Indentation**: 4 spaces (no tabs)
- **Braces**: K&R style (opening brace on same line)
- **Line Length**: 100 characters maximum
- **File Encoding**: UTF-8

**Naming Conventions:**

```c
// Types: PascalCase
typedef struct {
    int value;
} ObjectValue;

// Functions: camelCase
void compileExpression(Parser* parser);

// Variables: camelCase
int lineNumber = 0;

// Constants/Macros: SCREAMING_SNAKE_CASE
#define MAX_LOCALS 256
```

**Code Organization:**

```c
// 1. File header comment
/*
 * ProX Programming Language (ProXPL)
 * File: vm.c
 * Author: Your Name
 * Created: 2024-12-23
 * Copyright (c) 2024 Kanishk Raj (ProgrammerKR)
 */

// 2. Includes (system, then project)
#include <stdio.h>
#include <stdlib.h>

#include "vm.h"
#include "compiler.h"

// 3. Macros and constants
#define STACK_MAX 256

// 4. Type definitions
typedef struct { ... } MyType;

// 5. Function prototypes
static void helperFunction();

// 6. Function implementations
void publicFunction() {
    // ...
}

static void helperFunction() {
    // ...
}
```

**Best Practices:**

- ✅ **Single Responsibility**: Functions should do one thing well
- ✅ **Small Functions**: Aim for < 50 lines
- ✅ **Meaningful Names**: `getUserAge()` not `getUA()`
- ✅ **Comments**: Explain *why*, not *what*
- ✅ **Error Handling**: Check all allocations, handle all errors
- ✅ **No Magic Numbers**: Use named constants
- ✅ **Memory Safety**: Free what you allocate, avoid leaks

**Example:**

```c
// Good
int calculateFibonacci(int n) {
    if (n <= 1) return n;
    return calculateFibonacci(n - 1) + calculateFibonacci(n - 2);
}

// Bad
int calc(int x) {  // Unclear name
    if(x<=1)return x;  // Poor formatting
    return calc(x-1)+calc(x-2);  // No spaces
}
```

### Code Review Checklist

Before submitting, ensure your code:

- [ ] Compiles without warnings (`-Wall -Wextra`)
- [ ] Follows the style guide
- [ ] Includes appropriate comments
- [ ] Has no memory leaks (run with Valgrind)
- [ ] Includes tests for new functionality
- [ ] Updates relevant documentation
- [ ] Doesn't break existing tests

---

## Testing Guidelines

### Running Tests

```bash
# Build with tests
cd build
cmake -DBUILD_TESTS=ON ..
make

# Run all tests
make test

# Or use CTest for detailed output
ctest --output-on-failure

# Run specific test
./tests/lexer_test
./tests/parser_test
./tests/vm_test
```

### Writing Tests

We use a simple testing framework. Here's how to add tests:

**Example Unit Test** (`tests/unit/test_example.c`):

```c
#include "../../include/vm.h"
#include "../test_framework.h"

void test_vm_push_pop() {
    VM vm;
    initVM(&vm);
    
    // Test push
    push(&vm, NUMBER_VAL(42));
    ASSERT_EQUAL(vm.stackTop - vm.stack, 1);
    
    // Test pop
    Value val = pop(&vm);
    ASSERT_EQUAL(AS_NUMBER(val), 42);
    ASSERT_EQUAL(vm.stackTop - vm.stack, 0);
    
    freeVM(&vm);
}

int main() {
    RUN_TEST(test_vm_push_pop);
    PRINT_TEST_RESULTS();
    return 0;
}
```

**Integration Test** (ProXPL code):

Create `.prox` files in `tests/integration/`:

```javascript
// tests/integration/test_math.prox
// Test: Math operations
// Expected: All assertions pass

func test_addition() {
    assert(1 + 1 == 2, "Addition failed");
    assert(10 + 5 == 15, "Addition failed");
}

func test_multiplication() {
    assert(2 * 3 == 6, "Multiplication failed");
    assert(5 * 0 == 0, "Multiplication failed");
}

test_addition();
test_multiplication();
print("All tests passed!");
```

### Test Coverage

We aim for:
- **Core Components**: 80%+ coverage
- **New Features**: 100% coverage required
- **Bug Fixes**: Include regression test

---

## Documentation

### Code Documentation

Use clear comments for complex logic:

```c
/**
 * Compiles a function declaration.
 * 
 * This handles:
 * - Function parameters
 * - Local variables
 * - Nested scope
 * - Return statements
 *
 * @param compiler The current compiler state
 * @param type The function type (SCRIPT, FUNCTION, etc.)
 */
static void function(Compiler* compiler, FunctionType type) {
    // Implementation...
}
```

### Markdown Documentation

When adding to `docs/`:

- Use clear headings
- Include code examples
- Add diagrams where helpful
- Link to related documentation
- Test all code examples

### API Documentation

For standard library functions, document in `docs/stdlib/`:

```markdown
## `sqrt(number)`

Returns the square root of a number.

**Parameters:**
- `number` (Float): The number to calculate square root for

**Returns:**
- (Float): The square root of the number

**Errors:**
- Throws error if number is negative

**Example:**
```javascript
let result = sqrt(16);  // Returns 4.0
let x = sqrt(2);        // Returns 1.414...
```

**See Also:**
- `pow()` - Raise to power
- `abs()` - Absolute value
```

---

## Submitting Changes

### Pull Request Process

1. **Update documentation** for any changed functionality
2. **Add tests** that cover your changes
3. **Ensure all tests pass** locally
4. **Update CHANGELOG.md** if it's a notable change
5. **Create pull request** with clear description

### Pull Request Template

```markdown
## Description
Brief description of what this PR does.

## Type of Change
- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Documentation update

## Related Issues
Fixes #123
Related to #456

## Testing
- [ ] Unit tests added/updated
- [ ] Integration tests added/updated
- [ ] All tests pass locally
- [ ] Tested on multiple platforms (if applicable)

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Comments added for complex code
- [ ] Documentation updated
- [ ] No new warnings
- [ ] No memory leaks (Valgrind clean)

## Screenshots (if applicable)
Add screenshots for UI changes or visual improvements.

## Additional Notes
Any additional information reviewers should know.
```

---

## Review Process

### What to Expect

1. **Automated Checks**: CI/CD will run tests and linting
2. **Code Review**: Maintainers will review your code
3. **Feedback**: You may receive change requests
4. **Iteration**: Make requested changes
5. **Approval**: Once approved, your PR will be merged!

### Review Timeline

- **Initial response**: Within 2-3 days
- **Full review**: Within 1 week
- **Merge**: After approval and CI passes

### Being a Good Reviewer

If you're reviewing code:

- ✅ Be respectful and constructive
- ✅ Explain *why* changes are needed
- ✅ Suggest alternatives
- ✅ Acknowledge good code
- ✅ Test the changes locally
- ❌ Don't be condescending
- ❌ Don't nitpick style if CI passes
- ❌ Don't block on personal preference

---

## Getting Help

### Resources

- **Documentation**: Check `docs/` directory
- **Examples**: Browse `examples/` for code samples
- **Discussions**: [GitHub Discussions](https://github.com/ProgrammerKR/ProXPL/discussions)
- **Issues**: [GitHub Issues](https://github.com/ProgrammerKR/ProXPL/issues)

### Questions?

- **General questions**: Open a discussion
- **Bug reports**: Open an issue
- **Security issues**: Email maintainers directly (see SECURITY.md)
- **Feature requests**: Start a discussion first

### Mentorship

New to compiler development? We're happy to mentor!

- Look for issues tagged `good-first-issue`
- Ask questions in discussions
- Request guidance on complex features
- Pair program with maintainers

---

## Recognition

We value all contributions! Contributors will be:

- Listed in `AUTHORS` file
- Mentioned in release notes
- Credited in commit history
- Invited to become maintainers (for sustained contributions)

---

## Project Areas

### Easy Areas for Beginners

- 📝 **Documentation**: Always needs improvement
- 🧪 **Tests**: Add test coverage
- 🐛 **Bug fixes**: Start with `good-first-issue` label
- 📚 **Examples**: Create tutorial programs
- 🎨 **Error messages**: Make them more helpful

### Intermediate Areas

- 🔧 **Standard library**: Add new functions
- ⚡ **Optimizations**: Improve performance
- 🛠️ **Tooling**: Enhance CLI, PRM, LSP
- 📊 **Benchmarks**: Create performance tests

### Advanced Areas

- 🏗️ **Compiler**: Lexer, parser, type checker
- 🖥️ **VM**: Bytecode execution engine
- 🧹 **GC**: Garbage collector improvements
- 🚀 **LLVM Backend**: Native code generation
- 🔬 **Optimizations**: IR-level optimizations

---

## License

By contributing to ProXPL, you agree that your contributions will be licensed under the MIT License.

---

## Thank You! 🙏

Your contributions make ProXPL better for everyone. Whether you're fixing a typo or implementing a major feature, we appreciate your time and effort!

**Happy coding!** 🚀

---

<p align="center">
  <i>Questions? Open a <a href="https://github.com/ProgrammerKR/ProXPL/discussions">discussion</a>!</i>
</p>
