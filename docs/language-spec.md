# ProXPL Language Specification

This document defines the core specification of the ProXPL programming language.

## A. Reserved Keywords (45)

ProXPL reserves the following 45 keywords. They cannot be used as identifiers.

| Category | Keywords |
|----------|----------|
| **Declarations** | `let`, `const`, `func`, `class`, `enum`, `struct`, `interface`, `abstract` |
| **Control Flow** | `if`, `else`, `while`, `for`, `switch`, `case`, `default`, `break`, `continue`, `return`, `try`, `catch`, `finally`, `throw`, `defer` |
| **Modules** | `import`, `from`, `export`, `as`, `use` |
| **Values** | `true`, `false`, `null`, `void` |
| **OOP/Types** | `this`, `super`, `static`, `public`, `private`, `protected`, `extends`, `implements` |
| **Async** | `async`, `await` |
| **Operators/Runtime** | `in`, `is`, `typeof`, `native` |

## B. Data Types (12)

ProXPL supports exactly 12 core data types.

1.  **Int**: Integer numbers (e.g., `42`, `-10`).
2.  **Float**: Floating-point numbers (e.g., `3.14`).
3.  **Bool**: Boolean values (`true`, `false`).
4.  **String**: Text enclosed in quotes (e.g., `"Hello"`).
5.  **List**: Ordered collection (e.g., `[1, 2, 3]`).
6.  **Dict**: Key-value pairs (e.g., `{"key": "value"}`).
7.  **Set**: Unique collection (e.g., `Set([1, 2])`).
8.  **Null**: Represents absence of value (`null`).
9.  **Function**: Callable code block.
10. **NativeFunc**: Internal VM function.
11. **Bytes**: Raw byte sequence.
12. **Error**: Exception object.

## C. Operators (42)

ProXPL defines 42 operators.

### Arithmetic (6)
`+`, `-`, `*`, `/`, `%`, `**`

### Comparison (6)
`==`, `!=`, `<`, `>`, `<=`, `>=`

### Logical (3)
`&&`, `||`, `!`

### Bitwise (6)
`&`, `|`, `^`, `~`, `<<`, `>>`

### Assignment (12)
`=`, `+=`, `-=`, `*=`, `/=`, `%=`, `**=`
`&=`, `|=`, `^=`, `<<=`, `>>=`

### Special (4)
- `.`: Member access
- `?.`: Optional chaining
- `=>`: Arrow function
- `..`: Range

### Ternary / Null (3)
- `?`: Question part of ternary
- `:`: Colon part of ternary
- `??`: Null coalescing

### Increment / Decrement (2)
`++`, `--`

## D. Built-in Functions (75)

The standard library includes 75 built-in functions grouped by module.

### IO (5)
- `print(...args) -> Void`: Print to console.
- `input(prompt) -> String`: Read input from console.
- `read_file(path) -> String`: Read file content.
- `write_file(path, content) -> Bool`: Write to file.
- `append_file(path, content) -> Bool`: Append to file.

### Math (15)
- `abs(n)`, `ceil(n)`, `floor(n)`, `round(n, digits)`, `max(list)`, `min(list)`
- `pow(base, exp)`, `sqrt(n)`, `log(n, base)`, `exp(n)`
- `sin(n)`, `cos(n)`, `tan(n)`
- `random()`, `randint(min, max)`

### String (15)
- `len(s)`, `upper(s)`, `lower(s)`, `trim(s)`
- `split(s, sep)`, `join(sep, list)`, `replace(s, old, new)`
- `startswith(s, sub)`, `endswith(s, sub)`, `contains(haystack, needle)`
- `index_of(s, sub)`, `substring(s, start, end)`, `char_at(s, index)`
- `format(fmt, ...args)`, `to_bytes(s)`

### Collections (15)
- `push(list, item)`, `pop(list)`, `insert(list, idx, item)`, `remove(list, item)`
- `range(start, end, step)`, `sort(list)`, `reverse(list)`
- `keys(dict)`, `values(dict)`, `entries(dict)`, `contains_key(dict, key)`
- `merge(d1, d2)`, `clear(col)`, `clone(col)`, `deep_clone(col)`

### DateTime (5)
- `now()`, `timestamp()`, `sleep(seconds)`
- `format_date(ts, fmt)`, `parse_date(str, fmt)`

### System (5)
- `exit(code)`, `env(var_name)`, `platform()`, `version()`, `exec(cmd)`

### Convert (10)
- `to_int(v)`, `to_float(v)`, `to_string(v)`, `to_bool(v)`
- `to_list(v)`, `to_dict(v)`
- `to_hex(n)`, `to_bin(n)`
- `parse_json(str)`, `stringify_json(obj)`

### Runtime (5)
- `type(v)`, `assert(cond, msg)`, `id(v)`, `hash(v)`, `is_instance(v, type)`

## E. Modules and Packages

ProXPL provides a module system to organize code and manage dependencies.

### 1. The `use` Keyword
Modules are imported using the `use` keyword followed by the module identifier or path.

```prox
use math;
use utils/io;
use libA, libB;
```

### 2. Resolution Order
When resolving a module name (e.g., `use mymod`), ProXPL searches in the following order:
1.  **Current Working Directory**: Checks if `./mymod.prox` exists.
2.  **Package Directory**: Checks `packages/mymod.prox`.
3.  **Standard Library**: Checks `stdlib/mymod.prox`.

### 3. Execution Model
-   Modules are executed exactly once per runtime session.
-   Subsequent imports of the same module do not re-execute the code.
-   Circular imports are detected and result in a runtime error to prevent infinite recursion.

### 4. Package Manager (PRM)
The **ProX Repository Manager (PRM)** is the official package handling tool.
It manages dependencies in the `packages/` directory using a `registry.json` manifest.
