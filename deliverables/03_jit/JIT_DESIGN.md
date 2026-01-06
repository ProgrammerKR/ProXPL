# JIT Compiler Design (LLVM-based) - Overview

Target: an LLVM-backed JIT that compiles hot functions/traces to native code and integrates with the interpreter.

High-level flow
- Frontend bytecode → High-level IR (typed SSA) → Optimizations (const-folding, type specialization) → LLVM IR → Native code.

IR translation notes
- Map ProXPL bytecode ops to typed IR ops (i32/float64/ptr) with explicit boxing/unboxing inserted only at boundaries.
- Represent closures with environment pointer + function pointer pair; lower closures to structs.

Sample lowering (arithmetic + call)
- Bytecode: PUSH_CONST a; PUSH_CONST b; ADD; CALL print, argc=1
- IR: tmp0 = load_const(a); tmp1 = load_const(b); tmp2 = add_f64(tmp0,tmp1); call print(tmp2)

Memory management
- Generated code will use runtime calling conventions to allocate and root values.
- Use stack maps and LLVM GC metadata or explicit root registration for conservative root scanning.

Integration plan
- Keep interpreter as fallback; implement OSR and tiering: interpreter → JIT for hot functions.  
- Use a call boundary ABI that allows compiled/native code to call interpreter functions and vice versa.

See `deliverables/04_jit_examples/` for prototype code and build instructions (to be added).
