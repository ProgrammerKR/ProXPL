# ProXPL Language Specification

**Version:** 1.6.3
**Status:** Stable

Complete grammar, syntax, and semantics of ProXPL.

## Overview

ProXPL is a statically-typed, multi-paradigm language combining:
- **Object-Oriented Programming (OOP)**
- **Intent-Oriented Programming (IOP)**
- **Context-Oriented Programming (COP)**

## Quick Reference

### Lexical Structure
- **Keywords**: 60+ reserved words
- **Identifiers**: Start with letter/underscore, case-sensitive
- **Literals**: Numbers (`42`, `3.14`), strings (`"hello"`), booleans, null

### Data Types
| Type | Description |
|------|-------------|
| `Int`, `Float` | Numeric types |
| `Bool` | Boolean |
| `String` | UTF-8 text |
| `List`, `Dict`, `Set` | Collections |
| `Tensor` | Multi-dimensional arrays |
| `Function`, `Class`, `Instance` | Callable/object types |

### Operators
- Arithmetic: `+`, `-`, `*`, `/`, `%`, `**`, `@` (matrix)
- Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`
- Logical: `&&`, `||`, `!`
- Bitwise: `&`, `|`, `^`, `~`, `<<`, `>>`
- Special: `.` (member), `?.` (optional chaining), `??` (null coalesce), `..` (range)

### Statements
```javascript
let x = 42;                  // Variable declaration
const PI = 3.14;             // Constant
if (x > 0) { ... }           // Conditional
for (let i = 0; i < 10; i++) // Loop
func add(a, b) { return a + b } // Function
```

### Functions
```javascript
func add(a, b) {
    return a + b;
}

async func fetch(url) {
    return await http.get(url);
}
```

### Classes
```javascript
class Animal {
    func makeSound() { print("sound") }
}

class Dog extends Animal {
    func makeSound() { print("Woof!") }
}
```

### Modules
```javascript
use std.math;
use std.io;
```

### Control Flow
```javascript
try {
    throw Error("error");
} catch (e) {
    print(e);
} finally {
    // cleanup
}
```

### Type System
- Static typing with type inference
- Hindley-Milner style inference
- Type compatibility rules

---

**Status**: Stable
**See Also**: [Syntax](SYNTAX.md), [Grammar](Grammar.md), [Reference](REFERENCE.md)
