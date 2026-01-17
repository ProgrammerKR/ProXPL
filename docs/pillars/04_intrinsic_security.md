# Pillar 4: Intrinsic Security

## Concept
Intrinsic Security treats data safety as a type-system feature. It introduces Taint Analysis directly into the language, forcing developers to explicitly sanitize untrusted inputs before using them in sensitive contexts.

## Syntax

### Tainted Types
```javascript
policy NoSQLInjection { ... }
```
(Note: Policy definition syntax is reserved for future expansion)

### Usage
```javascript
// 'input()' returns a tainted string by default (conceptually)
let userInput = input("Enter query: "); 

// Attempting to use 'userInput' in a sensitive sink (like exec) would warn/error
// exec(userInput); // Error: Tainted data in sensitive sink

// Must sanitize first
let cleanInput = sanitize(userInput);
exec(cleanInput); // Safe
```

### Keywords
- `tainted`: (Internal type attribute) Marks data as untrusted.
- `sanitize(expr)`: Primitive to strip taint from a value.
- `policy`: Define security boundaries.
