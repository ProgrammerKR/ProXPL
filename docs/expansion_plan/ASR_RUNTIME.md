ASR Runtime: Supervisor + Checkpoint Microkernel
===============================================

Goal
----
Minimize runtime cost of Self-Healing (ASR) by keeping fast-path operations inline and low-overhead while providing deterministic, low-latency recovery for anomalies.

Key components
--------------
- ASR Supervisor (C++): pinned threads per NUMA node, event-driven, consumes repair queue.
- Checkpoint probes: compiler emits lightweight probes at safe points; probes snapshot only metadata (taint, intent, chrono expirations).
- Recovery handlers: C++ RAII-style handlers registered per type; fast-path: atomic state swap; slow-path: push to repair queue.
- Lock-free repair queue: MPMC ring buffer with priority buckets (critical/user/background).
- Adaptive sampling: supervisor reduces probe frequency under high load.

C++ ABI sketch
--------------
// asr_runtime.h -- minimal ABI
// (pseudo-C++ / header sketch)
typedef void (*asr_handler_fn)(void* obj, void* ctx);

extern "C" {
  void asr_register_handler(uint64_t type_id, asr_handler_fn handler);
  void asr_checkpoint(void* thread_state);
  void asr_report_anomaly(uint32_t code, void* data);
}

Design notes
------------
- ASR_CHECKPOINT lowered inline: a single atomic read of a thread-local flag, doing nothing when clear.
- Metadata-only checkpoints avoid copying heap; compiler ensures probes at function entries, loops, and allocation sites.
- Handlers are compiled C++ for determinism and performance; language-level handler registration is an intrinsic.

Recovery flow (fast path)
-------------------------
1. Anomaly detected (hardware/logic/runtime)
2. Thread marks object; attempts atomic state transition
3. If atomic succeeds, inline handler repairs and returns
4. If fail or long-running, push repair job to lock-free queue for supervisor

Why fits ProXPL
---------------
- Keeps overhead minimal by snapshotting intent/taint/TTL only.
- C++ fast-path handlers for systems performance; ProXPL maintains high-level semantics.
