ASR Demo: Build and Run Instructions
===================================

This demo contains a simple illustrative lock-free queue and a supervisor stub.

Build (requires g++ with C++17 and pthread support):

```powershell
g++ -std=c++17 -O2 -pthread runtime/asr_lockfree_queue.cpp runtime/asr_supervisor.cpp tests/asr_queue_test.cpp -o build\asr_test
```

Run:

```powershell
.\build\asr_test
```

Notes:
- This is a demonstration of the design pattern; not intended as a production-ready queue.
