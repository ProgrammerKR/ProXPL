# ProXPL Standard Library Complete API Reference

**Version**: v1.6.4 (Concurrency Core)  
**Last Updated**: 2026-09-02  
**Total Native Functions**: 75+

---

## 📑 Table of Contents

1. [Core Runtime & Type Conversions](#1-core-runtime--type-conversions)
2. [Filesystem & I/O (`std.fs`, `std.io`)](#2-filesystem--io-stdfs-stdio)
3. [String Manipulation (`std.string`, `std.str`)](#3-string-manipulation-stdstring-stdstr)
4. [Collections & Data Structures (`std.collections`)](#4-collections--data-structures-stdcollections)
5. [Mathematics & Statistics (`std.math`)](#5-mathematics--statistics-stdmath)
6. [JSON & Encodings (`std.json`, `std.encoding`)](#6-json--encodings-stdjson-stdencoding)
7. [System, OS & Process (`std.sys`, `std.os`)](#7-system-os--process-stdsys-stdos)
8. [Cryptography & Hashing (`std.crypto`, `std.hash`)](#8-cryptography--hashing-stdcrypto-stdhash)
9. [Reflection & Memory Management (`std.reflect`, `std.gc`)](#9-reflection--memory-management-stdreflect-stdgc)
10. [Database & Concurrency Primitives (`std.db`, Concurrency)](#10-database--concurrency-primitives-stddb-concurrency)

---

## 1. Core Runtime & Type Conversions

### `print(...values: any): void`
Outputs values followed by a newline to standard output.
- **Parameters**: `values` — One or more expressions to format and print.
- **Returns**: `void`
- **Example**:
  ```proxpl
  print("Hello, " + name, "count:", 42);
  ```

### `input(prompt?: string): string`
Reads a line of text from standard input.
- **Parameters**: `prompt` *(optional)* — Text prompt displayed before waiting for input.
- **Returns**: `string` — The line entered by the user (without trailing newline).
- **Example**:
  ```proxpl
  let name = input("Enter your name: ");
  ```

### `type(value: any): string`
Returns the runtime type string of any value.
- **Returns**: `"number" | "string" | "bool" | "list" | "dict" | "function" | "instance" | "null"`
- **Example**:
  ```proxpl
  print(type(42));       // "number"
  print(type([1, 2]));   // "list"
  ```

### `len(collectionOrString: list | dict | string): int`
Returns the element count of a list/dictionary or character count of a string.
- **Returns**: `int` (>= 0)
- **Example**:
  ```proxpl
  let size = len("ProXPL"); // 6
  ```

### `clock(): float`
Returns elapsed execution time in seconds with microsecond resolution.
- **Returns**: `float`
- **Example**:
  ```proxpl
  let t0 = clock();
  heavyComputation();
  let elapsed = clock() - t0;
  print("Elapsed: " + to_string(elapsed) + "s");
  ```

### `to_string(value: any): string`
Converts any value to its string representation.
- **Example**: `let s = to_string(123.45); // "123.45"`

### `to_int(value: any): int`
Parses a string or truncates a float to an integer.
- **Example**: `let n = to_int("42"); // 42`

### `to_float(value: any): float`
Parses a string or converts an integer to a float.
- **Example**: `let f = to_float("3.1415"); // 3.1415`

### `to_bool(value: any): bool`
Converts value to boolean according to truthiness rules (`null`, `0`, `false`, `""` are false).

### `to_hex(n: int): string`
Formats an integer as a lowercase hexadecimal string without prefix.
- **Example**: `to_hex(255); // "ff"`

### `to_bin(n: int): string`
Formats an integer as a binary string.
- **Example**: `to_bin(5); // "101"`

### `char_at(s: string, index: int): string`
Returns the single-character string at the specified 0-based index.

---

## 2. Filesystem & I/O (`std.fs`, `std.io`)

### `read_file(path: string): string | null`
Reads the entire contents of a UTF-8 text file.
- **Parameters**: `path` — Relative or absolute file path.
- **Returns**: `string` with file content, or `null` if file does not exist.
- **Example**:
  ```proxpl
  let content = read_file("config.json");
  ```

### `write_file(path: string, content: string): bool`
Writes text to a file, creating it or overwriting existing contents.
- **Returns**: `true` on success, `false` on I/O error.

### `append_file(path: string, content: string): bool`
Appends text to the end of a file.

### `file_exists(path: string): bool`
Returns `true` if a file or directory exists at `path`.

### `file_size(path: string): int`
Returns file size in bytes, or `-1` if inaccessible.

### `remove_file(path: string): bool`
Deletes the file at `path`.

### `list_dir(path: string): list`
Returns a list of filenames contained within the specified directory.

### `make_dir(path: string): bool`
Creates a directory path.

---

## 3. String Manipulation (`std.string`, `std.str`)

### `split(str: string, delimiter: string): list`
Splits a string into a list of substrings by delimiter.
- **Example**: `split("a,b,c", ","); // ["a", "b", "c"]`

### `join(items: list, delimiter: string): string`
Joins a list of values into a single string separated by `delimiter`.
- **Example**: `join(["a", "b", "c"], "-"); // "a-b-c"`

### `replace(str: string, target: string, replacement: string): string`
Replaces the first occurrence of `target` with `replacement`.

### `replace_all(str: string, target: string, replacement: string): string`
Replaces all occurrences of `target` with `replacement`.

### `to_upper(str: string): string`
Converts string to uppercase.

### `to_lower(str: string): string`
Converts string to lowercase.

### `trim(str: string): string`
Strips leading and trailing whitespace.

### `starts_with(str: string, prefix: string): bool`
Checks if `str` begins with `prefix`.

### `ends_with(str: string, suffix: string): bool`
Checks if `str` ends with `suffix`.

### `contains(str: string, sub: string): bool`
Returns `true` if `sub` is present in `str`.

### `substring(str: string, start: int, length?: int): string`
Extracts a slice of `str` starting at index `start`.

---

## 4. Collections & Data Structures (`std.collections`)

### `push(list: list, item: any): void`
Appends `item` to the end of `list` in-place.

### `pop(list: list): any`
Removes and returns the last element of `list`.

### `insert(list: list, index: int, item: any): void`
Inserts `item` at `index`, shifting subsequent elements right.

### `remove(list: list, index: int): any`
Removes and returns the element at `index`.

### `keys(dict: dict): list`
Returns a list containing all string keys of `dict`.

### `values(dict: dict): list`
Returns a list containing all values of `dict`.

### `clear(collection: list | dict): void`
Removes all elements from the collection in-place.

### `clone(collection: list | dict): list | dict`
Performs a shallow clone of the collection.

### Collection Classes (`std/lib/collections.prox`)
- **`Stack()`**: `push(x)`, `pop()`, `peek()`, `isEmpty()`, `size()`
- **`Queue()`**: `enqueue(x)`, `dequeue()`, `peek()`, `isEmpty()`, `size()`
- **`Set()`**: `add(x)`, `has(x)`, `remove(x)`, `size()`, `values()`

---

## 5. Mathematics & Statistics (`std.math`)

### Basic Math
- **`abs(x: number): number`** — Absolute value.
- **`sqrt(x: number): number`** — Square root.
- **`cbrt(x: number): number`** — Cube root.
- **`pow(base: number, exp: number): number`** — Exponential power.
- **`ceil(x: number): int`** — Round up to nearest integer.
- **`floor(x: number): int`** — Round down to nearest integer.
- **`round(x: number): int`** — Round to nearest integer.
- **`min(a: number, b: number): number`** — Minimum of two values.
- **`max(a: number, b: number): number`** — Maximum of two values.
- **`clamp(val: number, min: number, max: number): number`** — Clamps value between bounds.
- **`random(min?: number, max?: number): number`** — Random float `[0,1)` or integer `[min, max]`.

### Trigonometry & Logarithms
- **`sin(rad)`**, **`cos(rad)`**, **`tan(rad)`** — Standard trigonometric functions.
- **`asin(x)`**, **`acos(x)`**, **`atan(x)`**, **`atan2(y, x)`** — Inverse trigonometric functions.
- **`log(x: number): number`** — Natural logarithm (base e).
- **`log10(x: number): number`** — Base-10 logarithm.
- **`exp(x: number): number`** — Exponential function ($e^x$).

---

## 6. JSON & Encodings (`std.json`, `std.encoding`)

### `json_parse(jsonString: string): any`
Parses a JSON formatted string into ProXPL dicts, lists, numbers, and strings.

### `json_stringify(value: any): string`
Serializes any ProXPL data structure into a JSON string.

### `base64_encode(data: string): string`
Encodes plaintext to Base64.

### `base64_decode(encoded: string): string`
Decodes Base64 to plaintext.

### `hex_encode(bytes: string): string`
Encodes data to a hexadecimal string.

### `hex_decode(hex: string): string`
Decodes hexadecimal string to raw data.

---

## 7. System, OS & Process (`std.sys`, `std.os`)

### `time(): int`
Returns current Unix epoch timestamp in seconds.

### `sleep(milliseconds: int): void`
Pauses thread execution for the specified duration.

### `env(varName: string): string | null`
Returns the environment variable value or `null` if unset.

### `args(): list`
Returns the list of command-line arguments passed to the script.

### `exec(command: string): int`
Executes a system shell command and returns the process exit code.

### `exit(code: int): void`
Immediately terminates the ProXPL runtime with status `code`.

### `platform(): string`
Returns `"windows"`, `"linux"`, or `"macos"`.

---

## 8. Cryptography & Hashing (`std.crypto`, `std.hash`)

### `sha256(data: string): string`
Computes the 64-character hex SHA-256 digest of `data`.

### `md5(data: string): string`
Computes the 32-character hex MD5 digest of `data`.

### `uuid_v4(): string`
Generates a random RFC-4122 Version 4 UUID string.

### `crc32(data: string): int`
Computes the 32-bit Cyclic Redundancy Check checksum.

---

## 9. Reflection & Memory Management (`std.reflect`, `std.gc`)

### `gc_collect(): int`
Triggers an immediate mark-and-sweep garbage collection cycle. Returns bytes freed.

### `gc_allocated_bytes(): int`
Returns total currently allocated heap memory in bytes.

### `type_of(val: any): string`
Returns detailed type information.

---

## 10. Database & Concurrency Primitives (`std.db`, Concurrency)

### SQLite Database (`std.db`)
- `db_open(path: string): DBConnection` — Opens or creates an SQLite database.
- `db_exec(conn: DBConnection, sql: string): bool` — Executes non-query statements.
- `db_query(conn: DBConnection, sql: string): list` — Queries and returns row dictionaries.
- `db_close(conn: DBConnection): void` — Closes the database connection.

### Concurrency Primitives
- `actor ActorName { receive(msg) { ... } }` — Lightweight message-passing actor.
- `worker ! msg` — Asynchronous send to actor mailbox.
- `let res = worker ? req` — Synchronous request-response to actor.
- `comptime { ... }` — Compile-time expression evaluation.
