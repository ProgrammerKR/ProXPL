TensorGraph — Tensor-first Game / Simulation Engine
==================================================

Concept
-------
Model the entire simulation state as tensors (positions Nx3, velocities Nx3, mass Nx1, shapes encoded) and use hardware-accelerated tensor ops for collision detection, constraint solving, and neural controllers.

Pipeline (text diagram)

Scene -> Tensor Builder -> Physics Kernel (tensorized ops) -> Collision Layer (batched pairwise tests) -> Solver (iterative GPU-accelerated) -> Output

Example pseudocode (ProXPL)

fn physics_step(state: TensorState, dt: Float) -> TensorState {
  let forces = compute_forces(state)           // tensorized
  let accel = forces / state.mass
  state.vel += accel * dt
  state.pos += state.vel * dt
  state = resolve_collisions(state)            // batched tensor ops
  return state
}

Collision idea
--------------
- Use spatial hashing as tensor ops: cell_indices = floor(pos * grid_scale) -> one-hot encode -> batched group-by to limit pairwise checks.
- Compute pairwise distances using batched matmul patterns where possible.

Why this fits ProXPL
--------------------
- Reuses AI-Native tensor infrastructure for high throughput simulation.
- Enables hybrid ML+physics systems: neural controllers trainable in the same runtime.
