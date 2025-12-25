# ProXPL Language Specification

**Version:** 1.0.0  
**Status:** Stable  
**Last Updated:** December 2024

This document defines the complete specification of the ProXPL programming language.

---

## Table of Contents

1. [Lexical Structure](#lexical-structure)
2. [Data Types](#data-types)
3. [Operators](#operators)
4. [Statements](#statements)
5. [Expressions](#expressions)
6. [Functions](#functions)
7. [Modules](#modules)
8. [Standard Library](#standard-library)
9. [Grammar Reference](#grammar-reference)

---

## Lexical Structure

### Keywords (45)

ProXPL reserves 45 keywords that cannot be used as identifiers.

| Category | Keywords |
|----------|----------|
| **Declarations** | `let`, `const`, `func`, `class`, `enum`, `struct`, `interface`, `abstract` |
| **Control Flow** | `if`, `else`, `while`, `for`, `switch`, `case`, `default`, `break`, `continue`, `return`, `try`, `catch`, `finally`, `throw`, `defer` |
| **Modules** | `import`, `from`, `export`, `as`, `use` |
| **Values** | `true`, `false`, `null`, `void` |
| **OOP/Types** | `this`, `super`, `static`, `public`, `private`, `protected`, `extends`, `implements` |
| **Async** | `async`, `await` |
| **Operators/Runtime** | `in`, `is`, `typeof`, `native` |

### Identifiers

```
identifier ::= [a-zA-Z_][a-zA-Z0-9_]*
```

**Rules:**
- Must start with letter or underscore
- Can contain letters, digits, underscores
- Case-sensitive
- Cannot be a keyword

**Examples:**
```javascript
myVariable    // Valid
_private      // Valid
counter123    // Valid
123invalid    // Invalid (starts with digit)
class         // Invalid (keyword)
```

### Literals

**Integer:**
```javascript
42          // Decimal
0x2A        // Hexadecimal
0b101010    // Binary
0o52        // Octal
```

**Float:**
```javascript
3.14
2.5e10
1.5E-5
.5          // 0.5
```

**String:**
```javascript
"Hello, World!"
'Single quotes'
"Escape sequences: \n \t \\ \" \'"
```

**Boolean:**
```javascript
true
false
```

**Null:**
```javascript
null
```

### Comments

```javascript
// Single-line comment

/*
 * Multi-line comment
 * Can span multiple lines
 */
```

---

## Data Types

ProXPL supports 12 core data types.

### 1. Int

Signed 64-bit integers.

```javascript
let count = 42;
let negative = -10;
let hex = 0xFF;
```

### 2. Float

IEEE 754 double-precision floating-point.

```javascript
let pi = 3.14159;
let scientific = 6.022e23;
```

### 3. Bool

Boolean values.

```javascript
let isActive = true;
let hasError = false;
```

### 4. String

Immutable UTF-8 encoded text.

```javascript
let name = "Alice";
let message = "Hello, " + name;
let multiline = "Line 1\nLine 2";
```

### 5. List

Ordered, mutable collection.

```javascript
let numbers = [1, 2, 3, 4, 5];
let mixed = [1, "two", true, null];
let nested = [[1, 2], [3, 4]];

// Access
let first = numbers[0];  // 1

// Modify
numbers[0] = 10;
push(numbers, 6);
```

### 6. Dict

Key-value pairs (hash map).

```javascript
let person = {
    "name": "Alice",
    "age": 30,
    "active": true
};

// Access
let name = person["name"];

// Modify
person["email"] = "alice@example.com";
```

### 7. Set

Unique, unordered collection.

```javascript
let unique = Set([1, 2, 2, 3, 3]);  // {1, 2, 3}
```

### 8. Null

Represents absence of value.

```javascript
let empty = null;
```

### 9. Function

First-class functions.

```javascript
func add(a, b) {
    return a + b;
}

let multiply = func(a, b) { return a * b; };
```

### 10. NativeFunc

Built-in VM functions.

```javascript
let printer = print;  // Reference to native function
```

### 11. Bytes

Raw byte sequence.

```javascript
let data = to_bytes("Hello");
```

### 12. Error

Exception object.

```javascript
throw Error("Something went wrong");
```

---

## Operators

ProXPL defines 42 operators organized by category.

### Arithmetic (6)

| Operator | Description | Example |
|----------|-------------|---------|
| `+` | Addition | `5 + 3` → `8` |
| `-` | Subtraction | `5 - 3` → `2` |
| `*` | Multiplication | `5 * 3` → `15` |
| `/` | Division | `10 / 2` → `5` |
| `%` | Modulo | `10 % 3` → `1` |
| `**` | Exponentiation | `2 ** 3` → `8` |

### Comparison (6)

| Operator | Description | Example |
|----------|-------------|---------|
| `==` | Equal | `5 == 5` → `true` |
| `!=` | Not equal | `5 != 3` → `true` |
| `<` | Less than | `3 < 5` → `true` |
| `>` | Greater than | `5 > 3` → `true` |
| `<=` | Less or equal | `3 <= 3` → `true` |
| `>=` | Greater or equal | `5 >= 5` → `true` |

### Logical (3)

| Operator | Description | Example |
|----------|-------------|---------|
| `&&` | Logical AND | `true && false` → `false` |
| `||` | Logical OR | `true || false` → `true` |
| `!` | Logical NOT | `!true` → `false` |

### Bitwise (6)

| Operator | Description | Example |
|----------|-------------|---------|
| `&` | Bitwise AND | `5 & 3` → `1` |
| `|` | Bitwise OR | `5 | 3` → `7` |
| `^` | Bitwise XOR | `5 ^ 3` → `6` |
| `~` | Bitwise NOT | `~5` → `-6` |
| `<<` | Left shift | `5 << 1` → `10` |
| `>>` | Right shift | `5 >> 1` → `2` |

### Assignment (12)

| Operator | Description | Example |
|----------|-------------|---------|
| `=` | Assignment | `x = 5` |
| `+=` | Add and assign | `x += 3` |
| `-=` | Subtract and assign | `x -= 3` |
| `*=` | Multiply and assign | `x *= 3` |
| `/=` | Divide and assign | `x /= 3` |
| `%=` | Modulo and assign | `x %= 3` |
| `**=` | Exponent and assign | `x **= 2` |
| `&=` | Bitwise AND assign | `x &= 3` |
| `|=` | Bitwise OR assign | `x |= 3` |
| `^=` | Bitwise XOR assign | `x ^= 3` |
| `<<=` | Left shift assign | `x <<= 1` |
| `>>=` | Right shift assign | `x >>= 1` |

### Special (9)

| Operator | Description | Example |
|----------|-------------|---------|
| `.` | Member access | `obj.property` |
| `?.` | Optional chaining | `obj?.property` |
| `=>` | Arrow function | `(x) => x * 2` |
| `..` | Range | `1..10` |
| `?` | Ternary condition | `x > 0 ? "pos" : "neg"` |
| `:` | Ternary separator | (part of ternary) |
| `??` | Null coalescing | `x ?? default` |
| `++` | Increment | `x++` or `++x` |
| `--` | Decrement | `x--` or `--x` |

### Operator Precedence

From highest to lowest:

1. `()` `[]` `.` `?.`
2. `++` `--` (postfix)
3. `!` `~` `++` `--` (prefix) `-` (unary)
4. `**`
5. `*` `/` `%`
6. `+` `-`
7. `<<` `>>`
8. `<` `<=` `>` `>=`
9. `==` `!=`
10. `&`
11. `^`
12. `|`
13. `&&`
14. `||`
15. `??`
16. `? :`
17. `=` `+=` `-=` etc.

---

## Statements

### Variable Declaration

```javascript
let x = 10;              // Mutable variable
const PI = 3.14159;      // Immutable constant

// Type annotations (optional)
let count: int = 0;
let name: string = "Alice";
```

### If Statement

```javascript
if (condition) {
    // code
} else if (otherCondition) {
    // code
} else {
    // code
}
```

### While Loop

```javascript
while (condition) {
    // code
    if (shouldBreak) break;
    if (shouldSkip) continue;
}
```

### For Loop

```javascript
// Traditional for loop
for (let i = 0; i < 10; i = i + 1) {
    print(i);
}

// For-in loop (planned)
for (let item in collection) {
    print(item);
}
```

### Switch Statement

```javascript
switch (value) {
    case 1:
        print("One");
        break;
    case 2:
        print("Two");
        break;
    default:
        print("Other");
}
```

### Try-Catch

```javascript
try {
    riskyOperation();
} catch (error) {
    print("Error: " + error);
} finally {
    cleanup();
}
```

### Return Statement

```javascript
func getValue() {
    return 42;
}
```

---

## Expressions

### Function Call

```javascript
result = add(5, 3);
print("Hello");
```

### Member Access

```javascript
let name = person.name;
let item = list[0];
let value = dict["key"];
```

### Ternary Operator

```javascript
let result = condition ? valueIfTrue : valueIfFalse;
let sign = x >= 0 ? "positive" : "negative";
```

### Lambda Expressions

```javascript
let double = (x) => x * 2;
let add = (a, b) => a + b;
```

---

## Functions

### Function Declaration

```javascript
func greet(name) {
    return "Hello, " + name + "!";
}
```

### Function with Type Annotations

```javascript
func add(a: int, b: int): int {
    return a + b;
}
```

### Default Parameters (Planned)

```javascript
func greet(name = "World") {
    return "Hello, " + name + "!";
}
```

### Variadic Functions (Planned)

```javascript
func sum(...numbers) {
    let total = 0;
    for (let n in numbers) {
        total += n;
    }
    return total;
}
```

### Closures

```javascript
func makeCounter() {
    let count = 0;
    return func() {
        count = count + 1;
        return count;
    };
}

let counter = makeCounter();
print(counter());  // 1
print(counter());  // 2
```

---

## Modules

### The `use` Keyword

Import modules using `use`:

```javascript
use std.math;           // Standard library
use http.client;        // Installed package
use ./local_helper;     // Local file
```

### Resolution Order

When resolving `use mymod`, ProXPL searches:

1. **Standard Library**: `stdlib/mymod.prox`
2. **Packages**: `prox_modules/mymod/`
3. **Relative Path**: `./mymod.prox`

### Module Execution

- Modules execute once per session
- Subsequent imports return cached module
- Circular imports cause runtime error

### Package Manager (PRM)

```bash
prm install <package>   # Install package
prm update              # Update all packages
prm list                # List installed packages
prm search <query>      # Search for packages
```

**Project Structure:**
```
project/
├── prox.toml           # Project manifest
├── prox.lock           # Dependency lockfile
├── prox_modules/       # Installed packages
├── src/
│   └── main.prox
└── tests/
```

---

## Standard Library

ProXPL includes 75+ built-in functions across 8 modules.

### IO (5 functions)

```javascript
print(...args);                    // Print to console
let input = input("Prompt: ");     // Read user input
let content = read_file("file.txt");
write_file("out.txt", "content");
append_file("log.txt", "entry");
```

### Math (15 functions)

```javascript
abs(-5);              // 5
ceil(3.2);            // 4
floor(3.8);           // 3
round(3.5, 0);        // 4
max([1, 5, 3]);       // 5
min([1, 5, 3]);       // 1
pow(2, 8);            // 256
sqrt(16);             // 4
log(100, 10);         // 2
sin(PI / 2);          // 1
random();             // 0.0-1.0
randint(1, 100);      // Random int
```

### String (15 functions)

```javascript
len("hello");                    // 5
upper("hello");                  // "HELLO"
lower("HELLO");                  // "hello"
trim("  text  ");                // "text"
split("a,b,c", ",");             // ["a", "b", "c"]
join(",", ["a", "b"]);           // "a,b"
replace("hello", "l", "L");      // "heLLo"
startswith("hello", "he");       // true
contains("hello", "ll");         // true
substring("hello", 1, 4);        // "ell"
```

### Collections (15 functions)

```javascript
push(list, item);                // Add to end
pop(list);                       // Remove from end
insert(list, 0, item);           // Insert at index
remove(list, item);              // Remove first occurrence
range(1, 10, 2);                 // [1, 3, 5, 7, 9]
sort(list);                      // Sort in-place
reverse(list);                   // Reverse in-place
keys(dict);                      // Get all keys
values(dict);                    // Get all values
clone(collection);               // Shallow copy
```

### DateTime (5 functions)

```javascript
now();                           // Current timestamp
timestamp();                     // Unix timestamp
sleep(1.5);                      // Sleep 1.5 seconds
format_date(ts, "%Y-%m-%d");
parse_date("2024-12-23", "%Y-%m-%d");
```

### System (5 functions)

```javascript
exit(0);                         // Exit program
env("PATH");                     // Get environment variable
platform();                      // "windows", "linux", "macos"
version();                       // ProXPL version
exec("ls -la");                  // Execute shell command
```

### Convert (10 functions)

```javascript
to_int("42");                    // 42
to_float("3.14");                // 3.14
to_string(42);                   // "42"
to_bool(1);                      // true
to_hex(255);                     // "0xFF"
to_bin(5);                       // "0b101"
parse_json("{\"key\": \"value\"}");
stringify_json(obj);
```

### Runtime (5 functions)

```javascript
type(value);                     // "int", "string", etc.
assert(condition, "message");    // Assert condition
id(object);                      // Object identity
hash(value);                     // Hash code
is_instance(obj, type);          // Type check
```

---

## Grammar Reference

### Complete EBNF Grammar

```ebnf
program        ::= declaration* EOF

declaration    ::= funcDecl | varDecl | statement

funcDecl       ::= "func" IDENTIFIER "(" parameters? ")" block
varDecl        ::= ("let" | "const") IDENTIFIER (":" type)? "=" expression ";"

statement      ::= exprStmt | ifStmt | whileStmt | forStmt 
                 | returnStmt | breakStmt | continueStmt | block

exprStmt       ::= expression ";"
ifStmt         ::= "if" "(" expression ")" statement ("else" statement)?
whileStmt      ::= "while" "(" expression ")" statement
forStmt        ::= "for" "(" (varDecl | exprStmt | ";") expression? ";" expression? ")" statement
returnStmt     ::= "return" expression? ";"
breakStmt      ::= "break" ";"
continueStmt   ::= "continue" ";"
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
unary          ::= ("!" | "-") unary | power
power          ::= call ("**" unary)?
call           ::= primary ("(" arguments? ")" | "." IDENTIFIER | "[" expression "]")*
primary        ::= NUMBER | STRING | "true" | "false" | "null"
                 | IDENTIFIER | "(" expression ")" | listLiteral | dictLiteral

listLiteral    ::= "[" (expression ("," expression)*)? "]"
dictLiteral    ::= "{" (STRING ":" expression ("," STRING ":" expression)*)? "}"

parameters     ::= IDENTIFIER ("," IDENTIFIER)*
arguments      ::= expression ("," expression)*
type           ::= "int" | "float" | "string" | "bool" | "list" | "dict"
```

---

## Examples

### Hello World

```javascript
func main() {
    print("Hello, World!");
}

main();
```

### Fibonacci

```javascript
func fibonacci(n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

for (let i = 0; i < 10; i = i + 1) {
    print("fib(" + to_string(i) + ") = " + to_string(fibonacci(i)));
}
```

### File Processing

```javascript
use std.io;

func processFile(filename) {
    try {
        let content = read_file(filename);
        let lines = split(content, "\n");
        
        for (let i = 0; i < length(lines); i = i + 1) {
            print("Line " + to_string(i + 1) + ": " + lines[i]);
        }
    } catch (error) {
        print("Error reading file: " + error);
    }
}

processFile("data.txt");
```

---

<p align="center">
  <b>ProXPL Language Specification v1.0.0</b><br>
  <i>Complete, Consistent, Production-Ready</i>
</p>
