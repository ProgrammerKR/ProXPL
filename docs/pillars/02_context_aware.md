# Pillar 2: Context-Aware Polymorphism

## Concept
Context-Aware Polymorphism allow functions and behavior to adapt dynamically based on the execution environment (e.g., Mobile vs. Cloud, Dev vs. Prod). This is achieved via the `@context` decorator.

## Syntax

### Context Decorators
```javascript
@context(env="cloud")
func connectDB() {
    // Cloud-specific connection
}

@context(env="mobile")
func connectDB() {
    // Local SQLite connection
}
```

## Usage
The runtime evaluates the current context (set via start flags or environment variables) and dispatches the correct function variant. This eliminates complex `if-else` chains for environment-specific logic.
