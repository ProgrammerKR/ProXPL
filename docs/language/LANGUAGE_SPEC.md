# ProXPL Language Specification

**Version: 0.4.0**  
**Last Updated: December 2024**

---

## Table of Contents

1. [Overview](#overview)
2. [Lexical Structure](#lexical-structure)
3. [Data Types](#data-types)
4. [Operators](#operators)
5. [Statements](#statements)
6. [Expressions](#expressions)
7. [Functions](#functions)
8. [Type System](#type-system)
9. [Memory and Scope](#memory-and-scope)
10. [Standard Library Overview](#standard-library-overview)

---

## Overview

**ProXPL** is a statically-typed, imperative programming language compiled to bytecode and executed on a custom stack-based virtual machine.

### Design Goals

- Simple, clear syntax (C-style with Python influences)
- Strong static type checking
- Multi-Paradigm: **Object-Oriented**, **Intent-Oriented**, and **Context-Oriented**
- Fast bytecode execution
- Rich standard library
- Educational and practical use

### Language Properties

| Property | Value |
|----------|-------|
| **Type System** | Static, strong typing with runtime checks |
| **Compilation** | Bytecode (JIT-ready) |
| **Execution Model** | Stack-based virtual machine |
| **Memory** | Automatic garbage collection |
| **Keywords** | 60+ reserved words |
| **Operators** | 45+ defined operators |
| **Core Types** | 13 fundamental types (including Tensor) |
| **Standard Library** | 75+ built-in functions |

---

## Lexical Structure

### Identifiers

An identifier is a sequence of letters, digits, and underscores, starting with a letter or underscore.

```javascript
identifier = (letter | '_') (letter | digit | '_')*
letter = 'a' ... 'z' | 'A' ... 'Z'
digit = '0' ... '9'
```

Valid identifiers:
```javascript
x
_private
camelCase
snake_case
CONSTANT_VALUE
class2D
__dunder__
```

### Keywords (45 Reserved Words)

ProXPL reserves 45 keywords that cannot be used as identifiers:

**Control Flow (8):**
```
if, else, while, for, switch, case, default, break, continue, return
```

**Declarations (8):**
```
let, const, func, class, enum, struct, interface, abstract
```

**Error Handling (5):**
```
try, catch, finally, throw, defer
```

**Modules (5):**
```
import, from, export, as, use
```

**Values (4):**
```
true, false, null, void
```

**Object-Oriented (7):**
```
this, super, static, public, private, protected, extends, implements
```

**Async (2):**
```
async, await
```

**IOP & COP (5):**
```
intent, resolver, context, layer, activate
```

**Type & Runtime (4):**
```
typeof, native, in, is
```

**Declarations Plus (1):**
```
tensor
```

**AI & Security (7):**
```
model, train, predict, resilient, recovery, verify, identity
```

### Literals

#### Numeric Literals

**Integers:**
```javascript
42
-17
0
9999999
```

**Floating-Point:**
```javascript
3.14
-2.5
0.0
1e10
2.5e-3
```

#### String Literals

Strings are enclosed in double quotes with escape sequences:

```javascript
"Hello, World!"
"Line 1\nLine 2"
"Tab\there"
"Quote: \"hello\""
"Backslash: \\"
```

Escape sequences:
- `\n` - Newline
- `\t` - Tab
- `\r` - Carriage return
- `\\` - Backslash
- `\"` - Double quote
- `\xHH` - Hex character

#### Boolean Literals

```javascript
true
false
```

#### Null Literal

```javascript
null
```

### Comments

**Single-line comments:**
```javascript
// This is a comment
let x = 42; // End-of-line comment
```

**Multi-line comments:**
```javascript
/*
 * This is a multi-line comment
 * It can span multiple lines
 */
```

---

## Data Types

ProXPL has **12 core data types**:

### 1. **Int** - Integer Numbers

```javascript
let count = 42;
let negative = -100;
let zero = 0;
let large = 999999999;
```

Range: -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807 (64-bit)

Operations: `+`, `-`, `*`, `/`, `%`, `**`, `==`, `!=`, `<`, `>`, `<=`, `>=`

### 2. **Float** - Floating-Point Numbers

```javascript
let pi = 3.14159;
let scientific = 1.23e-4;
let negative = -2.5;
```

Operations: `+`, `-`, `*`, `/`, `%`, `**`, `==`, `!=`, `<`, `>`, `<=`, `>=`

### 3. **Bool** - Boolean Values

```javascript
let active = true;
let done = false;
let result = 5 > 3;  // true
```

Operations: `&&`, `||`, `!`, `==`, `!=`

### 4. **String** - Text Data

```javascript
let greeting = "Hello";
let empty = "";
let multiline = "Line 1\nLine 2";
```

Operations: Concatenation (`+`), comparison (`==`, `!=`, `<`, `>`), indexing (`[i]`)

### 5. **List** - Ordered Collections

```javascript
let numbers = [1, 2, 3];
let empty = [];
let mixed = [1, "two", 3.0];
let nested = [[1, 2], [3, 4]];
```

Operations:
- Indexing: `list[0]`
- Length: `len(list)`
- Modification: `list[0] = 10`
- Functions: `push`, `pop`, `insert`, `remove`, `sort`, `reverse`

### 6. **Dict** - Key-Value Mapping

```javascript
let person = {"name": "Alice", "age": 30};
let empty = {};
let nested = {"user": {"id": 1, "name": "Bob"}};
```

Operations:
- Access: `dict["key"]`
- Keys: `keys(dict)` returns list
- Values: `values(dict)` returns list
- Check: `contains_key(dict, key)`

### 7. **Set** - Unique Collections

```javascript
let unique = Set([1, 2, 3]);
let empty = Set([]);
```

Operations: Add, remove, contains, union, intersection

### 8. **Null** - Absence of Value

```javascript
let nothing = null;
let maybe = null;

if (maybe == null) {
    print("No value");
}
```

### 9. **Function** - Callable Code

```javascript
func add(a, b) {
    return a + b;
}

let result = add(3, 4);  // 7
```

### 10. **NativeFunc** - Built-in Function

```javascript
print("Hello");  // print is a NativeFunc
let len = len;   // Store reference to built-in
```

### 11. **Bytes** - Raw Data

```javascript
let data = bytes([1, 2, 3]);
let str_bytes = to_bytes("Hello");
```

### 12. **Error** - Exception Object

```javascript
try {
    throw Error("Something went wrong");
} catch (e) {
    print(e);  // Error type
}
```

---

## Operators

ProXPL defines **42 operators** with standard precedence and associativity.

### Operator Precedence (Highest to Lowest)

1. **Postfix**: `++`, `--`, `()`, `[]`, `.`
2. **Unary**: `!`, `-`, `~`, `++`, `--` (prefix), `typeof`
3. **Exponentiation**: `**` (right-associative)
4. **Multiplicative**: `*`, `/`, `%`
5. **Additive**: `+`, `-`
6. **Bitwise Shift**: `<<`, `>>`
7. **Relational**: `<`, `>`, `<=`, `>=`, `in`
8. **Equality**: `==`, `!=`
9. **Bitwise AND**: `&`
10. **Bitwise XOR**: `^`
11. **Bitwise OR**: `|`
12. **Logical AND**: `&&`
13. **Logical OR**: `||`
14. **Ternary**: `?` `:`
15. **Null Coalescing**: `??`
16. **Assignment**: `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `**=`, `&=`, `|=`, `^=`, `<<=`, `>>=`

### Arithmetic Operators

| Operator | Name | Example | Result |
|----------|------|---------|--------|
| `+` | Addition | `5 + 3` | `8` |
| `-` | Subtraction | `5 - 3` | `2` |
| `*` | Multiplication | `5 * 3` | `15` |
| `/` | Division | `15 / 3` | `5` |
| `%` | Modulo | `17 % 5` | `2` |
| `**` | Exponentiation | `2 ** 8` | `256` |

### Comparison Operators

| Operator | Name | Example |
|----------|------|---------|
| `==` | Equal to | `5 == 5` → `true` |
| `!=` | Not equal | `5 != 3` → `true` |
| `<` | Less than | `3 < 5` → `true` |
| `>` | Greater than | `5 > 3` → `true` |
| `<=` | Less or equal | `5 <= 5` → `true` |
| `>=` | Greater or equal | `5 >= 3` → `true` |

### Logical Operators

| Operator | Name | Short-circuit | Example |
|----------|------|---------------|---------|
| `&&` | AND | Yes | `true && false` → `false` |
| `||` | OR | Yes | `true \|\| false` → `true` |
| `!` | NOT | No | `!true` → `false` |

### Bitwise Operators

| Operator | Name | Example |
|----------|------|---------|
| `&` | AND | `5 & 3` → `1` (0101 & 0011) |
| `\|` | OR | `5 \| 3` → `7` (0101 \| 0011) |
| `^` | XOR | `5 ^ 3` → `6` (0101 ^ 0011) |
| `~` | NOT | `~5` → `-6` |
| `<<` | Left shift | `5 << 1` → `10` |
| `>>` | Right shift | `10 >> 1` → `5` |

### Assignment Operators

| Operator | Equivalent | Example |
|----------|-----------|---------|
| `=` | Direct assignment | `x = 5` |
| `+=` | `x = x + y` | `x += 3` |
| `-=` | `x = x - y` | `x -= 3` |
| `*=` | `x = x * y` | `x *= 2` |
| `/=` | `x = x / y` | `x /= 2` |
| `%=` | `x = x % y` | `x %= 3` |
| `**=` | `x = x ** y` | `x **= 2` |

### Special Operators

| Operator | Purpose | Example |
|----------|---------|---------|
| `.` | Member access | `obj.property` |
| `?.` | Optional chaining | `obj?.property` |
| `()` | Function call | `func()` |
| `[]` | Indexing | `list[0]` |
| `..` | Range | `1..5` |
| `??` | Null coalescing | `a ?? b` |
| `=>` | Arrow function | `(x) => x * 2` |

---

## Statements

### Variable Declaration

**let** - Mutable variable
```javascript
let x = 5;
x = 10;  // OK
```

**const** - Immutable constant
```javascript
const PI = 3.14159;
// PI = 3.14;  // Error!
```

### Expression Statement

Any expression followed by semicolon:
```javascript
print("Hello");
x = x + 1;
foo();
```

### Block Statement

Curly braces create a scope:
```javascript
{
    let x = 10;
    print(x);
}
// x is no longer accessible here
```

### If Statement

```javascript
if (condition) {
    // statement(s)
} else if (condition2) {
    // statement(s)
} else {
    // statement(s)
}
```

### While Loop

```javascript
while (condition) {
    // statement(s)
}
```

### For Loop

```javascript
for (let i = 0; i < 10; i = i + 1) {
    // statement(s)
}
```

### Switch Statement

```javascript
switch (value) {
    case 1:
        // statement(s)
        break;
    case 2:
        // statement(s)
        break;
    default:
        // statement(s)
}
```

### Break & Continue

```javascript
while (true) {
    if (should_exit) {
        break;      // Exit loop
    }
    if (should_skip) {
        continue;   // Skip to next iteration
    }
}
```

### Return Statement

```javascript
func getValue() {
    return 42;
}

func noValue() {
    return;        // Return void
}
```

### Try-Catch-Finally

```javascript
try {
    // statements
} catch (e) {
    // error handling
} finally {
    // cleanup (always runs)
}
```

### Throw Statement

```javascript
throw Error("Something went wrong");
```

### Defer Statement

Schedules code to run before function returns:

```javascript
func example() {
    defer print("Cleanup");
    
    print("Main work");
    return;
    
    // Output:
    // Main work
    // Cleanup
}
```

---

## Expressions

### Literals

```javascript
42              // Integer
3.14            // Float
"hello"         // String
true, false     // Boolean
null            // Null
[1, 2, 3]       // List
{"a": 1}        // Dictionary
```

### Identifier Reference

```javascript
let x = 5;
print(x);       // Reference to variable
```

### Unary Expressions

```javascript
-x              // Negation
!condition      // Logical NOT
~bits           // Bitwise NOT
typeof value    // Type check
++x             // Pre-increment
x++             // Post-increment
```

### Binary Expressions

```javascript
a + b           // Arithmetic
a == b          // Comparison
a && b          // Logical
a & b           // Bitwise
```

### Ternary Conditional

```javascript
condition ? true_expr : false_expr

let sign = n > 0 ? "positive" : "negative";
```

### Function Call

```javascript
print("hello")
add(5, 3)
obj.method(arg1, arg2)
```

### Indexing

```javascript
list[0]         // List index
dict["key"]     // Dictionary key
str[2]          // String character
```

### Member Access

```javascript
obj.property
obj["property"]
```

### List/Dict Literal

```javascript
[1, 2, 3]
[1, [2, 3], 4]
{"name": "Alice", "age": 30}
```

### Range Expression

```javascript
1..5            // [1, 2, 3, 4, 5]
1..5 by 2       // [1, 3, 5]
```

---

## Functions

### Function Declaration

```javascript
func add(a, b) {
    return a + b;
}

func main() {
    let result = add(3, 4);
    print(result);
}

main();
```

### Parameters and Arguments

```javascript
// No parameters
func greet() {
    return "Hello";
}

// Multiple parameters
func multiply(a, b) {
    return a * b;
}

// Default return (void)
func printValue(x) {
    print(x);
}
```

### Return Values

```javascript
func absolute(n) {
    if (n < 0) {
        return -n;
    }
    return n;
}
```

### Recursion

```javascript
func factorial(n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
```

### Closures

Functions can capture variables from outer scopes:

```javascript
func makeAdder(x) {
    func add(y) {
        return x + y;  // x is captured
    }
    return add;
}

let addFive = makeAdder(5);
print(addFive(3));  // 8
```

### Arrow Functions (Planned)

```javascript
let double = (x) => x * 2;
let result = double(5);  // 10
```

---

## Type System

### Static Type Checking

ProXPL performs compile-time type checking:

```javascript
let x = 5;           // x is Int
x = "hello";         // Type error: String != Int

let s = "text";
let len = len(s);    // len is Int
```

### Type Coercion

Explicit type conversion:

```javascript
let num = 42;
let str = to_string(num);        // "42"
let back = to_int(str);          // 42

let float_val = to_float("3.14");
let int_val = to_int(3.14);      // 3 (truncated)
```

### Type Checking at Runtime

```javascript
if (typeof value == "int") {
    // value is an Int
}

let is_list = value is List;
```

---

## Memory and Scope

### Variable Scope

Variables are block-scoped:

```javascript
{
    let x = 5;
    print(x);  // OK
}
// x is not accessible here

func test() {
    let local = 10;
}
// local is not accessible here
```

### Shadowing

Inner scope can shadow outer:

```javascript
let x = 5;
{
    let x = 10;
    print(x);  // 10
}
print(x);      // 5
```

### Lifetime

Variables live for the duration of their scope:

```javascript
func test() {
    let temp = [1, 2, 3];
    // ... use temp
}  // temp is freed here (GC)
```

### Garbage Collection

ProXPL uses automatic garbage collection:

```javascript
func process() {
    let data = read_file("large.txt");
    // ... process data
}  // data is automatically freed

let list = [1, 2, 3, 4, 5];
list = null;  // Eligible for GC
```

---

## Standard Library Overview

ProXPL includes 75+ built-in functions organized into modules:

### I/O Module (5 functions)
- `print(...args)` - Print to console
- `input(prompt)` - Read user input
- `read_file(path)` - Read file contents
- `write_file(path, content)` - Write to file
- `append_file(path, content)` - Append to file

### Math Module (15 functions)
- `abs(n)`, `ceil(n)`, `floor(n)`, `round(n)`
- `sqrt(n)`, `pow(base, exp)`, `log(n)`, `exp(n)`
- `sin(n)`, `cos(n)`, `tan(n)`
- `max(list)`, `min(list)`
- `random()`, `randint(min, max)`

### String Module (15 functions)
- `len(s)` - String length
- `upper(s)`, `lower(s)` - Case conversion
- `trim(s)` - Remove whitespace
- `split(s, sep)` - Split string
- `join(sep, list)` - Join list to string
- `replace(s, old, new)` - String replacement
- `startswith(s, prefix)`, `endswith(s, suffix)`
- `contains(haystack, needle)`
- `index_of(s, sub)`, `substring(s, start, end)`
- `char_at(s, index)`
- `format(fmt, ...args)`
- `to_bytes(s)`

### Collections Module (15 functions)
- `push(list, item)` - Add to list
- `pop(list)` - Remove from list
- `insert(list, idx, item)` - Insert at index
- `remove(list, item)` - Remove by value
- `sort(list)` - Sort list
- `reverse(list)` - Reverse list
- `range(start, end, step)` - Create range
- `keys(dict)` - Get dictionary keys
- `values(dict)` - Get dictionary values
- `entries(dict)` - Get key-value pairs
- `contains_key(dict, key)` - Check key exists
- `merge(d1, d2)` - Merge dictionaries
- `clone(col)` - Shallow copy
- `deep_clone(col)` - Deep copy
- `clear(col)` - Clear collection

### Type Conversion Module (10 functions)
- `to_int(value)` - Convert to integer
- `to_float(value)` - Convert to float
- `to_string(value)` - Convert to string
- `to_bool(value)` - Convert to boolean
- `to_list(value)` - Convert to list
- `to_dict(value)` - Convert to dict
- `to_hex(value)` - Convert to hex
- `to_bin(value)` - Convert to binary
- `to_bytes(value)` - Convert to bytes
- `parse_json(str)` - Parse JSON

### System Module (5 functions)
- `clock()` - Get current time
- `sleep(ms)` - Pause execution
- `exit(code)` - Exit program
- `platform()` - Get OS info
- `exec(cmd)` - Execute command

### Runtime Module (5 functions)
- `typeof(value)` - Get type name
- `assert(condition, message)` - Assertion
- `id(value)` - Get object ID
- `hash(value)` - Get hash value
- `is_instance(value, type)` - Type check

See [STDLIB_DOC.md](STDLIB_DOC.md) for complete documentation with examples.

---

## Examples

### Example 1: Variables and Types

```javascript
func main() {
    let num = 42;
    let pi = 3.14159;
    let name = "ProXPL";
    let active = true;
    
    print(num);
    print(pi);
    print(name);
    print(active);
}

main();
```

### Example 2: Control Flow

```javascript
func main() {
    for (let i = 1; i <= 10; i = i + 1) {
        if (i % 2 == 0) {
            print(to_string(i) + " is even");
        }
    }
}

main();
```

### Example 3: Functions and Recursion

```javascript
func fib(n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}

func main() {
    for (let i = 0; i < 10; i = i + 1) {
        print(to_string(fib(i)));
    }
}

main();
```

### Example 4: Collections

```javascript
func main() {
    let numbers = [1, 2, 3, 4, 5];
    push(numbers, 6);
    
    let person = {"name": "Alice", "age": 30};
    print(person["name"]);
    
    let keys = keys(person);
    print(keys);
}

main();
```

### Example 5: Error Handling

```javascript
func safe_divide(a, b) {
    if (b == 0) {
        throw Error("Division by zero");
    }
    return a / b;
}

func main() {
    try {
        let result = safe_divide(10, 2);
        print(to_string(result));
    } catch (e) {
        print("Error: " + to_string(e));
    }
}

main();
```


---

## Foreign Function Interface

ProXPL supports loading and calling external native functions using the `extern` declaration.

### Syntax
```javascript
extern "libname" "symbol" func name(param1, param2);
```

- **libname**: Path or name of the dynamic library (`.dll`, `.so`, `.dylib`).
- **symbol**: Exact name of the exported symbol in the library.
- **name**: Name used to call the function within ProXPL.
- **params**: Parameter names (types are marshalled automatically).

### Type Marshalling

| ProXPL Type | C Type (Argument) | C Type (Return) |
|-------------|-------------------|-----------------|
| `Number`    | `int` / `double`  | `int` (default) |
| `String`    | `char*`           | `char*`         |
| `Bool`      | `int`             | `int`           |
| `Null`      | `NULL`            | `NULL`          |

> **Note**: The current implementation primarily supports integer return types. Support for floating-point and void returns is planned.

---

## Conclusion

This specification defines the core ProXPL language. For detailed information on specific features, see:

- [STDLIB_DOC.md](STDLIB_DOC.md) - Standard library reference
- [ARCHITECTURE.md](ARCHITECTURE.md) - Implementation details
- [CODING_STANDARD.md](CODING_STANDARD.md) - Development guidelines
- [examples/](examples/) - Code examples
