# ProXPL Language Syntax

## Variables
```prox
let x = 10;
const PI = 3.14;
let name = "ProXPL";
let isValid = true;
```

## Control Flow
```prox
if (x > 5) {
    print("X is big");
} else {
    print("X is small");
}

while (x > 0) {
    x = x - 1;
}

for (let i = 0; i < 10; i = i + 1) {
    print(i);
}
```

## Functions
```prox
func add(a, b) {
    return a + b;
}

let result = add(5, 3);
```

## Classes
```prox
class Dog {
    func bark() {
        print("Woof!");
    }
}

let myDog = Dog();
myDog.bark();
```
