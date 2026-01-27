# ProXPL Multi-Paradigm Architecture

ProXPL is a high-performance systems programming language that uniquely combines three powerful programming paradigms: **Object-Oriented Programming (OOP)**, **Intent-Oriented Programming (IOP)**, and **Context-Oriented Programming (COP)**.

## 1. Object-Oriented Programming (OOP)

ProXPL provides a robust class-based OOP model, similar to modern languages like Java, C#, or TypeScript.

### Core Features:
- **Classes & Inheritance**: Use the `class` and `extends` keywords.
- **Interfaces**: Define contracts using the `interface` keyword and implement them with `implements`.
- **Encapsulation**: Access modifiers `public`, `private`, and `protected`.
- **Abstraction**: `abstract` classes and methods.
- **Polymorphism**: Traditional method overriding and interface implementation.

```proxpl
class Animal {
    func makeSound() {
        print "Generic sound";
    }
}

class Dog extends Animal {
    func makeSound() {
        print "Woof!";
    }
}
```

---

## 2. Intent-Oriented Programming (IOP)

Intent-Oriented Programming shifts the focus from *how* to solve a problem to *what* problem needs to be solved.

### Core Features:
- **Intent**: An abstract declaration of a goal or service requirement.
- **Resolver**: A concrete implementation that satisfies an intent.
- **Decoupling**: Callers request an intent, and the runtime (or linker) resolves it to the best available implementation.

```proxpl
intent Storage(data: string, path: string);

resolver LocalStorage matches Storage {
    // Implementation for local disk
}

resolver CloudStorage matches Storage {
    // Implementation for cloud sync
}

// Usage (Runtime selection)
Storage("database_backup", "/etc/config");
```

---

## 3. Context-Oriented Programming (COP)

Context-Oriented Programming allows a program to adapt its behavior dynamically based on its execution environment (context).

### Core Features:
- **Context**: A top-level container for behavioral variations.
- **Layer**: A collection of method overrides that are active only when the context is enabled.
- **Activation**: The `activate` block to apply layers to a scope.

```proxpl
context PerformanceMode {
    layer MathOps {
        func multiply(a, b) {
            // High-performance GPU implementation
            return gpu_mul(a, b);
        }
    }
}

// Execution
activate PerformanceMode {
    let x = multiply(10, 20); // Uses PerformanceMode implementation
}
```

---

## Why Multi-Paradigm?

By integrating these three paradigms, ProXPL allows developers to:
1. Use **OOP** for structural organization and code reuse.
2. Use **IOP** for service discovery and behavioral decoupling.
3. Use **COP** for cross-cutting concerns and dynamic adaptation.

This combination makes ProXPL exceptionally well-suited for complex systems like distributed applications, AI-integrated software, and highly adaptive environments.
