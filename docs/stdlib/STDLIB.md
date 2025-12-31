# ProXPL Standard Library Reference

**Complete Documentation for Built-in Functions**

**Version:** 0.4.0  
**Last Updated:** December 2024  
**Status:** Stable

---

## Table of Contents

1. [Introduction](#introduction)
2. [Core Functions](#core-functions)
3. [Mathematical Operations](#mathematical-operations)
4. [String Manipulation](#string-manipulation)
5. [Collections](#collections)
6. [Type Conversion](#type-conversion)
7. [File I/O](#file-io)
8. [System Functions](#system-functions)
9. [Error Handling](#error-handling)
10. [Quick Reference](#quick-reference)

---

## Introduction

### Philosophy

The ProXPL Standard Library is designed with two core principles:

1. **⚡ Efficiency**: All functions are implemented in native C for maximum performance
2. **🎯 Simplicity**: Clean, intuitive APIs that are easy to learn and use

### Accessing Built-in Functions

ProXPL provides **75+ built-in functions** that are available globally without any imports:

```javascript
// Direct access - no imports needed
print("Hello, World!");
let result = abs(-42);
let length = len("ProXPL");
```

For organized code, you can also use module-style access (optional):

```javascript
use std.math;
use std.string;

let result = math::sqrt(16);
let upper = string::toUpper("hello");
```

### Function Categories

| Category | Functions | Description |
|----------|-----------|-------------|
| **Core** | 5 | Basic I/O and type inspection |
| **Math** | 15 | Mathematical operations |
| **String** | 15 | String manipulation |
| **Collections** | 15 | List and dictionary operations |
| **Conversion** | 10 | Type conversion utilities |
| **File I/O** | 5 | File operations |
| **System** | 5 | System and environment |
| **DateTime** | 5 | Date and time utilities |

---

## Core Functions

### Overview

Core functions provide essential I/O and type inspection capabilities.

| Function | Signature | Description |
|----------|-----------|-------------|
| `print()` | `print(...args) -> void` | Print values to stdout |
| `println()` | `println(...args) -> void` | Print with newline |
| `input()` | `input(prompt: string) -> string` | Read user input |
| `typeOf()` | `typeOf(value: any) -> string` | Get type name |
| `assert()` | `assert(condition: bool, message: string) -> void` | Assert condition |

---

### print()

Print one or more values to standard output without a trailing newline.

**Signature:**
```javascript
print(...args: any) -> void
```

**Parameters:**
- `...args` - Variable number of arguments of any type

**Returns:**
- `void` - No return value

**Behavior:**
- Converts all arguments to strings
- Concatenates with spaces
- Does not add newline at end

**Examples:**

```javascript
// Single value
print("Hello");              // Output: Hello

// Multiple values
print("Age:", 25);           // Output: Age: 25

// Mixed types
print("Result:", 3.14, true); // Output: Result: 3.14 true

// Expressions
let x = 10;
print("x =", x * 2);         // Output: x = 20
```

---

### println()

Print one or more values to standard output with a trailing newline.

**Signature:**
```javascript
println(...args: any) -> void
```

**Parameters:**
- `...args` - Variable number of arguments of any type

**Returns:**
- `void` - No return value

**Behavior:**
- Same as `print()` but adds `\n` at the end
- Flushes output buffer

**Examples:**

```javascript
println("Line 1");
println("Line 2");
// Output:
// Line 1
// Line 2

// Empty line
println();                   // Output: (blank line)

// Multiple values
println("Name:", "Alice", "Age:", 30);
// Output: Name: Alice Age: 30
```

---

### input()

Read a line of text from standard input.

**Signature:**
```javascript
input(prompt: string) -> string
```

**Parameters:**
- `prompt` - Text to display before reading input

**Returns:**
- `string` - User input (without trailing newline)

**Behavior:**
- Displays prompt to stdout
- Waits for user to press Enter
- Returns input as string
- Strips trailing newline

**Examples:**

```javascript
// Basic input
let name = input("Enter your name: ");
println("Hello, " + name + "!");

// Numeric input (requires conversion)
let ageStr = input("Enter age: ");
let age = to_int(ageStr);
println("You are " + to_string(age) + " years old");

// Validation loop
let valid = false;
while (!valid) {
    let answer = input("Continue? (yes/no): ");
    if (answer == "yes" || answer == "no") {
        valid = true;
    } else {
        println("Please enter 'yes' or 'no'");
    }
}
```

---

### typeOf()

Get the type name of a value as a string.

**Signature:**
```javascript
typeOf(value: any) -> string
```

**Parameters:**
- `value` - Any value to inspect

**Returns:**
- `string` - Type name: `"int"`, `"float"`, `"string"`, `"bool"`, `"null"`, `"list"`, `"dict"`, `"function"`, `"nativeFunc"`

**Examples:**

```javascript
// Primitives
println(typeOf(42));           // Output: int
println(typeOf(3.14));         // Output: float
println(typeOf("hello"));      // Output: string
println(typeOf(true));         // Output: bool
println(typeOf(null));         // Output: null

// Collections
println(typeOf([1, 2, 3]));    // Output: list
println(typeOf({"a": 1}));     // Output: dict

// Functions
func myFunc() { return 42; }
println(typeOf(myFunc));       // Output: function
println(typeOf(print));        // Output: nativeFunc

// Type checking
let value = 42;
if (typeOf(value) == "int") {
    println("It's an integer!");
}
```

---

### assert()

Assert that a condition is true, throwing an error if false.

**Signature:**
```javascript
assert(condition: bool, message: string) -> void
```

**Parameters:**
- `condition` - Boolean expression to test
- `message` - Error message if assertion fails

**Returns:**
- `void` - No return value (throws error on failure)

**Behavior:**
- If `condition` is `true`, does nothing
- If `condition` is `false`, throws runtime error with `message`

**Examples:**

```javascript
// Basic assertion
let age = 25;
assert(age >= 18, "Must be 18 or older");

// Function preconditions
func divide(a, b) {
    assert(b != 0, "Division by zero");
    return a / b;
}

// Invariant checking
let balance = 100;
balance = balance - 50;
assert(balance >= 0, "Balance cannot be negative");

// Type checking
func processNumber(x) {
    assert(typeOf(x) == "int" || typeOf(x) == "float", 
           "Expected numeric type");
    return x * 2;
}
```

---

## Mathematical Operations

### Overview

Mathematical functions provide common numeric operations with C-level performance.

| Function | Signature | Description |
|----------|-----------|-------------|
| `abs()` | `abs(x: number) -> number` | Absolute value |
| `sqrt()` | `sqrt(x: number) -> float` | Square root |
| `floor()` | `floor(x: number) -> int` | Round down |
| `ceil()` | `ceil(x: number) -> int` | Round up |
| `round()` | `round(x: number, digits: int) -> number` | Round to N digits |
| `pow()` | `pow(base: number, exp: number) -> number` | Exponentiation |
| `min()` | `min(list: list) -> number` | Minimum value |
| `max()` | `max(list: list) -> number` | Maximum value |
| `sin()` | `sin(x: number) -> float` | Sine (radians) |
| `cos()` | `cos(x: number) -> float` | Cosine (radians) |
| `tan()` | `tan(x: number) -> float` | Tangent (radians) |
| `log()` | `log(x: number, base: number) -> float` | Logarithm |
| `exp()` | `exp(x: number) -> float` | e^x |
| `random()` | `random() -> float` | Random [0, 1) |
| `randint()` | `randint(min: int, max: int) -> int` | Random integer |

---

### abs()

Return the absolute value of a number.

**Signature:**
```javascript
abs(x: number) -> number
```

**Parameters:**
- `x` - Integer or float

**Returns:**
- Same type as input, always non-negative

**Edge Cases:**
- `abs(0)` returns `0`
- `abs(-0.0)` returns `0.0`

**Examples:**

```javascript
println(abs(42));      // Output: 42
println(abs(-42));     // Output: 42
println(abs(0));       // Output: 0
println(abs(-3.14));   // Output: 3.14

// Distance calculation
let a = 10;
let b = 25;
let distance = abs(a - b);
println(distance);     // Output: 15
```

---

### sqrt()

Calculate the square root of a number.

**Signature:**
```javascript
sqrt(x: number) -> float
```

**Parameters:**
- `x` - Non-negative number

**Returns:**
- `float` - Square root of x

**Edge Cases:**
- `sqrt(0)` returns `0.0`
- `sqrt(negative)` throws runtime error
- `sqrt(1)` returns `1.0`

**Examples:**

```javascript
println(sqrt(16));     // Output: 4.0
println(sqrt(2));      // Output: 1.414213...
println(sqrt(0));      // Output: 0.0
println(sqrt(100));    // Output: 10.0

// Pythagorean theorem
let a = 3;
let b = 4;
let c = sqrt(a*a + b*b);
println(c);            // Output: 5.0

// Error case
// sqrt(-1);           // Runtime Error: sqrt of negative number
```

---

### floor()

Round a number down to the nearest integer.

**Signature:**
```javascript
floor(x: number) -> int
```

**Parameters:**
- `x` - Any number

**Returns:**
- `int` - Largest integer ≤ x

**Edge Cases:**
- `floor(5.9)` returns `5`
- `floor(-2.1)` returns `-3` (rounds toward negative infinity)
- `floor(5)` returns `5`

**Examples:**

```javascript
println(floor(3.7));    // Output: 3
println(floor(3.2));    // Output: 3
println(floor(-2.7));   // Output: -3
println(floor(-2.2));   // Output: -3
println(floor(5));      // Output: 5

// Integer division
let result = floor(7 / 2);
println(result);        // Output: 3
```

---

### ceil()

Round a number up to the nearest integer.

**Signature:**
```javascript
ceil(x: number) -> int
```

**Parameters:**
- `x` - Any number

**Returns:**
- `int` - Smallest integer ≥ x

**Edge Cases:**
- `ceil(5.1)` returns `6`
- `ceil(-2.9)` returns `-2` (rounds toward positive infinity)
- `ceil(5)` returns `5`

**Examples:**

```javascript
println(ceil(3.2));     // Output: 4
println(ceil(3.7));     // Output: 4
println(ceil(-2.2));    // Output: -2
println(ceil(-2.7));    // Output: -2
println(ceil(5));       // Output: 5

// Pages calculation
let items = 47;
let perPage = 10;
let pages = ceil(items / perPage);
println(pages);         // Output: 5
```

---

### pow()

Raise a number to a power.

**Signature:**
```javascript
pow(base: number, exp: number) -> number
```

**Parameters:**
- `base` - Base number
- `exp` - Exponent

**Returns:**
- `number` - base^exp

**Edge Cases:**
- `pow(x, 0)` returns `1` for any x
- `pow(0, 0)` returns `1` (by convention)
- `pow(negative, fraction)` may return NaN

**Examples:**

```javascript
println(pow(2, 8));      // Output: 256
println(pow(10, 3));     // Output: 1000
println(pow(5, 2));      // Output: 25
println(pow(2, -1));     // Output: 0.5
println(pow(4, 0.5));    // Output: 2.0 (square root)

// Compound interest
let principal = 1000;
let rate = 0.05;
let years = 10;
let amount = principal * pow(1 + rate, years);
println(amount);         // Output: 1628.89...
```

---

## String Manipulation

### Overview

String functions provide powerful text processing capabilities.

| Function | Signature | Description |
|----------|-----------|-------------|
| `len()` | `len(s: string) -> int` | String length |
| `substr()` | `substr(s: string, start: int, end: int) -> string` | Extract substring |
| `toUpper()` | `toUpper(s: string) -> string` | Convert to uppercase |
| `toLower()` | `toLower(s: string) -> string` | Convert to lowercase |
| `trim()` | `trim(s: string) -> string` | Remove whitespace |
| `split()` | `split(s: string, sep: string) -> list` | Split into list |
| `join()` | `join(sep: string, list: list) -> string` | Join list to string |
| `replace()` | `replace(s: string, old: string, new: string) -> string` | Replace substring |
| `startsWith()` | `startsWith(s: string, prefix: string) -> bool` | Check prefix |
| `endsWith()` | `endsWith(s: string, suffix: string) -> bool` | Check suffix |
| `contains()` | `contains(s: string, sub: string) -> bool` | Check substring |
| `indexOf()` | `indexOf(s: string, sub: string) -> int` | Find position |
| `charAt()` | `charAt(s: string, index: int) -> string` | Get character |

---

### len()

Get the length of a string (number of characters).

**Signature:**
```javascript
len(s: string) -> int
```

**Parameters:**
- `s` - String to measure

**Returns:**
- `int` - Number of characters (UTF-8 aware)

**Edge Cases:**
- `len("")` returns `0`
- Counts Unicode characters, not bytes

**Examples:**

```javascript
println(len("hello"));       // Output: 5
println(len(""));            // Output: 0
println(len("ProXPL"));      // Output: 6
println(len("Hello, World!")); // Output: 13

// Validation
let password = input("Enter password: ");
if (len(password) < 8) {
    println("Password too short!");
}

// Iteration
let text = "ProXPL";
for (let i = 0; i < len(text); i = i + 1) {
    println(charAt(text, i));
}
```

---

### substr()

Extract a substring from a string.

**Signature:**
```javascript
substr(s: string, start: int, end: int) -> string
```

**Parameters:**
- `s` - Source string
- `start` - Starting index (inclusive, 0-based)
- `end` - Ending index (exclusive)

**Returns:**
- `string` - Substring from start to end

**Edge Cases:**
- Negative indices not supported (throws error)
- `end > len(s)` is clamped to string length
- `start >= end` returns empty string

**Examples:**

```javascript
let text = "Hello, World!";

println(substr(text, 0, 5));    // Output: Hello
println(substr(text, 7, 12));   // Output: World
println(substr(text, 0, 13));   // Output: Hello, World!

// Extract extension
let filename = "document.pdf";
let dot = indexOf(filename, ".");
let ext = substr(filename, dot + 1, len(filename));
println(ext);                   // Output: pdf

// First N characters
let preview = substr("Long text here", 0, 10);
println(preview);               // Output: Long text
```

---

### toUpper()

Convert a string to uppercase.

**Signature:**
```javascript
toUpper(s: string) -> string
```

**Parameters:**
- `s` - String to convert

**Returns:**
- `string` - Uppercase version

**Behavior:**
- Converts ASCII letters to uppercase
- Non-letter characters unchanged
- Creates new string (original unchanged)

**Examples:**

```javascript
println(toUpper("hello"));      // Output: HELLO
println(toUpper("ProXPL"));     // Output: PROXPL
println(toUpper("hello123"));   // Output: HELLO123
println(toUpper("ALREADY"));    // Output: ALREADY

// Case-insensitive comparison
let input = input("Enter command: ");
if (toUpper(input) == "EXIT") {
    println("Exiting...");
}

// Formatting
let name = "alice";
let formatted = toUpper(substr(name, 0, 1)) + substr(name, 1, len(name));
println(formatted);             // Output: Alice
```

---

### toLower()

Convert a string to lowercase.

**Signature:**
```javascript
toLower(s: string) -> string
```

**Parameters:**
- `s` - String to convert

**Returns:**
- `string` - Lowercase version

**Behavior:**
- Converts ASCII letters to lowercase
- Non-letter characters unchanged
- Creates new string (original unchanged)

**Examples:**

```javascript
println(toLower("HELLO"));      // Output: hello
println(toLower("ProXPL"));     // Output: proxpl
println(toLower("HELLO123"));   // Output: hello123
println(toLower("already"));    // Output: already

// Email normalization
let email = input("Enter email: ");
let normalized = toLower(trim(email));
println("Normalized:", normalized);

// Case-insensitive search
let text = "The Quick Brown Fox";
let search = "quick";
if (contains(toLower(text), toLower(search))) {
    println("Found!");
}
```

---

### trim()

Remove leading and trailing whitespace from a string.

**Signature:**
```javascript
trim(s: string) -> string
```

**Parameters:**
- `s` - String to trim

**Returns:**
- `string` - Trimmed string

**Behavior:**
- Removes spaces, tabs, newlines from both ends
- Internal whitespace preserved
- Returns new string

**Examples:**

```javascript
println(trim("  hello  "));     // Output: hello
println(trim("\thello\n"));     // Output: hello
println(trim("hello"));         // Output: hello
println(trim("  a  b  "));      // Output: a  b

// Input cleaning
let name = input("Enter name: ");
name = trim(name);
if (len(name) == 0) {
    println("Name cannot be empty!");
}

// CSV parsing
let line = "  value1  ,  value2  ,  value3  ";
let parts = split(line, ",");
for (let i = 0; i < len(parts); i = i + 1) {
    parts[i] = trim(parts[i]);
}
```

---

## Collections

### Overview

Collection functions operate on lists and dictionaries.

| Function | Signature | Description |
|----------|-----------|-------------|
| `push()` | `push(list: list, item: any) -> void` | Add to end |
| `pop()` | `pop(list: list) -> any` | Remove from end |
| `insert()` | `insert(list: list, index: int, item: any) -> void` | Insert at index |
| `remove()` | `remove(list: list, item: any) -> bool` | Remove first match |
| `range()` | `range(start: int, end: int, step: int) -> list` | Generate range |
| `sort()` | `sort(list: list) -> void` | Sort in-place |
| `reverse()` | `reverse(list: list) -> void` | Reverse in-place |
| `keys()` | `keys(dict: dict) -> list` | Get all keys |
| `values()` | `values(dict: dict) -> list` | Get all values |
| `clone()` | `clone(collection: any) -> any` | Shallow copy |

---

## Type Conversion

### Overview

Conversion functions transform values between types.

| Function | Signature | Description |
|----------|-----------|-------------|
| `to_int()` | `to_int(value: any) -> int` | Convert to integer |
| `to_float()` | `to_float(value: any) -> float` | Convert to float |
| `to_string()` | `to_string(value: any) -> string` | Convert to string |
| `to_bool()` | `to_bool(value: any) -> bool` | Convert to boolean |

**Examples:**

```javascript
// String to number
let age = to_int("25");
let pi = to_float("3.14");

// Number to string
let text = "Age: " + to_string(age);

// Boolean conversion
println(to_bool(1));      // Output: true
println(to_bool(0));      // Output: false
println(to_bool(""));     // Output: false
println(to_bool("text")); // Output: true
```

---

## File I/O

### Overview

File operations for reading and writing data.

| Function | Signature | Description |
|----------|-----------|-------------|
| `read_file()` | `read_file(path: string) -> string` | Read entire file |
| `write_file()` | `write_file(path: string, content: string) -> bool` | Write file |
| `append_file()` | `append_file(path: string, content: string) -> bool` | Append to file |

**Examples:**

```javascript
// Read file
let content = read_file("data.txt");
println(content);

// Write file
let success = write_file("output.txt", "Hello, World!");
if (success) {
    println("File written successfully");
}

// Append to log
append_file("log.txt", "New entry\n");
```

---

## System Functions

### Overview

System-level operations and environment access.

| Function | Signature | Description |
|----------|-----------|-------------|
| `exit()` | `exit(code: int) -> void` | Exit program |
| `env()` | `env(name: string) -> string` | Get environment variable |
| `platform()` | `platform() -> string` | Get OS name |
| `version()` | `version() -> string` | Get ProXPL version |
| `exec()` | `exec(command: string) -> string` | Execute shell command |

**Examples:**

```javascript
// Get environment variable
let path = env("PATH");
println(path);

// Platform detection
if (platform() == "windows") {
    println("Running on Windows");
}

// Version check
println("ProXPL version:", version());

// Exit with code
if (error) {
    exit(1);
}
```

---

## Error Handling

### Invalid Input Handling

The ProXPL standard library handles invalid inputs gracefully:

**Type Errors:**
```javascript
// Runtime error: Type mismatch
sqrt("hello");  // Error: Expected number, got string
abs([1, 2, 3]); // Error: Expected number, got list
```

**Range Errors:**
```javascript
// Runtime error: Out of range
substr("hello", -1, 5);  // Error: Negative index
charAt("hello", 10);     // Error: Index out of bounds
```

**Domain Errors:**
```javascript
// Runtime error: Invalid domain
sqrt(-1);        // Error: sqrt of negative number
log(-5, 10);     // Error: log of negative number
```

**Null/Undefined Errors:**
```javascript
// Runtime error: Null reference
len(null);       // Error: Cannot get length of null
toUpper(null);   // Error: Cannot convert null to string
```

### Best Practices

1. **Validate inputs** before calling functions
2. **Use try-catch** for error-prone operations
3. **Check types** with `typeOf()` when needed
4. **Handle edge cases** explicitly

**Example:**

```javascript
func safeSqrt(x) {
    if (typeOf(x) != "int" && typeOf(x) != "float") {
        println("Error: Expected number");
        return null;
    }
    if (x < 0) {
        println("Error: Cannot sqrt negative number");
        return null;
    }
    return sqrt(x);
}

let result = safeSqrt(16);
if (result != null) {
    println("Result:", result);
}
```

---

## Quick Reference

### Function Index

**Core:** `print`, `println`, `input`, `typeOf`, `assert`

**Math:** `abs`, `sqrt`, `floor`, `ceil`, `round`, `pow`, `min`, `max`, `sin`, `cos`, `tan`, `log`, `exp`, `random`, `randint`

**String:** `len`, `substr`, `toUpper`, `toLower`, `trim`, `split`, `join`, `replace`, `startsWith`, `endsWith`, `contains`, `indexOf`, `charAt`

**Collections:** `push`, `pop`, `insert`, `remove`, `range`, `sort`, `reverse`, `keys`, `values`, `clone`

**Conversion:** `to_int`, `to_float`, `to_string`, `to_bool`

**File I/O:** `read_file`, `write_file`, `append_file`

**System:** `exit`, `env`, `platform`, `version`, `exec`

---

<p align="center">
  <b>ProXPL Standard Library - Efficient, Simple, Powerful</b>
</p>
