# Quick Start Guide

Get started with ProXPL in 10 minutes.

## 1. Install

Download binaries or build from source (see [Build Guide](BUILD_GUIDE.md)).

## 2. Write Your First Program

```javascript
func main() {
    print("Hello, ProXPL!");
}

main();
```

## 3. Run It

```bash
prm run hello.prox
```

## 4. Explore Features

### Variables and Types
```javascript
let count = 42
let price = 19.99
let active = true
let message = "Hello"
```

### Functions
```javascript
func fibonacci(n) {
    if (n <= 1) return n
    return fibonacci(n - 1) + fibonacci(n - 2)
}
```

### Collections
```javascript
let numbers = [1, 2, 3, 4, 5]
let config = {"host": "localhost"}
```

### Modules
```javascript
use std.math
let result = sqrt(16)
```

## Next Steps

- [Language Reference](../language/REFERENCE.md)
- [Standard Library](../standard-library/REFERENCE.md)
- [Tutorials](../tutorials/README.md)
