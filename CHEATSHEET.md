# ProXPL Cheatsheet

A quick reference guide for the ProX Programming Language (ProXPL).

## 1. Basics

### Comments
```javascript
// Single-line comment
/* Multi-line
   comment */
```

### Variables
ProXPL uses `let` for mutable variables and `const` for constants.
```javascript
let x = 10;
x = 20;

const PI = 3.14;
// PI = 3.14159; // Error: Assignment to constant
```

### Data Types
- **Primitive**: `int`, `float`, `bool`, `string`, `null`.
- **Composite**: `List`, `Dictionary` (Map).

```javascript
let isDone = true;
let count = 42;
let price = 19.99;
let name = "ProX";
let empty = null;
let list = [1, 2, 3];
// let map = {"key": "value"}; // (Syntax supported in some contexts)
```

## 2. Control Flow

### If-Else
```javascript
if (score > 90) {
    print("Excellent");
} else if (score > 50) {
    print("Pass");
} else {
    print("Fail");
}
```

### Loops
```javascript
// While Loop
let i = 0;
while (i < 5) {
    print(i);
    i = i + 1;
}

// For Loop
for (let j = 0; j < 5; j = j + 1) {
    print(j);
}
```

## 3. Functions

### Declaration
```javascript
func add(a, b) {
    return a + b;
}

let sum = add(5, 3);
```

### Arrow Functions (Short syntax)
```javascript
func square(x) => x * x;
```

## 4. Object-Oriented Programming

### Classes
```javascript
class Animal {
    func speak() {
        print("Generic sound");
    }
}

class Dog extends Animal {
    func speak() {
        print("Woof!");
    }
}
```

### Instantiation
```javascript
let d = new Dog();
d.speak(); // Output: Woof!
```

### Usage of `this`
```javascript
class Person {
    func setAge(a) {
        this.age = a;
    }
}
```

## 5. Context-Oriented Programming (COP)

### Context & Layers
```javascript
context MobileMode {
    layer Display {
        func render() {
            print("Mobile UI");
        }
    }
}
```

### Dynamic Activation
```javascript
activate(MobileMode) {
    render(); // Dispatches to MobileMode.Display.render
}
```

## 6. Standard Library

### IO (`std.io`)
```javascript
print("Hello World");       // Native print
// or
use std.io;
IO.println("Hello World");
let name = IO.readLine();
```

### Math (`std.math`)
```javascript
use std.math;

let maxVal = Math.max(10, 20);
let rand = Math.random(); 
let root = Math.sqrt(16);
```

### Native Functions
Built-in global functions available without imports:
- `print(msg)`
- `input(prompt)`
- `to_string(value)`
- `clock()` - Returns current time
- `random(min, max)` (If available in specific implementations)

## 7. Operators
- **Arithmetic**: `+`, `-`, `*`, `/`, `%`
- **Comparison**: `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Logical**: `&&`, `||`, `!`

## 8. Example Program
```javascript
// fib.prox
func fib(n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}

func main() {
    print(fib(10));
}

main();
```
