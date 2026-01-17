# Pillar 6: Event-Driven Concurrency

## Concept
Event-Driven Concurrency provides native primitives for building distributed, node-based systems. It introduces `node` and `distributed type` declarations to model concurrent actors and shared data structures without external libraries.

## Syntax

### Node Declaration
```javascript
node Server {
   // Capabilities and internal state
}
```

### Distributed Type
```javascript
distributed type Ledger {
    // A data structure shared/replicated across nodes
}
```

## Usage
These constructs lay the groundwork for building distributed systems where concurrency is managed by the language runtime (actor model or similar) rather than manual thread management.
