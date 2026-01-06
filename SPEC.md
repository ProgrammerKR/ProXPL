# ProX Programming Language Specification

**Version:** 1.0.0-alpha  
**Status:** Draft  
**Date:** January 2026  
**Author:** ProXPL Design Team  

---

## 1. Introduction

### 1.1. Purpose
This document defines the formal specification of the **ProX Programming Language** (ProXPL). It serves as the authoritative reference for the language's syntax, semantics, type system, and execution model. This specification is intended for compiler implementers, tool developers, and advanced users requiring precise knowledge of the language behavior.

### 1.2. Scope
This specification describes the core language features, including lexical structure, grammar, type system, memory model, and standard library interfaces. It distinguishes between mandatory language features and implementation-specific behaviors.

### 1.3. Goals
ProXPL is designed with the following key objectives:
- **Performance**: Predictable performance suitable for systems programming and backend development.
- **Clarity**: A clean, readable syntax inspired by Python and JavaScript to reduce cognitive load.
- **Scalability**: Support for large codebases through a robust module system and static typing.
- **Safety**: Compile-time type safety to eliminate common classes of runtime errors.

### 1.4. Non-Goals
- **Undefined Behavior**: The language aims to minimize undefined behavior; however, unsafe interfaces (FFI) are provided for system-level access.
- **Dynamic Typing**: While type inference is supported, ProXPL is fundamentally statically typed.

---

## 2. Language Overview

ProXPL is a statically-typed, general-purpose programming language. It combines the expressive high-level syntax of scripting languages with the performance characteristics of compiled languages.

### 2.1. Design Philosophy
- **Explicit is better than implicit**, except where type inference improves readability without sacrificing safety.
- **Composition over inheritance**, favoring interfaces and traits (planned) over deep class hierarchies.
- **Batteries included**, providing a comprehensive standard library for common tasks.

### 2.2. Intended Use Cases
- specific Systems tooling and CLI applications.
- High-performance web servers and network services.
- Embedded logic and game scripting.
- Cross-platform application development.

---

## 3. Lexical Structure

ProXPL source code is encoded in UTF-8.

### 3.1. Formatting
The language is free-form. Whitespace (spaces, tabs, newlines) acts as a token separator but is otherwise ignored, except in string literals. Semicolons (`;`) are recommended to terminate statements but arguments may vary on strict enforcement in future versions (current grammar requires valid statement termination).

### 3.2. Identifiers
Identifiers name variables, types, functions, and other entities.
- **Pattern**: `[a-zA-Z_][a-zA-Z0-9_]*`
- **Case Sensitivity**: Identifiers are case-sensitive. `myVar` and `myvar` are distinct.

### 3.3. Keywords
The following tokens are reserved and cannot be used as identifiers:

| Declaration | Control Flow | Types/Values | OOP/Async | Modules |
| :--- | :--- | :--- | :--- | :--- |
| `let` | `if` | `true` | `class` | `use` |
| `const` | `else` | `false` | `new` | `import` |
| `func` | `while` | `null` | `this` | `from` |
| `native` | `for` | `void` | `super` | `as` |
| `extern` | `return` | `int` | `extends` | |
| | `break` | `float` | `interface` | |
| | `continue` | `bool` | `static` | |
| | `switch` | `string` | `async` | |
| | `case` | | `await` | |
| | `default` | | | |
| | `try` | | | |
| | `catch` | | | |
| | `throw` | | | |

### 3.4. Literals

- **Integer Literals**: Decimal (`123`), Hexadecimal (`0x7B`), Binary (`0b1111011`).
- **Floating-Point Literals**: Decimal with fractional part or exponent (`3.14`, `1.0e-5`).
- **Boolean Literals**: `true`, `false`.
- **String Literals**: Enclosed in double quotes (`"hello"`). access escape sequences: `\n`, `\t`, `\r`, `\"`, `\\`.
- **Null Literal**: `null`.

### 3.5. Comments
- **Line Comments**: Begin with `//` and continue to end of line.
- **Block Comments**: Begin with `/*` and end with `*/`. Nesting is not supported.

---

## 4. Syntax and Grammar

The grammar is defined using Extended Backus-Naur Form (EBNF) notation.

### 4.1. Declarations
A program consists of a sequence of declarations.

```ebnf
program      ::= declaration* EOF
declaration  ::= funcDecl | varDecl | classDecl | statement
```

#### 4.1.1. Variable Declaration
Variables are declared using `let` (mutable) or `const` (immutable).

```ebnf
varDecl ::= "let" IDENTIFIER ( ":" type )? ( "=" expression )? ";"
          | "const" IDENTIFIER ( ":" type )? "=" expression ";"
```

#### 4.1.2. Function Declaration
Functions are declared with the `func` keyword. Parameters may optionally define types.

```ebnf
funcDecl ::= "func" IDENTIFIER "(" parameters? ")" ( ":" type )? block
parameters ::= IDENTIFIER ( ":" type )? ( "," IDENTIFIER ( ":" type )? )*
```

### 4.2. Statements
Statements execute actions or control flow.

```ebnf
statement ::= exprStmt
            | ifStmt
            | whileStmt
            | forStmt
            | returnStmt
            | block
            | tryStmt
```

- **Block**: `{ statement* }` introduces a new scope.
- **If**: `if (expr) stmt (else stmt)?`
- **While**: `while (expr) stmt`
- **For**: `for (init?; cond?; incr?) stmt`
- **Return**: `return expr? ";"`

### 4.3. Expressions
Expressions evaluate to a value. Precedence follows standard C/Java rules.

```ebnf
expression ::= assignment
assignment ::= IDENTIFIER "=" assignment
             | logicOr
logicOr    ::= logicAnd ( "||" logicAnd )*
logicAnd   ::= equality ( "&&" equality )*
equality   ::= comparison ( ( "!=" | "==" ) comparison )*
comparison ::= term ( ( ">" | ">=" | "<" | "<=" ) term )*
term       ::= factor ( ( "-" | "+" ) factor )*
factor     ::= unary ( ( "/" | "*" | "%" ) unary )*
unary      ::= ( "!" | "-" ) unary | call
call       ::= primary ( "(" arguments? ")" | "." IDENTIFIER )*
primary    ::= NUMBER | STRING | "true" | "false" | "null"
             | "(" expression ")" | IDENTIFIER
             | "[" elements? "]" | "{" members? "}"
```

---

## 5. Type System

ProXPL employs a **static type system** with extensive **type inference**. Types are checked at compile-time to ensure safety.

### 5.1. Primitive Types
The type system distinguishes logical types, though implementation representations may vary (e.g., boxing).

| Type | Description |
| :--- | :--- |
| `int` | Signed 64-bit integer (compile-time constraint). |
| `float` | 64-bit IEEE 754 floating point number. |
| `bool` | Boolean value (`true` or `false`). |
| `string` | Immutable sequence of UTF-8 characters. |
| `void` | Absence of value (return type only). |
| `any` | Dynamic type (escape hatch from static checking). |

> **Note**: In the current version 1.0 Runtime, `int` and `float` may both be represented as double-precision floats (NaN tagging), limiting accurate integer precision to 53 bits.

### 5.2. Composite Types
- **List**: Mutable, ordered collection of values (`[1, 2, 3]`). Heterogeneous if typed as `List<any>`.
- **Dictionary**: Key-value map (`{"key": "value"}`).
- **Function**: First-class function type.

### 5.3. Object Types
- **Class**: User-defined type containing fields and methods.
- **Interface**: Abstract definition of behavior (method signatures).
- **Instance**: Concrete instance of a Class.

### 5.4. Type Inference
The compiler infers types for variables initialized at declaration.
```javascript
let x = 10; // Inferred as int
let y = x;  // Inferred as int
```
Function parameters defaulting to `any` if not annotated in the current grammar, though strict mode requires explicit types.

---

## 6. Memory Model

### 6.1. Allocation
- **Values**: Primitive types (booleans, numbers, null) are typically passed by value.
- **Objects**: Complex types (strings, lists, instances) are allocated on the heap and accessed via references.

### 6.2. Garbage Collection
ProXPL manages memory automatically using a **Mark-and-Sweep Garbage Collector**.
- **Reachability**: Objects are retained as long as they are reachable from the root set (global variables, stack, upvalues).
- **Cycles**: The GC can handle reference cycles.
- **Trigger**: GC is triggered based on allocation pressure or explicit request (`std.gc.collect()`).

### 6.3. Lifetime
Variables declared in a block (`{}`) exist until the end of that block. Objects created within the block persist as long as references exist.

---

## 7. Execution Model

ProXPL defines a dual-mode execution environment.

### 7.1. Bytecode Interpretation (Default)
The source code is compiled to platform-independent **bytecode**.
- **VM**: A stack-based Virtual Machine executes the bytecode.
- **Chunk**: Bytecode is organized into chunks containing instructions and constants.
- **Performance**: Optimization passes (constant folding, dead code elimination) occur during bytecode generation.

### 7.2. Native Compilation (AOT)
ProXPL supports Ahead-of-Time (AOT) compilation via an **LLVM Backend**.
- Transforms ProXPL intermediate representation (IR) into LLVM IR.
- Compiles to native machine code for maximum performance.
- Eliminates interpretation overhead.

### 7.3. Concurrency
ProXPL integrates **asynchronous execution** directly into the runtime.
- **Async/Await**: Syntax for non-blocking operations.
- **Coroutines**: Built on top of LLVM Coroutines (or fibers in VM), allowing functions to suspend and resume state.
- **Event Loop**: Scheduler manages execution of Tasks.

---

## 8. Error Handling

### 8.1. Compile-Time Errors
The compiler detects:
- Syntax errors.
- Type mismatches (e.g., adding `bool` to `float`).
- Undefined variables or symbols.
- Argument count mismatches.

### 8.2. Runtime Errors
Runtime conditions that cannot be checked statically raise specific errors:
- **Exceptions**: Structured mechanism using `try`, `catch`, `throw`. (Roadmap v1.1)
- **Panic**: Unrecoverable system errors (e.g., Stack Overflow, Out of Memory) terminate the process.

---

## 9. Standard Library

The rigid, versioned Standard Library provides essential capabilities protected by stability guarantees.

### 9.1. Modules
- **`std.core`**: Fundamental types and assertions.
- **`std.io`**: File and console I/O.
- **`std.math`**: Mathematical functions and constants.
- **`std.sys`**: System interaction (env vars, processes).
- **`std.net`**: Networking capabilities (planned).

### 9.2. Stability
APIs within the `std` namespace follow semantic versioning. Breaking changes occur only in major version increments.

---

## 10. Foreign Function Interface (FFI)

ProXPL allows binding to native system libraries.
- **`extern` keyword**: Defines signatures for external C functions.
- **Dynamic Loading**: Libraries (`.dll`, `.so`) are loaded at runtime.
- **Type Mapping**: ProXPL types are marshaled to C-compatible types (Number → double/int, String → char*).

```javascript
extern "kernel32.dll" "Sleep" func sleep(ms);
```

---

## 11. Undefined and Implementation-Defined Behavior

### 11.1. Implementation-Defined
- **Integer Size**: Currently mapped to double-precision float (53-bit SAFE_INT). Future versions may implement strict 64-bit integers.
- **Hash Order**: Iteration order of Dictionaries is implementation-dependent (not guaranteed).

### 11.2. Undefined Behavior
- Modifying a collection while iterating over it.
- Accessing FFI pointers incorrectly.
- Relying on specific GC timing.

---

## 12. Future Evolution

ProXPL evolves through a Request for Comments (RFC) process. Breaking changes are reserved for major integer releases. The roadmap includes Generic Programming and Pattern Matching for v2.0.

***
*End of Specification*
