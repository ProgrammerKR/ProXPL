ASR Runtime (Full): Prioritized Buckets and Supervisor
=====================================================

Summary
-------
This document expands the ASR microkernel idea to prioritized repair buckets, enabling critical repairs to be handled before user-visible and background repairs, reducing user-facing latency.

Design
------
- Three priority buckets: `Critical`, `UserVisible`, `Background`.
- Each bucket is a bounded MPMC ring (lock-free) to minimize contention.
- The supervisor loop polls buckets from high to low priority and processes jobs with minimal locking.
- Anomaly reporting (`asr_report_anomaly`) is best-effort non-blocking; under extreme pressure jobs may be demoted or dropped.

Fast-path semantics
------------------
- `ASR_CHECKPOINT` lowered to a single-thread-local atomic test to avoid unnecessary overhead.
- Handlers for common repairs should be registered and compiled to C++ for deterministic, low-overhead execution.

Integration notes
-----------------
- Compiler must emit checkpoints at safe points and lower ProXPL handler registration into the C++ ABI.
- Supervisor threads should be pinned per NUMA node and adjust polling frequency based on system load.

Further improvements
--------------------
- Hierarchical timing wheel to schedule delayed repair retries.
- Per-type rate-limiting and backoff strategies to avoid thrashing.
- Metrics and adaptive sampling to tune checkpoints and bucket sizes.
