# Bench Simple

This microbenchmark builds a CPU-bound chunk and runs it repeatedly against the VM to measure throughput.

Build and Run
-------------
1. Configure the project with CMake (from project root):

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target bench_simple
```

2. Run the benchmark:

```bash
./bench_simple
```
