# ProXPL Language Specification

**Version:** 1.0.0  
**Status:** Stable  
**Last Updated:** December 2024

This document defines the complete specification of the ProXPL programming language.

ProXPL is a high-performance **multi-paradigm** language that integrates:
- **Object-Oriented Programming (OOP)**: Traditional class-based inheritance and encapsulation.
- **Intent-Oriented Programming (IOP)**: Defining goals via `intent` and fulfilling them via `resolver`.
- **Context-Oriented Programming (COP)**: Dynamic behavioral adaptation via `context`, `layer`, and `activate`.

---

## 1. Lexical Structure

### Keywords
ProXPL reserves the following keywords (60+). They cannot be used as identifiers.

| Category | Keywords |
|----------|----------|
| **Declarations** | `let`, `const`, `func`, `class`, `enum`, `struct`, `interface`, `abstract`, `tensor` |
| **Control Flow** | `if`, `else`, `while`, `for`, `switch`, `case`, `default`, `break`, `continue`, `return`, `try`, `catch`, `finally`, `throw`, `defer` |
| **Modules** | `import`, `from`, `export`, `as`, `use` |
| **Values** | `true`, `false`, `null`, `void` |
| **OOP/Access** | `this`, `super`, `static`, `public`, `private`, `protected`, `extends`, `implements` |
| **IOP/COP** | `intent`, `resolver`, `context`, `layer`, `activate` |
| **Async** | `async`, `await` |
| **Runtime/Ops** | `in`, `is`, `typeof`, `native` |
| **AI/Resilience**| `model`, `train`, `predict`, `resilient`, `recovery`, `verify`, `identity` |
| **Misc** | `print` (statement) |

### Identifiers
Identifiers must start with a letter (`a-z`, `A-Z`) or underscore (`_`), followed by any combination of letters, digits (`0-9`), or underscores. They are case-sensitive.

### Literals
ProXPL supports the following literals:
- **Numbers**: Decimal integers (`42`) and floating-point numbers (`3.14`).
- **Strings**: Text enclosed in double quotes (`"Hello"`). Supports escape sequences like `\n`, `\t`, `\"`.
- **Booleans**: `true` and `false`.
- **Null**: `null`.

### Comments
- **Single-line**: Starts with `//` and extends to the end of the line.
- **Multi-line**: Enclosed between `/*` and `*/`.

---

## 2. Data Types

### Primitive Types
| Type | Description | Example |
|------|-------------|---------|
| **Null** | Represents the absence of a value. | `null` |
| **Boolean** | Logical true or false. | `true`, `false` |
| **Number** | Double-precision floating point (Ints & Floats). | `42`, `3.14` |

### Complex & Collection Types
| Type | Description | Example |
|------|-------------|---------|
| **String** | Immutable UTF-8 encoded text. | `"Hello"` |
| **List** | Ordered, mutable collection. | `[1, 2, 3]` |
| **Dict** | Key-value hash map. | `{"key": "val"}` |
| **Set** | Unique, unordered collection. | `Set([1, 2])` |
| **Bytes** | Raw byte sequence. | `to_bytes("ABC")` |

### Callable & System Types
| Type | Description |
|------|-------------|
| **Function** | User-defined callable block of code. |
| **NativeFunc** | Built-in VM function. |
| **Class** | A template for creating objects. |
| **Instance** | An object created from a class. |
| **Module** | A loaded module object. |
| **Error** | Exception object for error handling. |

---

## 3. Operators

### Arithmetic
| Operator | Description |
|----------|-------------|
| `+` | Addition / Concatenation |
| `-` | Subtraction |
| `*` | Multiplication |
| `/` | Division |
| `%` | Modulus |
| `**` | Exponentiation |

### Comparison
| Operator | Description |
|----------|-------------|
| `==` | Equal to |
| `!=` | Not equal to |
| `<` | Less than |
| `>` | Greater than |
| `<=` | Less than or equal to |
| `>=` | Greater than or equal to |

### Logical
| Operator | Description |
|----------|-------------|
| `&&` | Logical AND |
| `\|\|` | Logical OR |
| `!` | Logical NOT |

### Bitwise
| Operator | Description | Example |
|----------|-------------|---------|
| `&` | Bitwise AND | `5 & 3` |
| `\|` | Bitwise OR | `5 \| 3` |
| `^` | Bitwise XOR | `5 ^ 3` |
| `~` | Bitwise NOT | `~5` |
| `<<` | Left shift | `5 << 1` |
| `>>` | Right shift | `5 >> 1` |

### Assignment
| Operator | Description |
|----------|-------------|
| `=` | Assign |
| `+=`, `-=`, `*=`, `/=`, `%=`, `**=` | Compound Arithmetic |
| `&=`, `\|=`, `^=`, `<<=`, `>>=` | Compound Bitwise |

### Special Operators
| Operator | Description | Usage |
|----------|-------------|-------|
| `.` | Member access | `obj.prop` |
| `?.` | Optional chaining | `obj?.prop` |
| `??` | Null coalescing | `val ?? default` |
| `? :` | Ternary operator | `cond ? true : false` |
| `..` | Range operator | `1..10` |
| `=>` | Arrow function | `(x) => x*2` |
| `++` / `--` | Increment/Decrement | `x++`, `--x` |

### Precedence (High to Low)
1. `()` `[]` `.` `?.`
2. Postfix `++` `--`
3. Unary `!` `~` `++` `--` `-`
4. `**`
5. `*` `/` `%`
6. `+` `-`
7. `<<` `>>`
8. Comparison `<` `<=` `>` `>=`
9. Equality `==` `!=`
10. `&` (Bitwise AND)
11. `^` (Bitwise XOR)
12. `|` (Bitwise OR)
13. `&&` (Logical AND)
14. `||` (Logical OR)
15. `??` (Null Coalesce)
16. Ternary `? :`
17. Assignment `=` etc.

---

## 4. Statements & Control Flow

### Variable Declaration
```javascript
let x = 10;              // Mutable
const PI = 3.14;         // Immutable
let count: int = 0;      // Optional type annotation
```

### Conditionals & Loops
- **If/Else**: `if (cond) { ... } else { ... }`
- **Switch**: `switch (val) { case 1: ... default: ... }`
- **While**: `while (cond) { ... }`
- **For**: `for (let i = 0; i < 10; i++) { ... }`
- **Control**: `break`, `continue`

### Exceptions
```javascript
try {
    throw Error("Oops");
} catch (e) {
    print(e);
} finally {
    // cleanup
}
```

### Functions
```javascript
func add(a, b) {
    return a + b;
}
```

---
## 5. Asynchronous Programming

ProXPL supports native asynchronous programming using `async` and `await` keywords, powered by LLVM Coroutines.

### Async Functions
Functions declared with `async` return a `Task` object immediately when called. The body executes asynchronously.

```javascript
async func fetchData(url) {
    // Simulating delay or I/O
    return "Response from " + url;
}
```

### Await Expression
The `await` keyword suspends the execution of the current `async` function until the awaited `Task` completes.

```javascript
async func main() {
    let data = await fetchData("https://example.com");
    print(data);
}
```

- `await` can be used on any `Task` object.
- If `await` is used in a synchronous function (like `main`), it performs a blocking wait.

---

## 6. Built-in Functions (Global)
These are available in the global scope without imports.

| Function | Description |
|----------|-------------|
| `len(val)` | Length of string, list, etc. |
| `to_int(v)`, `to_float(v)` | Type conversion (Numeric). |
| `to_string(v)`, `to_bool(v)` | Type conversion (String/Bool). |
| `to_hex(i)`, `to_bin(i)` | Integer to Hex/Binary string. |
| `char_at(str, i)` | Character at index. |
| `print(...)` | Output to console. |

---

## 7. Standard Libraries
Import these using `use <module>;` or `import ... from ...;`.

### **std.core** (Core Utilities)
| Function | Description |
|----------|-------------|
| `assert(cond, msg)` | Abort if condition is false. |
| `typeOf(val)` | Return type string ("int", "string", etc.). |
| `unwrap(opt)` | Force unwrap or panic. |
| `id(obj)` | Object memory identity. |
| `hash(val)` | Compute hash code to value. |

### **std.math** (Mathematics)
| Function | Description |
|----------|-------------|
| `abs(x)`, `ceil(x)`, `floor(x)`, `round(x)` | Basic arithmetic helpers. |
| `min(..)` , `max(..)` | Variadic min/max. |
| `pow(b,e)`, `sqrt(x)`, `exp(x)`, `log(x)` | Powers and exponents. |
| `sin(x)`, `cos(x)`, `tan(x)` | Trigonometry. |
| `random()`, `randint(min, max)` | Random number generation. |

### **std.string** (String Manipulation)
| Function | Description |
|----------|-------------|
| `upper(s)`, `lower(s)` | Case conversion. |
| `trim(s)` | Remove whitespace. |
| `split(s, del)`, `join(del, list)` | Split/Join strings. |
| `replace(s, old, new)` | String replacement. |
| `contains(s, sub)`, `startswith(s, p)` | Existence checks. |
| `substring(s, start, len)` | Substring extraction. |

### **std.io** (Input/Output)
| Function | Description |
|----------|-------------|
| `input(prompt)` | Read line from stdin. |
| `read_file(path)` | Read entire file content. |
| `write_file(path, content)` | Write string to file. |
| `append_file(path, content)` | Append string to file. |

### **std.sys** (System Interface)
| Function | Description |
|----------|-------------|
| `exit(code)` | Terminate program. |
| `env(key)` | Get environment variable. |
| `platform()` | Get OS name (windows, linux, macos). |
| `version()` | Get language version. |
| `exec(cmd)` | Execute shell command. |
| **Constants** | `OS_NAME`, `ARCH` |

### **std.collections** (*Planned*)
List/Dict manipulation: `push`, `pop`, `insert`, `remove`, `sort`, `reverse`, `keys`, `values`.

### **std.datetime** (*Planned*)
Time functions: `now`, `timestamp`, `sleep`, `format_date`.

---

## 8. Modules and Packages
- **Standard Library**: Built-in modules like `std.io`.
- **Packages**: External code managed by PRM (ProX Project Manager).
- **Resolution**: `stdlib` -> `packages` -> `relative paths`.

---

## 9. Grammar Reference (EBNF)

```ebnf
program        ::= declaration* EOF
declaration    ::= funcDecl | varDecl | contextDecl | statement
funcDecl       ::= "func" IDENTIFIER "(" parameters? ")" block
varDecl        ::= ("let" | "const") IDENTIFIER (":" type)? "=" expression ";"
contextDecl    ::= "context" IDENTIFIER "{" layerDecl* "}"
layerDecl      ::= "layer" IDENTIFIER "{" funcDecl* "}"
statement      ::= exprStmt | ifStmt | whileStmt | forStmt | returnStmt | activateStmt | block ...
activateStmt   ::= "activate" "(" expression ")" block
block          ::= "{" declaration* "}"
expression     ::= assignment
assignment     ::= IDENTIFIER "=" assignment | ternary
ternary        ::= logicOr ("?" expression ":" ternary)?
logicOr        ::= logicAnd ("||" logicAnd)*
logicAnd       ::= equality ("&&" equality)*
equality       ::= comparison (("==" | "!=") comparison)*
comparison     ::= term (("<" | ">" | "<=" | ">=") term)*
term           ::= factor (("+" | "-") factor)*
factor         ::= unary (("*" | "/" | "%") unary)*
unary          ::= ("!" | "-") unary | call
call           ::= primary ("(" args? ")" | "." IDENTIFIER | "[" expr "]")*
```

---

## 10. Examples

### Hello World
```javascript
func main() {
    print("Hello, World!");
}
main();
```

### File Processing
```javascript
use std.io;
try {
    let content = read_file("data.txt");
    print(content);
} catch (e) {
    print("Error: " + e);
}
```

### Fibonacci
```javascript
func fib(n) {
    if (n <= 1) return n;
    return fib(n-1) + fib(n-2);
}
print(fib(10));
```
