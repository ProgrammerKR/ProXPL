# Standard Library Reference

Complete reference for ProXPL's built-in functions and modules.

## Overview

The ProXPL standard library provides 75+ native functions covering I/O, math, strings, collections, and system operations.

## Modules

### `std.core` - Core Utilities
| Function | Description |
|----------|-------------|
| `assert(cond, msg)` | Abort if condition is false |
| `typeOf(val)` | Return type string |
| `unwrap(opt)` | Force unwrap or panic |
| `id(obj)` | Object memory identity |
| `hash(val)` | Compute hash |

### `std.math` - Mathematics
| Function | Description |
|----------|-------------|
| `abs(x)`, `ceil(x)`, `floor(x)`, `round(x)` | Arithmetic helpers |
| `min(...)`, `max(...)` | Variadic min/max |
| `pow(b,e)`, `sqrt(x)`, `exp(x)`, `log(x)` | Powers and exponents |
| `sin(x)`, `cos(x)`, `tan(x)` | Trigonometry |
| `random()`, `randint(min, max)` | Random numbers |

### `std.string` - String Manipulation
| Function | Description |
|----------|-------------|
| `upper(s)`, `lower(s)` | Case conversion |
| `trim(s)` | Remove whitespace |
| `split(s, del)`, `join(del, list)` | Split/Join |
| `replace(s, old, new)` | String replacement |
| `contains(s, sub)`, `startswith(s, p)` | Existence checks |
| `substring(s, start, len)` | Substring extraction |

### `std.io` - Input/Output
| Function | Description |
|----------|-------------|
| `input(prompt)` | Read line from stdin |
| `read_file(path)` | Read file content |
| `write_file(path, content)` | Write file |
| `append_file(path, content)` | Append to file |

### `std.sys` - System Interface
| Function | Description |
|----------|-------------|
| `exit(code)` | Terminate program |
| `env(key)` | Get environment variable |
| `platform()` | OS name |
| `version()` | Language version |
| `exec(cmd)` | Execute command |

### Global Functions (no import needed)
| Function | Description |
|----------|-------------|
| `len(val)` | Length of string/list |
| `to_int(v)`, `to_float(v)` | Type conversion |
| `to_string(v)`, `to_bool(v)` | Type conversion |
| `print(...)` | Output to console |

### `std.collections` (*Planned*)
| Function | Description |
|----------|-------------|
| `push`, `pop`, `insert`, `remove`, `sort`, `reverse` | List manipulation |
| `keys`, `values` | Dict access |

### `std.datetime` (*Planned*)
| Function | Description |
|----------|-------------|
| `now`, `timestamp` | Time functions |
| `sleep`, `format_date` | Date/time |

---

**Status**: Stable (core modules), Planned (collections, datetime)
**See Also**: [STDLIB.md](STDLIB.md) for detailed implementations, [GC](GC.md) for garbage collection
