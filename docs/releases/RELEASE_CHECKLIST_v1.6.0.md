# ProXPL v1.6.0 Release Checklist

## P0: Implementation Criteria
- [x] Intents: End-to-end declaration → registration → resolution → execution
- [x] Contexts: Declaration → type checking → activation → dynamic layer resolution
- [x] ASR (Advanced State Recovery): Nested resilient blocks → exception unwinding

## P1: Validation Suite
- [x] Create `test_asr_recovery.prox` (Passes)
- [x] Create `test_asr_edge_cases.prox` (Passes)
- [x] Create `test_intents_advanced.prox` (Passes)
- [x] Create `test_intents_errors.prox` (Passes)
- [x] Create `test_intents_gc.prox` (Passes)
- [x] Create `test_contexts_layers.prox` (Passes)
- [x] Create `test_contexts_errors.prox` (Passes)

## P2: Benchmarks
- [x] Execution Time: Compare Direct Call vs Intent Resolver Dispatch (Minimal overhead confirmed)
- [x] Memory: Track Peak Working Set size of VM during GC test

## P3: Release Engineering
- [x] Global Version Bump to `1.6.0` (CMakeLists.txt, common.h, setup.iss)
- [x] Automate verification script (`scripts/verify_release.py`)
- [x] `CHANGELOG.md` updated
- [x] `docs/VERSION_UPGRADE_GUIDE_v1.6.0.md` updated
- [x] `IMPLEMENTATION_STATUS.md` marked as `IMPLEMENTED`
