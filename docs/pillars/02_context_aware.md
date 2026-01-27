# Pillar 2: Context-Aware Polymorphism

## Concept
Context-Aware Polymorphism allows functions and behavior to adapt dynamically based on the execution environment (e.g., Mobile vs. Cloud, Dev vs. Prod). This is achieved through Context-Oriented Programming (COP) constructs.

## Syntax

### Context & Layers
Define specialized behaviors inside named contexts and layers.

```javascript
context MobileMode {
    layer Display {
        func render() {
            print("Mobile-optimized View");
        }
    }
}
```

### Dynamic Activation
Enable a context for a specific block of code via the `activate` statement.

```javascript
activate(MobileMode) {
    render(); // Dispatches to the MobileMode layer implementation
}
```

## Usage
The ProXPL VM maintains an active context stack. When a function is called, the runtime dispatcher (LIFO) searches through the active layers for a specialized implementation before falling back to the global base function. This allows for clean, non-invasive behavioral shifts based on the runtime environment without complex conditional branching.
