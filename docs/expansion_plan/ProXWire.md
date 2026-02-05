ProXWire — Intent-Oriented Web Framework (design)
=================================================

Overview
--------
ProXWire is an intent-first web framework: endpoints declare the "intent" they satisfy (domain + action + schema) rather than only a URL path. This enables precompiled sanitizer/serializer pipelines, fast dispatch, and scheduler prioritization by intent.

Request flow (text diagram)

Client -> Transport (HTTP/TCP) -> prox.net.shield (sanitize) -> Intent Matcher -> Middleware Chain -> Handler Execution (Context-aware) -> Response Serializer -> Transport

Core primitives
---------------
- Intent: {domain, action, accepts, produces, schema}
- Router: compiles an Intent Trie keyed by signature (not path) for O(1) dispatch in hot paths
- Middlewares: intent-aware (auth, rate-limit, quota)
- Handler execution: spawned as async task with `Context` (intent, priority_hint, ttl, taint_level)

Example (pseudo-ProXPL)

@intent(domain:"user", action:"create", accepts:"application/json")
async fn create_user(req: Sanitized[UserCreate], ctx: Context) -> JSON {
  // ctx.priority_hint influences scheduler lane
  let u = await users.create(req)
  return JSON(u)
}

Router registration

router = ProXWire.Router()
router.add_intent(create_user)

Performance notes
-----------------
- Intent-based dispatch removes per-request reflection and mapping overhead.
- Precompiled sanitizer & serializer pipelines reduce runtime branching.
- Works tightly with Context-Aware scheduler to allocate fast lanes for user-visible intents.

Interservice intent discovery
-----------------------------
Services declare handled intents to a registry; an intent-oriented proxy routes typed messages between services, enabling typed RPC.
