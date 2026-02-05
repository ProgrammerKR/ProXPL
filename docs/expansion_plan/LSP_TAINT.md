LSP: Real-time Taint Highlighting & Actions
==========================================

Goals
-----
- Highlight tainted variables and propagation paths in the editor.
- Offer code actions to apply sanitizers or make raw access explicit.
- Correlate static taint analysis with runtime telemetry to reduce false positives.

Architecture
------------
- LSP Server: incremental parser + type checker + taint propagator
- Taint Symbol Table: per-file mapping from symbol -> taint metadata (origin, level, trace)
- Client: renders `tainted` semantic tokens and shows gutter icons; offers quick fixes via code actions

Protocol flow (summary)
-----------------------
1. `textDocument/didChange` -> Server updates AST incrementally
2. Server runs taint propagation and returns `publishDiagnostics` + `semanticTokens` with token type `tainted`
3. Client highlights variables and offers `codeAction` suggestions such as `Wrap with sanitizer.json_schema(...)`

Semantic tokens
---------------
- Token type: `tainted`
- Modifiers: `low`, `medium`, `high`, `untrusted`

UX examples
-----------
- Hover shows origin: `taint: network -> prox.net.shield(json) -> sanitized? false` and suggested sanitizers.
- Quick fix inserts the sanitizer call and updates the signature.

Implementation notes
--------------------
- Use incremental dataflow analysis to compute taint lattice cheaply on edits.
- Persist light snapshots for instant responsiveness.
- Provide an endpoint to ingest runtime ASR telemetry for correlation.
