DECODER & DISPATCH SPEC

Purpose
- Describe the decoding API for mapping raw bytes to instruction fields and present a high-performance dispatch strategy for the VM, offering both computed-goto (where supported) and a portable switch-based fallback.

Decoder API (C prototypes)
- size_t decode_uleb128(const uint8_t *buf, size_t buf_len, uint64_t *out_value);
- size_t decode_sleb128(const uint8_t *buf, size_t buf_len, int64_t *out_value);
- int decode_instruction(const Chunk *chunk, size_t ip, uint8_t *out_opcode, uint8_t *out_operands_buf, size_t *out_operands_len);
  - decode_instruction reads opcode at ip, and leaves out_operands_buf pointing into chunk->code + ip_after_opcode, and out_operands_len set to number of bytes consumed for operands. It does not allocate memory.
- Helpers:
  - size_t operand_bytes_for_opcode(uint8_t opcode, const uint8_t *buf, size_t buf_len);

Decoding rules
- Read opcode (1 byte).
- For fixed-size operand sequences (e.g., three u8 regs) read known sizes directly.
- For variable-length fields (uleb/sleb) decode them using LEB128 readers advancing pointer accordingly.
- For address calculation (JMP), decode sleb128 offset and compute jump_target = current_ip + offset_after_operand_read + offset.

Dispatch Strategies
1) Computed Goto (gcc/clang)
- Mechanism: Use GCC's labels-as-values extension and an array of void* label addresses indexed by opcode.
- Advantages: minimizes branch misprediction overhead, faster than switch in many cases.
- Implementation sketch:
  - Build a static label* table where index = opcode.
  - In the interpreter loop: opcode = *ip++; goto *labels[opcode];
  - Each label contains the handler code and ends with a "continue" or "goto dispatch" to jump to the next instruction.
- Fallbacks for unknown opcodes: direct to default handler label which raises an error.

2) Switch-based fallback (portable)
- Use a `switch (opcode)` for each instruction, with each case performing operand decoding and execution.
- Easier to maintain and portable to compilers not supporting computed-goto.

Dispatch loop template
- Initialize VMState with chunk pointer and zeroed state.
- Loop:
  - Fetch opcode.
  - Decode operands inline (for performance) using local pointer increments and LEB128 reads.
  - Execute semantics (inline or via helper calls).
  - Update vm.ip to the next instruction address (or set to target for jumps).
- Example performance tips:
  - Inline decoding for common opcodes (CONST, ADD, CALL, JMP).
  - Use local variables for frequently accessed vm fields (stack pointer, regs pointer) to help optimizer.
  - Place hot opcode handlers early in dispatch table to reduce cache misses.
  - Use restrict-qualified pointers where applicable.

Inline Caches (IC) for property access (LOAD/GET)
- Monomorphic inline cache structure:
  - For a given GET_FIELD site, attach a small IC record:
    struct ICEntry {
      uint32_t field_name_const_idx;
      void *cached_shape; /* pointer to object's type/shape descriptor */
      uint32_t field_slot; /* known offset/slot index */
      uint8_t hit_count;
    };
  - On GET_FIELD:
    - Check object->shape == cached_shape:
      - If equal: directly load field at field_slot.
      - Else: perform full lookup, update ICEntry with new shape and slot, increment miss/hit counters.
- Storage:
  - IC entries are stored in a VM-wide table or embedded in the function's bytecode metadata. For compactness, store IC index in the bytecode (u16) so the VM can read and access vm->ic_table[ic_idx].
- Benefits:
  - Fast property access in hot paths (e.g., loops).
  - Low overhead: single pointer compare + direct load on hit.

Monomorphic IC lifecycle
- Initially ICEntry.cached_shape = NULL -> trigger full lookup and populate.
- If site sees polymorphism (many shapes), the implementation may:
  - Convert to polymorphic inline cache (small array), or
  - Deoptimize to a generic slow-path and mark site as megamorphic.

Example of attaching IC to LOAD/GET in bytecode
- GET_FIELD rA, rObj, field_const_idx is extended by optional uleb128 ic_index (0xFFFFFFFF indicates no IC).
- VM uses ic_index to locate IC entry; if ic_index indicates none, fallback to lookup without cache.

Error handling and unknown opcodes
- Decoder should return an error for unknown opcode.
- Optionally, an "unknown-op" compatibility mode could treat unknown opcodes as NOP, but that is unsafe for production.

### Contextual Dispatch (COP)
- The VM maintains a LIFO `activeContextStack`.
- Global variable lookups (`OP_GET_GLOBAL`) are preceded by a contextual lookup:
  - Iterate from top to bottom of `activeContextStack`.
  - For each `ObjContext`, search its `layers` for a method matching the lookup name.
  - If found, push the contextual method and immediately dispatch/return.
- This provides O(1) override capability for strategic system behaviors without modifying original source code.

Summary
- Computed-goto provides best raw performance on supported compilers.
- Switch is portable and easier to debug.
- Inline decoding + inline caching for property access and small register windows provide a favorable balance of speed and simplicity.
