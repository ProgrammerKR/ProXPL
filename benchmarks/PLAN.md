# ProXPL Benchmarking Plan

## 1. Objectives
- Measure baseline performance of the current interpreter.
- Track improvements with the new Register VM and JIT.
- Compare against CPython 3.11, Lua 5.4, and Node.js.

## 2. Benchmark Suite Strategy

### 2.1 Microbenchmarks (CPU Core)
Focus on specific VM optimizations (dispatch, arithmetic, calls).

| Benchmark | Description | Target Speedup (vs Current) |
|-----------|-------------|----------------------------|
| `fib.prox` | Recursive Fibonacci (Call overhead) | 3x |
| `loop_sum.prox` | Tight loop addition (1M iters) | 5x |
| `array_access.prox` | Read/Write array elements | 2x |
| `dict_get.prox` | Dictionary lookups (String keys) | 2x |

### 2.2 Macrobenchmarks (Real Workload)
| Benchmark | Description | Target Speedup |
|-----------|-------------|----------------|
| `json_bench.prox` | Parse/Serialize simulated JSON | 3x |
| `http_sim.prox` | Simulated request routing/handling | 2x |
| `nbody.prox` | Physics simulation (Float math) | 10x (with JIT) |

## 3. Tools & Methodology

We will use `hyperfine` for statistical execution time measurement.

### Pre-requisites
- `hyperfine` (install via `cargo install hyperfine` or `apt-get install hyperfine`)
- `python3` (CPython 3.11+)
- `lua` (Lua 5.4)
- `node` (Node.js 20+)

### Execution Commands

Run the following command from the repository root:

```bash
# Example: Running the Fibonacci Benchmark
hyperfine --warmup 3 \
  "bin/proxpl run benchmarks/fib.prox" \
  "python3 benchmarks/reference/fib.py" \
  "lua benchmarks/reference/fib.lua" \
  --export-markdown benchmarks/results/fib_results.md
```

## 4. Directory Structure

```
benchmarks/
├── micro/
│   ├── fib.prox
│   ├── loop_sum.prox
│   └── ...
├── macro/
│   └── nbody.prox
├── reference/  <-- Equivalents in Py/Lua/Node
│   ├── fib.py
│   ├── fib.lua
│   └── ...
└── run_all.sh
```
