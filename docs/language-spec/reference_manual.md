# ProXPL Reference Manual

## SECTION A: Reserved Keywords List (45)

These keywords cannot be used as identifiers.

1.  `let` - Declare mutable variable
2.  `const` - Declare immutable constant
3.  `func` - Define function
4.  `class` - Define class
5.  `if` - Conditional block
6.  `else` - Alternative conditional block
7.  `while` - While loop trigger
8.  `for` - For loop trigger
9.  `break` - Exit current loop
10. `continue` - Skip to next loop iteration
11. `return` - Return from function
12. `switch` - Switch statement
13. `case` - Case within switch
14. `default` - Default within switch
15. `try` - Attempt dangerous code block
16. `catch` - Handle error
17. `finally` - Execute code after try/catch
18. `throw` - Raise an exception
19. `import` - Import module
20. `from` - Specify import source
21. `export` - Export symbol
22. `as` - Alias for imports/types
23. `true` - Boolean literal true
24. `false` - Boolean literal false
25. `null` - Null literal
26. `this` - Current instance reference
27. `super` - Parent instance reference
28. `static` - Static class member
29. `public` - Public access modifier
30. `private` - Private access modifier
31. `protected` - Protected access modifier
32. `extends` - Class inheritance
33. `implements` - Interface implementation
34. `interface` - Define interface/contract
35. `abstract` - Abstract class/method
36. `enum` - Enumerate type
37. `struct` - Value type structure
38. `async` - Define asynchronous function
39. `await` - Wait for async result
40. `void` - Return type for no value
41. `defer` - Defer execution until scope exit
42. `in` - Iterator operator
43. `is` - Type check operator
44. `typeof` - Get type of expression
45. `native` - Declare native binding

---

## SECTION B: Data Types (12)

ProXPL features a dynamic but strong typing system with these core primitives.

1.  **Int** (`int`)
    *   **Purpose**: 64-bit signed integer.
    *   **Literal**: `10`, `-5`, `0xFF` (hex), `0b10` (bin)
    *   **Example**: `let age = 25;`

2.  **Float** (`float`)
    *   **Purpose**: 64-bit IEEE 754 floating point.
    *   **Literal**: `3.14`, `1.0`, `2.5e-3`
    *   **Example**: `let pi = 3.14159;`

3.  **Bool** (`bool`)
    *   **Purpose**: Boolean logic value.
    *   **Literal**: `true`, `false`
    *   **Example**: `let isValid = true;`

4.  **String** (`string`)
    *   **Purpose**: UTF-8 immutable text sequence.
    *   **Literal**: `"Hello"`, `'World'`, `"Line\nBreak"`
    *   **Example**: `let msg = "Welcome";`

5.  **Char** (`char`)
    *   **Purpose**: Single UTF-8 character code point.
    *   **Literal**: `'a'`, `'\u0041'`
    *   **Example**: `let initial = 'K';`

6.  **Byte** (`byte`)
    *   **Purpose**: 8-bit unsigned integer (0-255).
    *   **Literal**: `0b11110000` (often cast from Int)
    *   **Example**: `let mask = 0xFF;`

7.  **List** (`list`)
    *   **Purpose**: Dynamic, ordered sequence of values.
    *   **Literal**: `[1, 2, "three"]`, `[]`
    *   **Example**: `let users = ["Alice", "Bob"];`

8.  **Map** (`map`)
    *   **Purpose**: Key-value hash map (dictionaries).
    *   **Literal**: `{"key": "value", "id": 1}`, `{}`
    *   **Example**: `let config = {"debug": true};`

9.  **Set** (`set`)
    *   **Purpose**: Collection of unique values.
    *   **Literal**: `{1, 2, 3}`
    *   **Example**: `let uniqueTags = {"dev", "prod"};`

10. **Func** (`func`)
    *   **Purpose**: First-class function type.
    *   **Literal**: `func(a) { return a; }`, `(a) => a`
    *   **Example**: `let callback = func() {};`

11. **Range** (`range`)
    *   **Purpose**: Represents a sequence of numbers.
    *   **Literal**: `0..10` (inclusive start, exclusive end)
    *   **Example**: `for (i in 1..5) { ... }`

12. **Null** (`null`)
    *   **Purpose**: Represents the absence of value.
    *   **Literal**: `null`
    *   **Example**: `let result = null;`

---

## SECTION C: Operators (42)

Categorized by precedence (highest to lowest).

### Special / Access (5)
1. `.` - Member access (`obj.prop`)
2. `?.` - Safe navigation (`obj?.prop` - returns null if obj is null)
3. `()` - Function call (`func()`)
4. `[]` - Index access (`arr[0]`)
5. `=>` - Arrow function (`() => {}`)

### Unary (5)
6. `++` - Increment (`i++` / `++i`)
7. `--` - Decrement (`i--` / `--i`)
8. `!` - Logical NOT (`!true`)
9. `~` - Bitwise NOT (`~0b10`)
10. `-` - Unary Negation (`-5`)

### Arithmetic (7)
11. `**` - Power (`2 ** 3`)
12. `*` - Multiplication
13. `/` - Division
14. `%` - Modulo
15. `+` - Addition
16. `-` - Subtraction
17. `..` - Range (`1..10`)

### Bitwise Shift (2)
18. `<<` - Left Shift
19. `>>` - Right Shift

### Comparison (6)
20. `<` - Less than
21. `<=` - Less or equal
22. `>` - Greater than
23. `>=` - Greater or equal
24. `==` - Equality
25. `!=` - Inequality

### Bitwise Logic (3)
26. `&` - Bitwise AND
27. `^` - Bitwise XOR
28. `|` - Bitwise OR

### Logical (2)
29. `and` - Logical AND
30. `or` - Logical OR

### Tertiary (2)
31. `??` - Null Coalescing (`val ?? default` coverage)
32. `? :` - Ternary Conditional (`cond ? true : false`)

### Assignment (10)
33. `=` - Assign
34. `+=` - Add assign
35. `-=` - Sub assign
36. `*=` - Mul assign
37. `/=` - Div assign
38. `%=` - Mod assign
39. `**=` - Pow assign
40. `&=` - Bitwise AND assign
41. `|=` - Bitwise OR assign
42. `^=` - Bitwise XOR assign

---

## SECTION D: Built-in Functions (75)

### I/O Utilities (10)
1.  `print(arg...)` - Print to stdout with newline.
2.  `input(prompt)` - Read line from stdin.
3.  `write(arg...)` - Print to stdout without newline.
4.  `open(path, mode)` - Open a file handle.
5.  `read(handle)` - Read content from file handle.
6.  `close(handle)` - Close file handle.
7.  `exists(path)` - Check if file path exists.
8.  `remove(path)` - Delete file.
9.  `mkdir(path)` - Create directory.
10. `listdir(path)` - List contents of directory.

### Math (20)
11. `abs(n)` - Absolute value.
12. `min(a, b)` - Minimum of two values.
13. `max(a, b)` - Maximum of two values.
14. `pow(base, exp)` - Power.
15. `sqrt(n)` - Square root.
16. `round(n)` - Round to nearest integer.
17. `floor(n)` - Round down.
18. `ceil(n)` - Round up.
19. `sin(rad)` - Sine.
20. `cos(rad)` - Cosine.
21. `tan(rad)` - Tangent.
22. `asin(n)` - Arc Sine.
23. `acos(n)` - Arc Cosine.
24. `atan(n)` - Arc Tangent.
25. `log(n)` - Natural logarithm.
26. `exp(n)` - Euler's number explicitly.
27. `clamp(n, min, max)` - Constrain value between min/max.
28. `random()` - Random float between 0.0 and 1.0.
29. `randint(min, max)` - Random integer.
30. `seed(n)` - Seed random number generator.

### String Manipulation (10)
31. `len(str)` - Length of string.
32. `split(str, delim)` - Split string into list.
33. `join(list, delim)` - Join list into string.
34. `replace(str, old, new)` - Replace substrings.
35. `substring(str, start, end)` - Extract substring.
36. `trim(str)` - Remove whitespace ends.
37. `lower(str)` - Convert to lowercase.
38. `upper(str)` - Convert to uppercase.
39. `char_at(str, index)` - Get char code at index.
40. `format(template, args...)` - String interpolation/formatting.

### Collections & Maps (10)
41. `push(list, item)` - Add item to end of list.
42. `pop(list)` - Remove and return last item.
43. `insert(list, index, item)` - Insert at index.
44. `shift(list)` - Remove first item.
45. `contains(coll, item)` - Check if collection contains item.
46. `keys(map)` - Get list of map keys.
47. `values(map)` - Get list of map values.
48. `sort(list)` - Sort list in place.
49. `reverse(list)` - Reverse list in place.
50. `copy(coll)` - Shallow copy of collection.

### System & Runtime (10)
51. `time()` - Current unix timestamp.
52. `sleep(ms)` - Pause execution.
53. `exit(code)` - Terminate program.
54. `env(key)` - Get environment variable.
55. `args()` - Get command line arguments.
56. `cwd()` - Get current working directory.
57. `uuid()` - Generate UUID string.
58. `assert(cond, msg)` - Abort if condition false.
59. `panic(msg)` - Crash with message.
60. `gc()` - Force garbage collection cycle.

### Type & Reflection (10)
61. `type_of(val)` - Return type name string.
62. `str(val)` - Convert to string.
63. `int(val)` - Convert to int.
64. `float(val)` - Convert to float.
65. `bool(val)` - Convert to boolean.
66. `list(val)` - Convert to list.
67. `dict(val)` - Convert to dict.
68. `is_instance(obj, class)` - Check inheritance.
69. `hash(val)` - Calculate hash code.
70. `hex(int)` - Convert int to hex string.

### Misc (5)
71. `bin(int)` - Convert int to binary string.
72. `range(start, end, step)` - Generate range object.
73. `eval(str)` - Execute code string (dangerous).
74. `import_module(path)` - Dynamic import.
75. `base64(str)` - Encode string to base64.
