# ProXPL Standard Library Documentation

**Version: 0.1.0**  
**Last Updated: December 2024**

Complete reference for all 75+ built-in functions in the ProXPL standard library, organized by module with examples and usage notes.

---

## Table of Contents

1. [I/O Functions](#io-functions)
2. [Math Functions](#math-functions)
3. [String Functions](#string-functions)
4. [Collections Functions](#collections-functions)
5. [Type Conversion Functions](#type-conversion-functions)
6. [System Functions](#system-functions)
7. [DateTime Functions](#datetime-functions)
8. [Runtime Functions](#runtime-functions)
9. [Quick Reference](#quick-reference)

---

## I/O Functions

### print(...args) → void

Prints values to the console, separated by spaces.

**Signature:**
```c
print(...args) -> void
```

**Parameters:**
- `...args` - One or more values of any type

**Returns:** void

**Examples:**
```javascript
print("Hello, World!");
// Output: Hello, World!

print("Count:", 42, "Active:", true);
// Output: Count: 42 Active: true

let items = [1, 2, 3];
print(items);
// Output: [1, 2, 3]
```

**Notes:**
- Automatically converts all arguments to strings
- Arrays and objects are formatted as readable output
- null prints as "null"
- Functions print as "<function>"

---

### input(prompt) → string

Reads a line of input from the user with an optional prompt.

**Signature:**
```c
input(prompt: string) -> string
```

**Parameters:**
- `prompt` - String to display before reading input

**Returns:** string - The user's input (without trailing newline)

**Examples:**
```javascript
func greet() {
    let name = input("What is your name? ");
    print("Hello, " + name + "!");
}

greet();
// Output: What is your name? Alice
//         Hello, Alice!
```

**Notes:**
- Displays the prompt before waiting for input
- Automatically removes the trailing newline
- Returns empty string if user enters nothing

---

### read_file(path) → string

Reads the entire contents of a text file.

**Signature:**
```c
read_file(path: string) -> string
```

**Parameters:**
- `path` - Path to the file to read

**Returns:** string - File contents

**Examples:**
```javascript
let content = read_file("config.txt");
print(content);

let lines = split(content, "\n");
for (let i = 0; i < len(lines); i = i + 1) {
    print(lines[i]);
}
```

**Notes:**
- Throws error if file doesn't exist or can't be read
- Preserves newlines and special characters
- Read entire file into memory

---

### write_file(path, content) → bool

Writes content to a file, overwriting if it exists.

**Signature:**
```c
write_file(path: string, content: string) -> bool
```

**Parameters:**
- `path` - Path to the file
- `content` - Content to write

**Returns:** bool - true if successful

**Examples:**
```javascript
let data = "Hello, File!\nLine 2\n";
if (write_file("output.txt", data)) {
    print("File written successfully");
} else {
    print("Failed to write file");
}
```

**Notes:**
- Creates file if it doesn't exist
- Overwrites entire file (use append_file to add)
- Returns false if write fails

---

### append_file(path, content) → bool

Appends content to the end of a file.

**Signature:**
```c
append_file(path: string, content: string) -> bool
```

**Parameters:**
- `path` - Path to the file
- `content` - Content to append

**Returns:** bool - true if successful

**Examples:**
```javascript
for (let i = 1; i <= 3; i = i + 1) {
    append_file("log.txt", "Log entry " + to_string(i) + "\n");
}
```

**Notes:**
- Creates file if it doesn't exist
- Appends to existing content
- Returns false if operation fails

---

## Math Functions

### abs(n) → number

Returns the absolute value of a number.

**Signature:**
```c
abs(n: int|float) -> int|float
```

**Parameters:**
- `n` - A number

**Returns:** Same type as input

**Examples:**
```javascript
print(abs(5));      // 5
print(abs(-5));     // 5
print(abs(0));      // 0
print(abs(-3.14));  // 3.14
```

---

### ceil(n) → int

Rounds up to the nearest integer.

**Signature:**
```c
ceil(n: float) -> int
```

**Parameters:**
- `n` - A floating-point number

**Returns:** int

**Examples:**
```javascript
print(ceil(3.1));   // 4
print(ceil(3.9));   // 4
print(ceil(3.0));   // 3
print(ceil(-2.1));  // -2
```

---

### floor(n) → int

Rounds down to the nearest integer.

**Signature:**
```c
floor(n: float) -> int
```

**Parameters:**
- `n` - A floating-point number

**Returns:** int

**Examples:**
```javascript
print(floor(3.1));  // 3
print(floor(3.9));  // 3
print(floor(-2.9)); // -3
```

---

### round(n, digits?) → int|float

Rounds to the nearest integer or specified decimal places.

**Signature:**
```c
round(n: float, digits?: int) -> int|float
```

**Parameters:**
- `n` - A number to round
- `digits` - Optional: decimal places (default: 0)

**Returns:** int if no digits, float if digits specified

**Examples:**
```javascript
print(round(3.7));      // 4
print(round(3.4));      // 3
print(round(3.14159, 2));  // 3.14
print(round(3.14159, 3));  // 3.142
```

---

### sqrt(n) → float

Returns the square root of a number.

**Signature:**
```c
sqrt(n: number) -> float
```

**Parameters:**
- `n` - A non-negative number

**Returns:** float

**Examples:**
```javascript
print(sqrt(16));    // 4.0
print(sqrt(2));     // 1.414...
print(sqrt(0));     // 0.0
```

---

### pow(base, exp) → number

Raises base to the power of exponent.

**Signature:**
```c
pow(base: number, exp: number) -> number
```

**Parameters:**
- `base` - The base
- `exp` - The exponent

**Returns:** number (int if both args are int and exp >= 0)

**Examples:**
```javascript
print(pow(2, 3));    // 8
print(pow(2, 10));   // 1024
print(pow(10, 2));   // 100
print(pow(2, -1));   // 0.5
print(pow(4, 0.5));  // 2.0
```

---

### log(n, base?) → float

Returns the logarithm of n to the given base (default: natural log).

**Signature:**
```c
log(n: float, base?: float) -> float
```

**Parameters:**
- `n` - The number
- `base` - Optional: logarithm base (default: e)

**Returns:** float

**Examples:**
```javascript
print(log(2.718));      // ≈ 1.0 (natural log)
print(log(100, 10));    // 2.0
print(log(8, 2));       // 3.0
```

---

### exp(n) → float

Returns e raised to the power of n (e^n).

**Signature:**
```c
exp(n: float) -> float
```

**Parameters:**
- `n` - The exponent

**Returns:** float

**Examples:**
```javascript
print(exp(0));      // 1.0
print(exp(1));      // 2.718...
print(exp(2));      // 7.389...
```

---

### sin(n) → float

Returns the sine of a number (in radians).

**Signature:**
```c
sin(n: float) -> float
```

**Parameters:**
- `n` - Angle in radians

**Returns:** float

**Examples:**
```javascript
print(sin(0));                  // 0.0
// sin(pi/2) ≈ 1.0
// sin(pi) ≈ 0.0
```

---

### cos(n) → float

Returns the cosine of a number (in radians).

**Signature:**
```c
cos(n: float) -> float
```

**Parameters:**
- `n` - Angle in radians

**Returns:** float

**Examples:**
```javascript
print(cos(0));  // 1.0
// cos(pi/2) ≈ 0.0
// cos(pi) ≈ -1.0
```

---

### tan(n) → float

Returns the tangent of a number (in radians).

**Signature:**
```c
tan(n: float) -> float
```

**Parameters:**
- `n` - Angle in radians

**Returns:** float

**Examples:**
```javascript
print(tan(0));  // 0.0
// tan(pi/4) ≈ 1.0
```

---

### max(list) → number

Returns the maximum value in a list.

**Signature:**
```c
max(list: list<number>) -> number
```

**Parameters:**
- `list` - List of numbers

**Returns:** number - Largest value

**Examples:**
```javascript
print(max([3, 7, 2, 9, 1]));     // 9
print(max([3.5, 2.1, 8.9]));     // 8.9
print(max([42]));                 // 42
```

---

### min(list) → number

Returns the minimum value in a list.

**Signature:**
```c
min(list: list<number>) -> number
```

**Parameters:**
- `list` - List of numbers

**Returns:** number - Smallest value

**Examples:**
```javascript
print(min([3, 7, 2, 9, 1]));     // 1
print(min([3.5, 2.1, 8.9]));     // 2.1
print(min([42]));                 // 42
```

---

### random() → float

Returns a random float between 0.0 (inclusive) and 1.0 (exclusive).

**Signature:**
```c
random() -> float
```

**Parameters:** None

**Returns:** float in range [0.0, 1.0)

**Examples:**
```javascript
let r = random();
print(r);  // e.g., 0.742891

let scaled = random() * 100;
print(to_int(scaled));  // Random int 0-99
```

---

### randint(min, max) → int

Returns a random integer in the range [min, max).

**Signature:**
```c
randint(min: int, max: int) -> int
```

**Parameters:**
- `min` - Minimum value (inclusive)
- `max` - Maximum value (exclusive)

**Returns:** int

**Examples:**
```javascript
print(randint(1, 7));       // Random 1-6 (dice roll)
print(randint(1, 101));     // Random 1-100
print(randint(0, 2));       // 0 or 1 (coin flip)
```

---

## String Functions

### len(s) → int

Returns the length of a string.

**Signature:**
```c
len(s: string) -> int
```

**Parameters:**
- `s` - A string

**Returns:** int - Number of characters

**Examples:**
```javascript
print(len("Hello"));        // 5
print(len(""));             // 0
print(len("ProXPL"));       // 6
```

---

### upper(s) → string

Returns the string in uppercase.

**Signature:**
```c
upper(s: string) -> string
```

**Parameters:**
- `s` - A string

**Returns:** string - Uppercase version

**Examples:**
```javascript
print(upper("hello"));      // "HELLO"
print(upper("ProXPL"));     // "PROXPL"
print(upper("123abc"));     // "123ABC"
```

---

### lower(s) → string

Returns the string in lowercase.

**Signature:**
```c
lower(s: string) -> string
```

**Parameters:**
- `s` - A string

**Returns:** string - Lowercase version

**Examples:**
```javascript
print(lower("HELLO"));      // "hello"
print(lower("ProXPL"));     // "proxpl"
print(lower("ABC123"));     // "abc123"
```

---

### trim(s) → string

Removes leading and trailing whitespace.

**Signature:**
```c
trim(s: string) -> string
```

**Parameters:**
- `s` - A string

**Returns:** string - Trimmed string

**Examples:**
```javascript
print(trim("  hello  "));   // "hello"
print(trim("\t\nhello\n")); // "hello"
print(trim("world"));       // "world"
```

---

### split(s, sep) → list<string>

Splits a string into a list of substrings.

**Signature:**
```c
split(s: string, sep: string) -> list<string>
```

**Parameters:**
- `s` - String to split
- `sep` - Separator string

**Returns:** list<string>

**Examples:**
```javascript
let words = split("a,b,c", ",");
print(words);  // ["a", "b", "c"]

let parts = split("one two three", " ");
print(parts);  // ["one", "two", "three"]
```

---

### join(sep, list) → string

Joins list elements into a string with separator.

**Signature:**
```c
join(sep: string, list: list) -> string
```

**Parameters:**
- `sep` - Separator string
- `list` - List to join

**Returns:** string

**Examples:**
```javascript
let parts = ["one", "two", "three"];
print(join("-", parts));    // "one-two-three"

let nums = [1, 2, 3];
print(join(", ", nums));    // "1, 2, 3"
```

---

### replace(s, old, new) → string

Replaces all occurrences of old with new.

**Signature:**
```c
replace(s: string, old: string, new: string) -> string
```

**Parameters:**
- `s` - Original string
- `old` - Substring to find
- `new` - Replacement string

**Returns:** string - Modified string

**Examples:**
```javascript
print(replace("hello world", "world", "ProXPL"));
// "hello ProXPL"

print(replace("aaa", "a", "b"));
// "bbb"
```

---

### startswith(s, prefix) → bool

Checks if string starts with prefix.

**Signature:**
```c
startswith(s: string, prefix: string) -> bool
```

**Parameters:**
- `s` - String to check
- `prefix` - Prefix to look for

**Returns:** bool

**Examples:**
```javascript
print(startswith("hello", "he"));    // true
print(startswith("hello", "world")); // false
print(startswith("", ""));           // true
```

---

### endswith(s, suffix) → bool

Checks if string ends with suffix.

**Signature:**
```c
endswith(s: string, suffix: string) -> bool
```

**Parameters:**
- `s` - String to check
- `suffix` - Suffix to look for

**Returns:** bool

**Examples:**
```javascript
print(endswith("hello", "lo"));      // true
print(endswith("hello", "world"));   // false
print(endswith("file.txt", ".txt")); // true
```

---

### contains(haystack, needle) → bool

Checks if haystack contains needle substring.

**Signature:**
```c
contains(haystack: string, needle: string) -> bool
```

**Parameters:**
- `haystack` - String to search in
- `needle` - Substring to find

**Returns:** bool

**Examples:**
```javascript
print(contains("hello world", "world"));  // true
print(contains("ProXPL", "XP"));         // true
print(contains("abc", "xyz"));           // false
```

---

### index_of(s, sub) → int

Returns the index of the first occurrence of substring.

**Signature:**
```c
index_of(s: string, sub: string) -> int
```

**Parameters:**
- `s` - String to search in
- `sub` - Substring to find

**Returns:** int - Index (0-based), or -1 if not found

**Examples:**
```javascript
print(index_of("hello world", "world"));  // 6
print(index_of("hello", "h"));            // 0
print(index_of("hello", "x"));            // -1
```

---

### substring(s, start, end?) → string

Extracts a substring.

**Signature:**
```c
substring(s: string, start: int, end?: int) -> string
```

**Parameters:**
- `s` - Original string
- `start` - Start index (inclusive)
- `end` - Optional: end index (exclusive, default: end of string)

**Returns:** string - Substring

**Examples:**
```javascript
print(substring("hello", 0, 3));    // "hel"
print(substring("hello", 1));       // "ello"
print(substring("hello", 1, 4));    // "ell"
```

---

### char_at(s, index) → string

Returns the character at the specified index.

**Signature:**
```c
char_at(s: string, index: int) -> string
```

**Parameters:**
- `s` - String
- `index` - Character index (0-based)

**Returns:** string - Single character

**Examples:**
```javascript
print(char_at("hello", 0));     // "h"
print(char_at("hello", 4));     // "o"
print(char_at("hello", -1));    // Error (invalid index)
```

---

### format(fmt, ...args) → string

Formats a string with printf-style placeholders.

**Signature:**
```c
format(fmt: string, ...args) -> string
```

**Parameters:**
- `fmt` - Format string with %s, %d, %f, etc.
- `...args` - Values to insert

**Returns:** string

**Examples:**
```javascript
print(format("Hello, %s!", "World"));
// "Hello, World!"

print(format("Count: %d, Ratio: %.2f", 42, 3.14159));
// "Count: 42, Ratio: 3.14"
```

---

### to_bytes(s) → bytes

Converts a string to bytes.

**Signature:**
```c
to_bytes(s: string) -> bytes
```

**Parameters:**
- `s` - String to convert

**Returns:** bytes

**Examples:**
```javascript
let data = to_bytes("Hello");
print(data);  // bytes object
```

---

## Collections Functions

### len(col) → int

Returns the number of elements in a collection.

**Signature:**
```c
len(col: list|dict|string) -> int
```

**Parameters:**
- `col` - A list, dict, or string

**Returns:** int

**Examples:**
```javascript
print(len([1, 2, 3]));          // 3
print(len({"a": 1, "b": 2}));  // 2
print(len("hello"));             // 5
print(len([]));                  // 0
```

---

### push(list, item) → void

Appends an item to the end of a list.

**Signature:**
```c
push(list: list, item: any) -> void
```

**Parameters:**
- `list` - List to modify
- `item` - Item to add

**Returns:** void

**Examples:**
```javascript
let nums = [1, 2];
push(nums, 3);
print(nums);  // [1, 2, 3]

let items = [];
push(items, "a");
push(items, "b");
print(items);  // ["a", "b"]
```

---

### pop(list) → any

Removes and returns the last item from a list.

**Signature:**
```c
pop(list: list) -> any
```

**Parameters:**
- `list` - List to modify

**Returns:** The removed item

**Examples:**
```javascript
let nums = [1, 2, 3];
let last = pop(nums);
print(last);   // 3
print(nums);   // [1, 2]
```

---

### insert(list, index, item) → void

Inserts an item at a specific position.

**Signature:**
```c
insert(list: list, index: int, item: any) -> void
```

**Parameters:**
- `list` - List to modify
- `index` - Position to insert at
- `item` - Item to insert

**Returns:** void

**Examples:**
```javascript
let nums = [1, 2, 3];
insert(nums, 1, 10);
print(nums);  // [1, 10, 2, 3]
```

---

### remove(list, item) → void

Removes the first occurrence of an item.

**Signature:**
```c
remove(list: list, item: any) -> void
```

**Parameters:**
- `list` - List to modify
- `item` - Item to remove

**Returns:** void

**Examples:**
```javascript
let items = [1, 2, 3, 2];
remove(items, 2);
print(items);  // [1, 3, 2]
```

---

### sort(list) → void

Sorts a list in ascending order.

**Signature:**
```c
sort(list: list) -> void
```

**Parameters:**
- `list` - List to sort (modified in place)

**Returns:** void

**Examples:**
```javascript
let nums = [3, 1, 4, 1, 5];
sort(nums);
print(nums);  // [1, 1, 3, 4, 5]

let words = ["dog", "cat", "ant"];
sort(words);
print(words);  // ["ant", "cat", "dog"]
```

---

### reverse(list) → void

Reverses a list in place.

**Signature:**
```c
reverse(list: list) -> void
```

**Parameters:**
- `list` - List to reverse

**Returns:** void

**Examples:**
```javascript
let nums = [1, 2, 3];
reverse(nums);
print(nums);  // [3, 2, 1]
```

---

### range(start, end, step?) → list

Creates a list of numbers from start to end.

**Signature:**
```c
range(start: int, end: int, step?: int) -> list<int>
```

**Parameters:**
- `start` - Start value (inclusive)
- `end` - End value (exclusive)
- `step` - Optional: increment (default: 1)

**Returns:** list<int>

**Examples:**
```javascript
print(range(0, 5));         // [0, 1, 2, 3, 4]
print(range(1, 6));         // [1, 2, 3, 4, 5]
print(range(0, 10, 2));     // [0, 2, 4, 6, 8]
print(range(5, 0, -1));     // [5, 4, 3, 2, 1]
```

---

### keys(dict) → list

Returns all keys of a dictionary as a list.

**Signature:**
```c
keys(dict: dict) -> list<string>
```

**Parameters:**
- `dict` - Dictionary

**Returns:** list<string>

**Examples:**
```javascript
let person = {"name": "Alice", "age": 30};
print(keys(person));  // ["name", "age"]
```

---

### values(dict) → list

Returns all values of a dictionary as a list.

**Signature:**
```c
values(dict: dict) -> list<any>
```

**Parameters:**
- `dict` - Dictionary

**Returns:** list<any>

**Examples:**
```javascript
let person = {"name": "Alice", "age": 30};
print(values(person));  // ["Alice", 30]
```

---

### entries(dict) → list

Returns all key-value pairs as a list of lists.

**Signature:**
```c
entries(dict: dict) -> list<list>
```

**Parameters:**
- `dict` - Dictionary

**Returns:** list of [key, value] pairs

**Examples:**
```javascript
let person = {"name": "Alice", "age": 30};
let pairs = entries(person);
// [["name", "Alice"], ["age", 30]]
```

---

### contains_key(dict, key) → bool

Checks if a dictionary has a key.

**Signature:**
```c
contains_key(dict: dict, key: string) -> bool
```

**Parameters:**
- `dict` - Dictionary
- `key` - Key to check

**Returns:** bool

**Examples:**
```javascript
let person = {"name": "Alice", "age": 30};
print(contains_key(person, "name"));   // true
print(contains_key(person, "city"));   // false
```

---

### merge(dict1, dict2) → dict

Merges two dictionaries.

**Signature:**
```c
merge(dict1: dict, dict2: dict) -> dict
```

**Parameters:**
- `dict1` - First dictionary
- `dict2` - Second dictionary

**Returns:** dict - New merged dictionary

**Examples:**
```javascript
let d1 = {"a": 1, "b": 2};
let d2 = {"b": 3, "c": 4};
let merged = merge(d1, d2);
// {"a": 1, "b": 3, "c": 4}
```

---

### clone(col) → any

Creates a shallow copy of a collection.

**Signature:**
```c
clone(col: list|dict) -> list|dict
```

**Parameters:**
- `col` - Collection to copy

**Returns:** Shallow copy

**Examples:**
```javascript
let original = [1, 2, [3, 4]];
let copy = clone(original);
// Modifying top level doesn't affect original
copy[0] = 99;  // original[0] still 1
// But nested lists are shared
copy[2][0] = 99;  // original[2][0] is now 99
```

---

### deep_clone(col) → any

Creates a deep copy of a collection (recursive).

**Signature:**
```c
deep_clone(col: list|dict) -> list|dict
```

**Parameters:**
- `col` - Collection to copy

**Returns:** Deep copy

**Examples:**
```javascript
let original = [1, 2, [3, 4]];
let copy = deep_clone(original);
copy[2][0] = 99;  // original[2][0] still 3
```

---

### clear(col) → void

Removes all elements from a collection.

**Signature:**
```c
clear(col: list|dict) -> void
```

**Parameters:**
- `col` - Collection to clear

**Returns:** void

**Examples:**
```javascript
let nums = [1, 2, 3];
clear(nums);
print(nums);  // []

let person = {"name": "Alice"};
clear(person);
print(person);  // {}
```

---

## Type Conversion Functions

### to_int(value) → int

Converts a value to an integer.

**Signature:**
```c
to_int(value: any) -> int
```

**Parameters:**
- `value` - Value to convert

**Returns:** int

**Examples:**
```javascript
print(to_int(3.7));        // 3 (truncated)
print(to_int("42"));       // 42
print(to_int(true));       // 1
print(to_int(false));      // 0
```

---

### to_float(value) → float

Converts a value to a float.

**Signature:**
```c
to_float(value: any) -> float
```

**Parameters:**
- `value` - Value to convert

**Returns:** float

**Examples:**
```javascript
print(to_float(3));        // 3.0
print(to_float("3.14"));   // 3.14
print(to_float(true));     // 1.0
```

---

### to_string(value) → string

Converts a value to a string.

**Signature:**
```c
to_string(value: any) -> string
```

**Parameters:**
- `value` - Value to convert

**Returns:** string

**Examples:**
```javascript
print(to_string(42));           // "42"
print(to_string(3.14));         // "3.14"
print(to_string(true));         // "true"
print(to_string([1, 2, 3]));   // "[1, 2, 3]"
```

---

### to_bool(value) → bool

Converts a value to a boolean.

**Signature:**
```c
to_bool(value: any) -> bool
```

**Parameters:**
- `value` - Value to convert

**Returns:** bool

**Examples:**
```javascript
print(to_bool(1));      // true
print(to_bool(0));      // false
print(to_bool(""));     // false
print(to_bool("text")); // true
print(to_bool(null));   // false
```

---

### to_list(value) → list

Converts a value to a list.

**Signature:**
```c
to_list(value: any) -> list
```

**Parameters:**
- `value` - Value to convert

**Returns:** list

**Examples:**
```javascript
print(to_list("abc"));  // ["a", "b", "c"]
print(to_list(range(1, 4)));  // [1, 2, 3]
```

---

### to_dict(value) → dict

Converts a value to a dictionary.

**Signature:**
```c
to_dict(value: any) -> dict
```

**Parameters:**
- `value` - Value to convert (typically list of pairs)

**Returns:** dict

**Examples:**
```javascript
let pairs = [["a", 1], ["b", 2]];
let dict = to_dict(pairs);
// {"a": 1, "b": 2}
```

---

### to_hex(value) → string

Converts an integer to hexadecimal string.

**Signature:**
```c
to_hex(value: int) -> string
```

**Parameters:**
- `value` - Integer to convert

**Returns:** string

**Examples:**
```javascript
print(to_hex(255));     // "ff"
print(to_hex(256));     // "100"
print(to_hex(4095));    // "fff"
```

---

### to_bin(value) → string

Converts an integer to binary string.

**Signature:**
```c
to_bin(value: int) -> string
```

**Parameters:**
- `value` - Integer to convert

**Returns:** string

**Examples:**
```javascript
print(to_bin(5));       // "101"
print(to_bin(255));     // "11111111"
```

---

### parse_json(str) → any

Parses a JSON string into a value.

**Signature:**
```c
parse_json(str: string) -> any
```

**Parameters:**
- `str` - JSON string

**Returns:** Parsed value

**Examples:**
```javascript
let data = parse_json('{"name": "Alice", "age": 30}');
print(data["name"]);  // "Alice"
```

---

### stringify_json(value) → string

Converts a value to JSON string.

**Signature:**
```c
stringify_json(value: any) -> string
```

**Parameters:**
- `value` - Value to convert

**Returns:** string

**Examples:**
```javascript
let obj = {"name": "Alice", "age": 30};
let json = stringify_json(obj);
// '{"name":"Alice","age":30}'
```

---

## System Functions

### clock() → float

Returns the current time in seconds since epoch.

**Signature:**
```c
clock() -> float
```

**Parameters:** None

**Returns:** float

**Examples:**
```javascript
let start = clock();
// ... do work ...
let elapsed = clock() - start;
print("Elapsed: " + to_string(elapsed) + " seconds");
```

---

### sleep(ms) → void

Pauses execution for the specified milliseconds.

**Signature:**
```c
sleep(ms: int) -> void
```

**Parameters:**
- `ms` - Milliseconds to sleep

**Returns:** void

**Examples:**
```javascript
print("Starting");
sleep(1000);  // Wait 1 second
print("Done");
```

---

### exit(code) → void

Exits the program with a status code.

**Signature:**
```c
exit(code: int) -> void
```

**Parameters:**
- `code` - Exit code (0 = success)

**Returns:** void (doesn't return)

**Examples:**
```javascript
if (error) {
    print("Error occurred");
    exit(1);
}
exit(0);  // Success
```

---

### platform() → string

Returns information about the platform.

**Signature:**
```c
platform() -> string
```

**Parameters:** None

**Returns:** string - "Linux", "Windows", "Darwin", etc.

**Examples:**
```javascript
let os = platform();
if (os == "Windows") {
    print("Running on Windows");
}
```

---

### exec(command) → string

Executes a system command and returns output.

**Signature:**
```c
exec(command: string) -> string
```

**Parameters:**
- `command` - Command to execute

**Returns:** string - Command output

**Examples:**
```javascript
let output = exec("ls -la");
print(output);
```

---

## DateTime Functions

### now() → float

Returns the current timestamp (seconds since epoch).

**Signature:**
```c
now() -> float
```

**Parameters:** None

**Returns:** float

**Examples:**
```javascript
let current = now();
print(to_string(current));
```

---

### timestamp(date_string) → float

Converts a date string to timestamp.

**Signature:**
```c
timestamp(date_string: string) -> float
```

**Parameters:**
- `date_string` - Date in format "YYYY-MM-DD HH:MM:SS"

**Returns:** float

**Examples:**
```javascript
let ts = timestamp("2024-12-11 12:00:00");
print(to_string(ts));
```

---

### format_date(timestamp, format) → string

Formats a timestamp as a string.

**Signature:**
```c
format_date(timestamp: float, format: string) -> string
```

**Parameters:**
- `timestamp` - Time value
- `format` - Format string ("%Y-%m-%d", etc.)

**Returns:** string

**Examples:**
```javascript
let ts = now();
print(format_date(ts, "%Y-%m-%d %H:%M:%S"));
// "2024-12-11 15:30:42"
```

---

### parse_date(date_string) → float

Parses a date string into timestamp.

**Signature:**
```c
parse_date(date_string: string) -> float
```

**Parameters:**
- `date_string` - Date string

**Returns:** float

**Examples:**
```javascript
let ts = parse_date("2024-12-11");
print(to_string(ts));
```

---

## Runtime Functions

### typeof(value) → string

Returns the type name of a value.

**Signature:**
```c
typeof(value: any) -> string
```

**Parameters:**
- `value` - Any value

**Returns:** string - Type name

**Examples:**
```javascript
print(typeof(42));           // "int"
print(typeof(3.14));         // "float"
print(typeof("hello"));      // "string"
print(typeof([1, 2]));       // "list"
print(typeof(true));         // "bool"
print(typeof(null));         // "null"
print(typeof(print));        // "function"
```

---

### assert(condition, message?) → void

Asserts a condition is true, throws error if false.

**Signature:**
```c
assert(condition: bool, message?: string) -> void
```

**Parameters:**
- `condition` - Condition to check
- `message` - Optional: error message

**Returns:** void

**Examples:**
```javascript
assert(5 > 3);
assert(len(list) > 0, "List must not be empty");
```

---

### id(value) → int

Returns the identity (memory address) of an object.

**Signature:**
```c
id(value: any) -> int
```

**Parameters:**
- `value` - Any value

**Returns:** int

**Examples:**
```javascript
let obj = {"a": 1};
print(to_string(id(obj)));  // Some large number
```

---

### hash(value) → int

Returns the hash value of an object.

**Signature:**
```c
hash(value: any) -> int
```

**Parameters:**
- `value` - Value to hash

**Returns:** int

**Examples:**
```javascript
let h1 = hash("hello");
let h2 = hash("hello");
// h1 == h2 (same string has same hash)
```

---

### is_instance(value, type) → bool

Checks if a value is an instance of a type.

**Signature:**
```c
is_instance(value: any, type: string) -> bool
```

**Parameters:**
- `value` - Value to check
- `type` - Type name as string

**Returns:** bool

**Examples:**
```javascript
print(is_instance(42, "int"));      // true
print(is_instance("hello", "int")); // false
print(is_instance([1, 2], "list")); // true
```

---

## Quick Reference

### All Functions by Category

**I/O (5):** print, input, read_file, write_file, append_file
**Math (15):** abs, ceil, floor, round, sqrt, pow, log, exp, sin, cos, tan, max, min, random, randint
**String (15):** len, upper, lower, trim, split, join, replace, startswith, endswith, contains, index_of, substring, char_at, format, to_bytes
**Collections (15):** len, push, pop, insert, remove, sort, reverse, range, keys, values, entries, contains_key, merge, clone, deep_clone, clear
**Type Conversion (10):** to_int, to_float, to_string, to_bool, to_list, to_dict, to_hex, to_bin, parse_json, stringify_json
**System (5):** clock, sleep, exit, platform, exec
**DateTime (5):** now, timestamp, format_date, parse_date
**Runtime (5):** typeof, assert, id, hash, is_instance

**Total: 75+ functions**

See the specific sections above for detailed documentation and examples.
