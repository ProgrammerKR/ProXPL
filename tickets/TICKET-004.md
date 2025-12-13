# TICKET-004: Benchmark Suite

Goal
----
Create a reproducible micro-benchmark and end-to-end benchmark suite (fib, matrix multiply, JSON work, HTTP echo) and integrate into CI for regression detection.

Acceptance Criteria
-------------------
- Benchmarks run reproducibly and produce JSON reports.
- CI job runs benchmarks on schedule and fails PRs when regressions exceed threshold.
