ProXWire Runtime: Implementation Sketch
======================================

This document expands the ProXWire design with more implementation details and integration points with `prox.net.shield` and the Context-Aware scheduler.

Dispatch
--------
- Intent signature: canonical string `domain:action:accepts` used as a trie key.
- Router stores precompiled sanitizer & serializer pipelines keyed by intent.
- Dispatch path:
  1. Transport receives bytes -> wraps as `Tainted[Bytes]` with metadata.
  2. `prox.net.shield.auto_sanitize` runs pipeline for intent; returns `Sanitized[T]`.
  3. Dispatcher schedules handler with `Context`.

Scheduler integration
---------------------
- Handlers are scheduled with `Context` describing `intent`, `priority_hint`, `ttl`, and `taint_level`.
- Fast lanes: runtime maintains dedicated worker pools per intent priority.

Optimizations
-------------
- Precompile sanitizer pipeline into an efficient C++ fast-path for common schemas.
- Inline intent matching where header `X-Intent` exists, fallback to content-based inference.

Example (lowering sketch)
-------------------------
// High-level route
@intent(domain:"user", action:"create", accepts:"application/json")
async fn create_user(req: Sanitized[UserCreate], ctx: Context) -> JSON { ... }

// Compiler output (sketch):
// 1) Register intent signature and sanitizer at module init
prox.net.shield.register_sanitizer("user:create:application/json", compiled_pipeline)
ProXWire.Router.register_handler("user:create:application/json", &create_user_wrapper)

// 2) Wrapper ensures typed call
fn create_user_wrapper(raw_req, ctx) {
  let sanitized = prox.net.shield.auto_sanitize(raw_req.tainted, "user:create:application/json")
  if sanitized.is_err() { return Response(400) }
  return create_user(sanitized.unwrap(), ctx)
}
