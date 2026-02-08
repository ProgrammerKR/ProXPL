# ProXPL Standard Library

The ProXPL Standard Library provides a comprehensive set of modules for common programming tasks.

## Available Libraries

### Core Libraries

#### `collections.prox` - Collection Data Structures and Utilities
- **Stack**: LIFO data structure with push, pop, peek operations
- **Queue**: FIFO data structure with enqueue, dequeue operations
- **Set**: Unique value collection
- **Collections utilities**: map, filter, reduce, sort, unique, flatten, reverse, and more

```prox
let stack = Stack();
stack.push(1).push(2).push(3);
print(stack.pop()); // 3

let numbers = [1, 2, 3, 4, 5];
let doubled = Collections.map(numbers, func(x) { return x * 2; });
let evens = Collections.filter(numbers, func(x) { return x % 2 == 0; });
```

#### `str.prox` - String Manipulation
Comprehensive string utilities including:
- Case conversion: `upper`, `lower`, `capitalize`, `title`
- Whitespace: `trim`, `trimStart`, `trimEnd`
- Search: `contains`, `startsWith`, `endsWith`, `indexOf`, `lastIndexOf`
- Manipulation: `split`, `replace`, `replaceAll`, `repeat`, `reverse`
- Padding: `padStart`, `padEnd`
- Analysis: `count`, `isDigit`, `isAlpha`, `isAlphaNumeric`
- Utilities: `join`, `lines`, `words`

```prox
let text = "  hello world  ";
print(StringUtils.trim(text));           // "hello world"
print(StringUtils.capitalize(text));     // "Hello world"
print(StringUtils.title(text));          // "Hello World"
print(StringUtils.repeat("Ha", 3));      // "HaHaHa"
print(StringUtils.padStart("42", 5, "0")); // "00042"
```

#### `math.prox` - Mathematical Functions
Extended math library with:
- **Constants**: PI, E, TAU, PHI, SQRT2, LN2, etc.
- **Basic**: abs, sign, ceil, floor, round, trunc, min, max, clamp
- **Power/Root**: pow, sqrt, cbrt, hypot
- **Exponential**: exp, log, log2, log10, ln
- **Trigonometric**: sin, cos, tan, asin, acos, atan, atan2
- **Hyperbolic**: sinh, cosh, tanh
- **Statistical**: sum, mean, median, variance, stddev
- **ML Activation**: sigmoid, relu, leakyRelu, softplus
- **Utilities**: factorial, gcd, lcm, isPrime, fibonacci, lerp, normalize, map

```prox
print(Math.PI);                    // 3.14159...
print(Math.factorial(5));          // 120
print(Math.gcd(48, 18));          // 6
print(Math.isPrime(17));          // true
print(Math.fibonacci(10));        // 55
print(Math.mean([1,2,3,4,5]));   // 3
print(Math.clamp(15, 0, 10));    // 10
```

### New Libraries

#### `datetime.prox` - Date and Time Manipulation
Complete date/time handling:
- **DateTime class**: Date and time representation
- **Factory methods**: `now()`, `fromTimestamp()`, `fromDate()`
- **Getters**: year, month, day, hour, minute, second, dayOfWeek
- **Formatting**: Custom format strings (YYYY-MM-DD, etc.)
- **Arithmetic**: addSeconds, addMinutes, addHours, addDays, addWeeks
- **Comparison**: isBefore, isAfter, isSame, diff, diffInDays
- **Time utilities**: measure, formatDuration

```prox
let now = DateTime.now();
print(now.format("YYYY-MM-DD HH:mm:ss"));
print(now.toISOString());

let future = now.addDays(7);
print("Days until: " + to_string(now.diffInDays(future)));

let duration = Time.formatDuration(3665); // "1h 1m 5s"
```

#### `crypto.prox` - Cryptography and Encoding
Hashing, encoding, and random utilities:
- **Crypto**: hashDJB2, hashSDBM, hashFNV1a, checksum, crc32
- **Base64**: encode, decode
- **Hex**: encode, decode
- **UUID**: v4 generation, validation
- **Random**: bytes, hex, alphanumeric, choice, shuffle

```prox
let message = "Hello, World!";
print(Crypto.hashDJB2(message));
print(Crypto.hashFNV1a(message));

let uuid = UUID.v4();
print(uuid); // "550e8400-e29b-41d4-a716-446655440000"

let randomHex = Random.hex(16);
let randomStr = Random.alphanumeric(10);
```

#### `regex.prox` - Pattern Matching and Text Processing
Regular expressions and text utilities:
- **Regex class**: Pattern matching with test, match, replace, split
- **Pattern validation**: isEmail, isURL, isIPv4, isPhone, isCreditCard
- **Character checks**: isAlphanumeric, isNumeric, isAlpha, isLowercase, isUppercase
- **Extraction**: extractNumbers, extractWords
- **TextUtils**: removeWhitespace, normalizeWhitespace, truncate, wordWrap, slugify, levenshtein

```prox
print(Pattern.isEmail("test@example.com"));     // true
print(Pattern.isURL("https://example.com"));    // true
print(Pattern.isIPv4("192.168.1.1"));          // true

let nums = Pattern.extractNumbers("Price: $99.99");
print(nums); // [99.99]

let slug = TextUtils.slugify("Hello World!");   // "hello-world"
let truncated = TextUtils.truncate("Long text...", 10, "...");
```

### Existing Libraries

#### `io.prox` - Input/Output
- print, println, error, readLine, flush
- Color constants and setColor

#### `json.prox` - JSON Parsing
- parse, stringify

#### `time.prox` - Time Functions
- now, sleep, clock, measure

#### `fs.prox` - File System Operations
- File reading, writing, and manipulation

#### `net.prox` - Network Operations
- HTTP requests and network utilities

#### `os.prox` - Operating System
- System information and operations

#### `sys.prox` - System Utilities
- Runtime and system utilities

#### `hash.prox` - Hashing
- Hash functions

#### `reflect.prox` - Reflection
- Runtime type inspection

## Usage

To use a library in your ProXPL code:

```prox
use std.lib.collections;
use std.lib.math;
use std.lib.datetime;

let stack = Stack();
let result = Math.factorial(5);
let now = DateTime.now();
```

## Testing

Run the comprehensive library test:

```bash
proxpl tests/test_libraries.prox
```

## Contributing

When adding new libraries:
1. Place them in `std/lib/`
2. Follow the existing naming conventions
3. Include comprehensive documentation
4. Add tests to verify functionality
5. Update this README

## License

Part of the ProXPL project. See main LICENSE file.
