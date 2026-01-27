# ProXPL Core Specification: Pillars 1-5

## 1. Intent-Oriented Programming (IOP)

### Concept
IOP decouples the *definition* of a capability from its *implementation*. The developer defines an `intent`, and the compiler or runtime resolves it to a concrete implementation based on environment, security constraints, or performance requirements.

### Syntax (EBNF)
```ebnf
intent_decl ::= "intent" identifier "(" [param_list] ")" [ "->" type ] ";"
resolver_decl ::= "resolver" identifier "matches" identifier "{" block "}"
```

**Example:**
```proxpl
// Definition
intent secure_verify(user) -> bool;

// Implementation (Provider A)
resolver Auth0Provider matches secure_verify {
    return auth0.verify(user.token);
}

// Implementation (Provider B - Local)
resolver LocalHashProvider matches secure_verify {
    return crypto.bcrypt_verify(user.hash, user.input);
}

// Usage
let is_valid = secure_verify(current_user); // Compiler picks best resolver
```

### Compiler Logic (The Solver)
1.  **Parsing:** `intent` nodes are treated as unlinked symbols.
2.  **Semantic Analysis:** The **IOP Resolver Pass** scans for all `resolver` blocks matching the signature.
3.  **Resolution Strategy:**
    *   **Static Resolution:** If `prople.toml` specifies a provider (e.g., `auth = "Auth0"`), the compiler hard-links the call site to `Auth0Provider`.
    *   **Dynamic Resolution:** If ambiguous, the compiler generates a vtable-like lookup where the "Best" provider is chosen at startup.

### Implementation Task (Rust - Compiler AST)
```rust
struct IntentNode {
    name: String,
    signature: FnSignature,
}

struct ResolverNode {
    target_intent: String,
    implementation: Block,
    cost_metric: u32,
}

fn resolve_intent(intent: &IntentNode, resolvers: &[ResolverNode], config: &Config) -> Option<ResolverNode> {
    // 1. Check strict config overrides
    if let Some(forced) = config.overrides.get(&intent.name) {
        return resolvers.find(|r| r.name == forced);
    }
    
    // 2. Default to lowest cost metric (static optimization)
    resolvers.iter().min_by_key(|r| r.cost_metric).cloned()
}
```

---

## 2. Context-Aware Polymorphism

### Concept
Functions execute different logic branches not based on arguments, but on system state (Context). This eliminates global `if (System.isOverloaded)` checks.

### Syntax
```ebnf
context_decl ::= "context" identifier "{" layer_decl* "}"
layer_decl   ::= "layer" identifier "{" func_decl* "}"
activate_stmt ::= "activate" [ "(" ] identifier [ ")" ] block
decorator    ::= "@" "context" "(" expression ")"
```

**Example (Layered COP):**
```proxpl
context MobileMode {
    layer Display {
       func render() {
           print "Rendering for mobile...";
       }
    }
}

activate(MobileMode) {
    render(); // Prints mobile version
}
```

**Example (Decorator COP):**
```proxpl
// Normal operation
@context(System.Load < 0.8)
func process_image(img) {
    return high_quality_resize(img);
}

// Under load, degrade gracefully
@context(System.Load >= 0.8)
func process_image(img) {
    return fast_bilinear_resize(img);
}
```

### runtime Behavior (Dispatch Table)
The runtime maintains a **Context Vector**, a thread-local or global bitmask representing current states (Power, Load, Security).
Function calls to `process_image` are indirect jumps through a **Context Dispatch Table (CDT)**.

### Implementation Task (C++ - Runtime Dispatch)
```cpp
// Context flags
enum ContextFlag { LOAD_HIGH = 1 << 0, BATTERY_LOW = 1 << 1 };
std::atomic<uint32_t> global_context;

// Dispatcher
typedef void (*ImgProcFn)(Image*);
ImgProcFn process_image_table[4]; // Indexed by flags

void init_dispatch() {
    process_image_table[0] = &high_quality_resize; // Normal
    process_image_table[1] = &fast_bilinear_resize; // LOAD_HIGH
    // ...
}

// Called at callsite: process_image(img) ->
// DOING_THIS_FAST:
inline void call_process_image(Image* img) {
    uint32_t ctx = global_context.load(std::memory_order_relaxed);
    process_image_table[ctx](img);
}
```

---

## 3. Autonomic Self-Healing Runtime (ASR)

### Concept
A runtime supervisor hierarchy similar to Erlang/Akka but native to the language control flow. Panics are not crashes; they are signals to the supervisor to intervene.

### Syntax
```ebnf
resilient_scope ::= "resilient" "{" statement_list "}" "recovery" "(" strategy ")" ";"
strategy ::= "restart" | "rollback" | "escalate"
```

**Example:**
```xpl
resilient {
    db.connect();
    db.query("SELECT *");
} recovery (restart: 3 times, then escalate);
```

### Runtime Logic (Supervisor Tree)
1.  **Scope Entry:** Pushes a `RecoveryFrame` onto the thread's Supervisor Stack.
2.  **Panic/Crash:** The signal handler (SIGSEGV/SIGABRT) or exception unwinder pauses unwinding at the nearest `RecoveryFrame`.
3.  **Heal:** The runtime executes the strategy. If `restart`, instruction pointer resets to the start of the block.

### Implementation Task (Rust - Runtime)
```rust
enum Strategy { Restart(usize), Eskalate }

struct SupervisorFrame {
    checkpoint: ContextSaveState, // Registers, Stack Ptr
    strategy: Strategy,
    retry_count: usize,
}

thread_local! {
    static SUPERVISOR_STACK: RefCell<Vec<SupervisorFrame>> = ...;
}

fn handle_panic() {
    let mut stack = SUPERVISOR_STACK.borrow_mut();
    if let Some(frame) = stack.last_mut() {
        match frame.strategy {
            Strategy::Restart(max) if frame.retry_count < max => {
                frame.retry_count += 1;
                longjmp(frame.checkpoint); // Jump back to start
            }
            _ => propagate_panic(),
        }
    }
}
```

---

## 4. Intrinsic Security (Security-as-Grammar)

### Concept
Information Flow Control (IFC) baked into the type system. Data originating from untrusted sources is `tainted<T>`. It cannot be used in sinks (Database, OS Shell) requiring `pure<T>` without sanitization.

### Syntax
```ebnf
type_modifier ::= "tainted" | "pure"
sanitize_op ::= "sanitize" "(" expr ")" "->" identifier
```

**Example:**
```xpl
// Input is inherently tainted
let user_input: tainted<string> = request.body;

// DB requires pure<string>
// db.execute(user_input); // COMPILER ERROR: Type Mismatch

// Must sanitize
let safe_sql: pure<string> = sanitize(user_input) using sql_escaper;
db.execute(safe_sql); // OK
```

### Compiler Logic (Taint Analysis)
1.  **Propagation:** `tainted<T> + pure<T> = tainted<T>`. Taint spreads like a virus.
2.  **Barrier:** Only `sanitize` blocks can cast `tainted<T>` to `pure<T>`.
3.  **Check:** Sinks are annotated with `requires: pure`.

### Implementation Task (Compiler Semantic Pass)
```rust
fn check_assignment(target_type: Type, source_type: Type) -> Result<(), Error> {
    if source_type.is_tainted() && !target_type.is_tainted() {
        return Err("Security Violation: Cannot assign tainted data to pure variable without sanitization.");
    }
    Ok(())
}
```

---

## 5. Chrono-Native Logic

### Concept
Data has a lifespan. Variables can expire, triggering garbage collection or logic shifts.

### Syntax
```ebnf
temporal_decl ::= "let" identifier ":" "temporal" "<" type ">" "=" expr "decay" duration ";"
check_expr ::= "if" "expired" "(" identifier ")" block
```

**Example:**
```xpl
// Auth token valid for 500ms
let token: temporal<string> = gen_token() decay 500ms;

// ... later ...
print(token); // If > 500ms, throws ExpiredAccessError or returns "decayed_value"
```

### Runtime Behavior (Temporal GC)
The allocator adds a `timestamp` header to every `temporal` allocation.
Dereferencing a temporal pointer checks `(now - alloc_time) > ttl`.

### Implementation Task (C++ - Smart Pointer)
```cpp
template<typename T>
struct Temporal {
    T value;
    uint64_t created_at;
    uint64_t ttl_ms;

    T& operator*() {
        if (now() - created_at > ttl_ms) {
            throw ExpiredException();
        }
        return value;
    }
};

// GC Logic
void collect_expired() {
    for (auto obj : heap) {
        if (obj.is_temporal && obj.is_expired()) {
            free(obj);
        }
    }
}
```
