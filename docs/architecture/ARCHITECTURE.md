# ProXPL Architecture Guide

## Table of Contents

1. [System Overview](#system-overview)
2. [Compiler Pipeline](#compiler-pipeline)
3. [Component Details](#component-details)
4. [Bytecode Format](#bytecode-format)
5. [Runtime System](#runtime-system)
6. [Type System](#type-system)
7. [Memory Management](#memory-management)
8. [Standard Library](#standard-library)
9. [Extension Points](#extension-points)

---

## System Overview

ProXPL is structured as a **bytecode compiler and stack-based virtual machine**. The architecture follows a classic three-phase design:

```
SOURCE CODE → COMPILE → BYTECODE → EXECUTE
```

### Design Principles

1. **Simplicity**: Clean separation of concerns
2. **Clarity**: Readable, maintainable code
3. **Performance**: Bytecode compilation for speed
4. **Extensibility**: Easy to add features
5. **Debuggability**: Built-in debugging support

---

## Compiler Pipeline

### Phase 1: Lexical Analysis (Lexer/Scanner)

**File**: `src/lexer/scanner.c`, `include/scanner.h`

The lexer tokenizes source code into a stream of tokens.

#### Key Structures
```c
typedef enum {
    // Literals
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    
    // Keywords
    TOKEN_LET, TOKEN_CONST, TOKEN_FUNC, TOKEN_CLASS,
    TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE, TOKEN_FOR,
    TOKEN_TRUE, TOKEN_FALSE, TOKEN_NULL,
    // ... 45 keywords total
    
    // Operators
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH,
    TOKEN_EQUAL_EQUAL, TOKEN_BANG_EQUAL,
    // ... 42 operators total
    
    // Structural
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,
    TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_DOT,
    TOKEN_COLON, TOKEN_ARROW,
    
    TOKEN_EOF, TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    const char *start;
    int length;
    int line;
    int column;
} Token;
```

#### Algorithm
- **Input**: Source code string
- **Process**: 
  - Character-by-character scanning
  - Identifier/keyword recognition
  - Number parsing (int and float)
  - String literal handling (with escape sequences)
  - Comment skipping
- **Output**: Token stream

#### Error Handling
- Line and column tracking
- Meaningful error messages
- Graceful recovery for continued scanning

---

### Phase 2: Parsing & AST Construction

**Files**: `src/parser/parser.c`, `src/parser/ast.c`, `include/parser.h`, `include/ast.h`

The parser builds an Abstract Syntax Tree (AST) from tokens.

#### Parsing Strategy
- **Type**: Recursive descent parser
- **Approach**: Predictive LL(1) with single-token lookahead
- **Precedence**: Operator precedence climbing for expressions

#### Key AST Node Types

```c
typedef enum {
    EXPR_LITERAL,      // 42, "hello", true, null
    EXPR_IDENTIFIER,   // variable names
    EXPR_BINARY,       // a + b, x * y
    EXPR_UNARY,        // -x, !flag
    EXPR_ASSIGNMENT,   // x = 5
    EXPR_CALL,         // func(arg1, arg2)
    EXPR_INDEX,        // arr[i], dict["key"]
    EXPR_MEMBER,       // obj.field
    EXPR_LIST,         // [1, 2, 3]
    EXPR_DICT,         // {"key": value}
    EXPR_LAMBDA,       // func(x) { return x * 2; }
    // ... more expression types
} ExprType;

typedef struct Expr {
    ExprType type;
    int line;
    union {
        struct { Value value; } literal;
        struct { const char *name; } identifier;
        struct { struct Expr *left; TokenType op; struct Expr *right; } binary;
        struct { struct Expr *expr; TokenType op; } unary;
        // ... more variants
    } as;
} Expr;
```

#### Precedence Table (Lowest to Highest)
1. Assignment (`=`)
2. Logical OR (`||`)
3. Logical AND (`&&`)
4. Equality (`==`, `!=`)
5. Comparison (`<`, `>`, `<=`, `>=`)
6. Bitwise OR (`|`)
7. Bitwise XOR (`^`)
8. Bitwise AND (`&`)
9. Shift (`<<`, `>>`)
10. Term (`+`, `-`)
11. Factor (`*`, `/`, `%`)
12. Power (`**`)
13. Unary (`!`, `-`, `~`)
14. Postfix (call, index, member access)

#### Statement Types

```c
typedef enum {
    STMT_EXPR,         // expression statement
    STMT_PRINT,        // print expr;
    STMT_LET,          // let x = value;
    STMT_CONST,        // const X = value;
    STMT_BLOCK,        // { ... }
    STMT_IF,           // if (...) ... else ...
    STMT_WHILE,        // while (...) ...
    STMT_FOR,          // for (init; cond; update) ...
    STMT_FUNCTION,     // func name(...) { ... }
    STMT_CLASS,        // class Name { ... }
    STMT_RETURN,       // return value;
    STMT_BREAK,        // break;
    STMT_CONTINUE,     // continue;
    // ... more statement types
} StmtType;
```

---

### Phase 3: Semantic Analysis & Type Checking

**File**: `src/parser/type_checker.c`, `include/type_checker.h`

The type checker performs semantic analysis and type validation.

#### Responsibilities
1. **Symbol Resolution**: Map identifiers to declarations
2. **Type Inference**: Infer types where not explicitly stated
3. **Type Checking**: Validate type compatibility
4. **Scope Management**: Track variable and function scopes
5. **Error Detection**: Find type mismatches, undefined variables, etc.

#### Type Representation

```c
typedef enum {
    TYPE_UNKNOWN,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_NULL,
    TYPE_LIST,
    TYPE_DICT,
    TYPE_SET,
    TYPE_FUNCTION,
    TYPE_CLASS,
    TYPE_NATIVE_FUNC,
    TYPE_OBJECT,
} ProXPLType;

typedef struct {
    ProXPLType tag;
    // Additional metadata for complex types
    // e.g., function signature, class definition
} Type;
```

#### Scope Chain

```
┌─────────────────────────┐
│   Global Scope          │
│  (built-in functions,   │
│   standard library)     │
└────────────┬────────────┘
             │
┌────────────▼────────────┐
│   Module Scope          │
│  (module-level vars)    │
└────────────┬────────────┘
             │
┌────────────▼────────────┐
│   Function Scope        │
│  (local variables)      │
└────────────┬────────────┘
             │
┌────────────▼────────────┐
│   Block Scope           │
│  (if/while/for blocks)  │
└─────────────────────────┘
```

#### Type Inference Rules

```
x = 42              → infer TYPE_INT
y = 3.14            → infer TYPE_FLOAT
z = "hello"         → infer TYPE_STRING
flag = true         → infer TYPE_BOOL
arr = [1, 2, 3]     → infer TYPE_LIST
dict = {"a": 1}     → infer TYPE_DICT
fn = func(x) {...}  → infer TYPE_FUNCTION
```

---

### Phase 4: Bytecode Compilation

**Files**: `src/runtime/compiler.c`, `src/runtime/chunk.c`, `include/compiler.h`, `include/chunk.h`

The compiler translates the AST into bytecode.

#### Bytecode Instructions (OpCode)

```c
typedef enum {
    // Stack manipulation
    OP_CONSTANT,        // Push constant onto stack
    OP_NIL,             // Push nil
    OP_TRUE,            // Push true
    OP_FALSE,           // Push false
    
    // Unary operations
    OP_NOT,             // Logical NOT
    OP_NEGATE,          // Arithmetic negation
    OP_BITWISE_NOT,     // Bitwise NOT
    
    // Binary operations
    OP_ADD,             // Addition
    OP_SUBTRACT,        // Subtraction
    OP_MULTIPLY,        // Multiplication
    OP_DIVIDE,          // Division
    OP_MODULO,          // Modulo
    OP_POWER,           // Exponentiation
    OP_EQUAL,           // ==
    OP_NOT_EQUAL,       // !=
    OP_LESS,            // <
    OP_LESS_EQUAL,      // <=
    OP_GREATER,         // >
    OP_GREATER_EQUAL,   // >=
    OP_AND,             // &&
    OP_OR,              // ||
    OP_BITWISE_AND,     // &
    OP_BITWISE_OR,      // |
    OP_BITWISE_XOR,     // ^
    OP_LEFT_SHIFT,      // <<
    OP_RIGHT_SHIFT,     // >>
    
    // Variable management
    OP_DEFINE_GLOBAL,   // Define global variable
    OP_DEFINE_LOCAL,    // Define local variable
    OP_GET_GLOBAL,      // Get global variable
    OP_GET_LOCAL,       // Get local variable
    OP_SET_GLOBAL,      // Set global variable
    OP_SET_LOCAL,       // Set local variable
    
    // Control flow
    OP_JUMP,            // Unconditional jump
    OP_JUMP_IF_FALSE,   // Jump if top of stack is false
    OP_JUMP_IF_TRUE,    // Jump if top of stack is true
    
    // Functions
    OP_CALL,            // Function call
    OP_RETURN,          // Function return
    
    // Collections
    OP_BUILD_LIST,      // Build list from stack
    OP_BUILD_DICT,      // Build dict from stack
    OP_INDEX_GET,       // Index access
    OP_INDEX_SET,       // Index assignment
    
    // Object-Oriented
    OP_NEW,             // Create new instance
    OP_METHOD_CALL,     // Call method
    OP_GET_PROPERTY,    // Get property
    OP_SET_PROPERTY,    // Set property
    
    // Misc
    OP_PRINT,           // Print top of stack
    OP_POP,             // Pop top of stack

    // COP (Context-Oriented Programming)
    OP_CONTEXT,         // Define context
    OP_LAYER,           // Define layer
    OP_ACTIVATE,        // Activate context
    OP_END_ACTIVATE,    // Deactivate context
} OpCode;
```

#### Chunk Structure

```c
typedef struct {
    uint8_t *code;           // Bytecode instructions
    int *lines;              // Line numbers for each instruction
    ValueArray constants;    // Constant pool
    int capacity;
    int count;
} Chunk;
```

#### Example Compilation

```javascript
// Source code
func add(a, b) {
    return a + b;
}
let result = add(5, 3);

// Compiled bytecode
OP_CONSTANT <func>          // Load function
OP_DEFINE_GLOBAL "add"      // Define function
OP_CONSTANT 5               // Push 5
OP_CONSTANT 3               // Push 3
OP_CALL 2                   // Call add with 2 args
OP_DEFINE_GLOBAL "result"   // Store result
```

#### Optimization Passes

1. **Constant Folding**: Compile-time arithmetic
   ```
   5 + 3  →  8  (single OP_CONSTANT)
   ```

2. **Dead Code Elimination**: Remove unreachable code
   ```
   if (false) { ... }  →  (removed)
   ```

3. **Peephole Optimization**: Small instruction sequences
   ```
   OP_CONSTANT nil followed by OP_CONSTANT nil  →  OP_NIL OP_NIL
   ```

---

## Component Details

### 1. Lexer (`scanner.c`)

**Responsibility**: Convert source text to tokens

**Key Functions**:
- `initScanner(Scanner *scanner, const char *source)` - Initialize lexer
- `nextToken(Scanner *scanner)` - Get next token
- `peek(Scanner *scanner)` - Look at current char without consuming
- `peekNext(Scanner *scanner)` - Look ahead one more char

**State Management**:
- Current position in source
- Current line and column
- Lookahead buffer

### 2. Parser (`parser.c`)

**Responsibility**: Convert tokens to AST

**Key Functions**:
- `initParser(Parser *parser, Token *tokens, int count)` - Initialize parser
- `parse(Parser *parser)` - Parse program
- `parseDeclaration(Parser *parser)` - Parse declaration
- `parseStatement(Parser *parser)` - Parse statement
- `parseExpression(Parser *parser)` - Parse expression
- `error(Parser *parser, Token token, const char *msg)` - Error handling

**Pattern**: Recursive descent with precedence climbing for expressions

### 3. Type Checker (`type_checker.c`)

**Responsibility**: Semantic analysis and type validation

**Key Functions**:
- `initTypeChecker()` - Initialize type checker
- `check(ASTNode *node)` - Type check node
- `infer(ASTNode *node)` - Infer type
- `unify(Type a, Type b)` - Unify two types
- `addSymbol(const char *name, Type type)` - Add to symbol table
- `lookupSymbol(const char *name)` - Look up symbol

**Symbol Table**:
- Hashtable mapping identifiers to types
- Scope stack for nested scopes
- Support for shadowing (inner scopes override outer)

### 4. Compiler (`compiler.c`)

**Responsibility**: Convert AST to bytecode

**Key Functions**:
- `initCompiler()` - Initialize compiler
- `compile(ASTNode *node)` - Compile node
- `emitByte(uint8_t byte)` - Emit single byte
- `emitConstant(Value value)` - Emit constant
- `emitJump(uint8_t opcode)` - Emit jump instruction

**Process**:
1. Walk the AST
2. Emit corresponding bytecode
3. Maintain constant pool
4. Track line numbers for debugging

---

## Bytecode Format

### Instruction Format

Each instruction is 1-3 bytes:

```
[OpCode: 1 byte] [Operand: 0-2 bytes]
```

### Example Instructions

```c
OP_CONSTANT <index>         // 1 + 2 bytes (opcode + const index)
OP_ADD                      // 1 byte (no operand)
OP_JUMP <offset>            // 1 + 2 bytes (opcode + jump offset)
OP_CALL <arg_count>         // 1 + 1 byte (opcode + arg count)
```

### Constant Pool

The constant pool stores:
- Numeric literals (int, float)
- String literals
- Function objects
- Class definitions

---

## Runtime System

### Virtual Machine (VM)

**File**: `src/runtime/vm.c`, `include/vm.h`

The VM executes bytecode instructions.

#### Stack-Based Design

```
┌─────────────────────────────────────┐
│         Stack (grows upward)        │
│  ┌─────────────────────────────────┐│
│  │  [sp] → (top, used for ops)     ││
│  ├─────────────────────────────────┤│
│  │  ...                            ││
│  ├─────────────────────────────────┤│
│  │  [stack[0]] (bottom)            ││
│  └─────────────────────────────────┘│
└─────────────────────────────────────┘
```

#### Execution Loop

```c
for (;;) {
    uint8_t instruction = READ_BYTE();
    
    switch (instruction) {
        case OP_CONSTANT:
            push(vm, vm->chunk->constants.values[READ_BYTE()]);
            break;
        case OP_ADD:
            {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, NUMBER_VAL(AS_NUMBER(a) + AS_NUMBER(b)));
            }
            break;
        // ... more cases
        case OP_RETURN:
            return INTERPRET_OK;
    }
}
```

#### Call Stack

Function calls use a separate call stack:

```c
typedef struct {
    ObjFunction *function;
    uint8_t *ip;              // Instruction pointer
    Value *slots;             // Local variables
} CallFrame;

typedef struct {
    CallFrame frames[FRAMES_MAX];
    int frameCount;
} VM;
```

### Value Representation

**File**: `src/runtime/value.c`, `include/value.h`

```c
typedef enum {
    VAL_BOOL,
    VAL_NULL,
    VAL_NUMBER,
    VAL_OBJ,
} ValueType;

typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
        Obj *obj;
    } as;
} Value;
```

**Macros for efficient type checking:**
```c
#define IS_BOOL(value)      ((value).type == VAL_BOOL)
#define IS_NULL(value)      ((value).type == VAL_NULL)
#define IS_NUMBER(value)    ((value).type == VAL_NUMBER)
#define IS_OBJ(value)       ((value).type == VAL_OBJ)

#define AS_BOOL(value)      ((value).as.boolean)
#define AS_NUMBER(value)    ((value).as.number)
#define AS_OBJ(value)       ((value).as.obj)

#define BOOL_VAL(value)     ((Value){VAL_BOOL, {.boolean = value}})
#define NULL_VAL            ((Value){VAL_NULL, {.number = 0}})
#define NUMBER_VAL(value)   ((Value){VAL_NUMBER, {.number = value}})
#define OBJ_VAL(object)     ((Value){VAL_OBJ, {.obj = (Obj*)object}})
```

### Object System

**File**: `src/runtime/object.c`, `include/object.h`

```c
typedef enum {
    OBJ_STRING,
    OBJ_FUNCTION,
    OBJ_CLOSURE,
    OBJ_CLASS,
    OBJ_INSTANCE,
    OBJ_BOUND_METHOD,
    OBJ_LIST,
    OBJ_DICT,
    OBJ_SET,
    OBJ_CONTEXT,
    OBJ_LAYER,
} ObjType;

typedef struct Obj {
    ObjType type;
    struct Obj *next;  // For linked list in memory manager
} Obj;

// String interning for efficiency
typedef struct {
    ObjString **strings;
    int count;
    int capacity;
} StringPool;
```

#### Object Types

**ObjString**: Interned strings
```c
typedef struct {
    Obj obj;
    int length;
    char *chars;
    uint32_t hash;
} ObjString;
```

**ObjFunction**: Function definition
```c
typedef struct {
    Obj obj;
    int arity;
    Chunk chunk;
    ObjString *name;
} ObjFunction;
```

**ObjClosure**: Function with captured variables
```c
typedef struct {
    Obj obj;
    ObjFunction *function;
    UpValue **upvalues;
    int upvalueCount;
} ObjClosure;
```

**ObjClass**: Class definition
```c
typedef struct {
    Obj obj;
    ObjString *name;
    Table methods;  // Method table
    ObjClass *superclass;
} ObjClass;
```

**ObjInstance**: Class instance
```c
typedef struct {
    Obj obj;
    ObjClass *klass;
    Table fields;  // Instance fields
} ObjInstance;
```

---

## Type System

### Type Hierarchy

```
Any
├── Primitive
│   ├── Null
│   ├── Bool
│   ├── Int
│   ├── Float
│   └── String
├── Collection
│   ├── List<T>
│   ├── Dict<K, V>
│   └── Set<T>
├── Function
│   └── (ParamTypes) → ReturnType
└── Object
    ├── Class
    └── Instance
```

### Type Inference Algorithm

Hindley-Milner style inference with extensions:

```
Algorithm: infer(expr)
  match expr:
    Literal(n: number) → Int | Float
    Literal(s: string) → String
    Literal(b: bool) → Bool
    Literal(null) → Null
    
    Var(name) → lookup(name)
    
    BinOp(e1, op, e2):
      t1 = infer(e1)
      t2 = infer(e2)
      return resultType(op, t1, t2)
    
    Call(func, args):
      tf = infer(func)
      if tf = (T1, ..., Tn) → R:
        check arg types match T1..Tn
        return R
    
    List(elems):
      if empty: List<Unknown>
      t = infer(elems[0])
      check all elems have type t
      return List<t>
    
    Lambda(params, body):
      infer params
      tr = infer(body)
      return (param_types) → tr
```

### Type Checking Rules

**Assignment Compatibility**:
```
Int    compatible with: Int, Float
Float  compatible with: Float
Bool   compatible with: Bool
String compatible with: String
Null   compatible with: Any (nullable types)
List<T> compatible with: List<T>
```

**Function Call**:
```
func f(x: T1): T2
call f(arg) ✓  if typeof(arg) compatible with T1
            ✗  otherwise type error
```

---

## Memory Management

### Allocation Strategy

**File**: `src/runtime/memory.c`, `include/memory.h`

```c
void *reallocate(void *pointer, size_t oldSize, size_t newSize) {
    totalAllocated += (newSize - oldSize);
    
    if (totalAllocated > GC_THRESHOLD) {
        collectGarbage();
    }
    
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }
    
    void *result = realloc(pointer, newSize);
    if (result == NULL) exit(1);
    return result;
}

#define ALLOCATE(type, count) \
    (type*)reallocate(NULL, 0, sizeof(type) * (count))

#define FREE(type, pointer) \
    reallocate(pointer, sizeof(type), 0)
```

### Garbage Collection (GC)

Mark-and-sweep garbage collector:

```
1. MARK PHASE:
   - Start from roots (globals, stack, current frame)
   - Follow all reachable objects
   - Mark reachable objects

2. SWEEP PHASE:
   - Iterate through all objects
   - Free unmarked objects
   - Unmark marked objects for next cycle

3. THRESHOLD:
   - GC triggered when allocated memory exceeds threshold
   - Threshold doubles after each collection
```

### String Interning

All strings are interned for memory efficiency:

```
intern("hello") → returns ObjString*
intern("hello") → returns same ObjString* (cached)
```

Interned strings allow:
- O(1) string comparison (pointer equality)
- Memory sharing for duplicate strings
- Fast hashing for dictionary keys

---

## Standard Library

### Structure

```
stdlib/
├── io_native.c          # I/O functions
│   ├── print(value)
│   ├── input(prompt)
│   ├── open(filename, mode)
│   └── read/write functions
│
├── math_native.c        # Math functions
│   ├── sqrt(x)
│   ├── sin(x), cos(x), tan(x)
│   ├── floor(x), ceil(x)
│   ├── abs(x)
│   └── more...
│
├── string_native.c      # String functions
│   ├── len(str)
│   ├── substring(str, start, len)
│   ├── toUpper(str), toLower(str)
│   ├── split(str, delim)
│   └── more...
│
├── convert_native.c     # Type conversion
│   ├── toInt(value)
│   ├── toFloat(value)
│   ├── toString(value)
│   └── more...
│
└── system_native.c      # System functions
    ├── exit(code)
    ├── system(cmd)
    ├── getEnv(name)
    └── more...
```

### Native Function Interface

```c
typedef Value (*NativeFn)(int argCount, Value *args);

typedef struct {
    ObjString *name;
    NativeFn function;
} NativeFunction;
```

### Function Registration

```c
void registerAll(VM *vm) {
    registerIOFunctions(vm);
    registerMathFunctions(vm);
    registerStringFunctions(vm);
    // ...
}
```

---

## Extension Points

### Adding New Features

#### 1. New Operator

1. Add token type: `TokenType.NEW_OP`
2. Add to lexer keywords/operators
3. Add precedence in parser
4. Create AST node type
5. Add type checking rules
6. Emit bytecode in compiler
7. Implement in VM

#### 2. New Statement Type

1. Create AST node type
2. Add parse function
3. Add type checking
4. Add compiler code generation
5. Add VM execution (if needed)

#### 3. New Built-in Function

1. Write C function: `Value myFunc(int argc, Value *argv)`
2. Register in `stdlib_core.c`
3. Add to type registry
4. Document in API

---

## Performance Considerations

### VM Performance

**Stack-based approach**:
- Pro: Simple, minimal memory footprint
- Con: More instructions for complex operations

**Bytecode caching**: Not implemented (possible future optimization)

**Function inlining**: Could optimize hot paths

### Memory Efficiency

1. **String Interning**: All strings cached
2. **Constant Pool**: Literals shared at compile time
3. **Intern

   ed Keywords**: Reused string representations

### Optimization Opportunities

1. **JIT Compilation**: Translate bytecode to native code
2. **Inline Caching**: Cache method lookups
3. **Object Shape Optimization**: Optimize instance layout
4. **Escape Analysis**: Stack allocate when possible

---

## Debugging Support

### Debug Information

Maintained during compilation:
- Line numbers for each instruction
- Variable names and types
- Function names and arity

### Debug Functions

**File**: `src/runtime/debug.c`

```c
void disassembleChunk(Chunk *chunk, const char *name);
void disassembleInstruction(Chunk *chunk, int offset);
```

### Example Disassembly

```
== fibonacci ==
0000    1 OP_CONSTANT           0 '2'
0002      OP_EQUAL
0003      OP_JUMP_IF_FALSE      → 0009
0006      OP_CONSTANT           1 '0'
0008      OP_RETURN
0009    2 OP_GET_LOCAL           0 'n'
0011      OP_CONSTANT           2 '1'
0013      OP_SUBTRACT
0014      OP_CALL                1
```

---

This architecture provides a solid foundation for a fast, extensible language implementation. The clear separation between compilation and runtime execution allows for future optimizations while maintaining simplicity and correctness.
