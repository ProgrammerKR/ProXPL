Title: ProXPL Bytecode Specification
Version: 1.0
Date: 2025-12-11

Overview
- Purpose: Define a compact, extensible, and fast binary bytecode format and runtime instruction semantics for the ProXPL language.
- Philosophy: Hybrid "register-lite + stack" model: a small fast register window for locals and arithmetic, with an operand stack for calls/temporaries and a compact bytecode encoding optimized for code density and decoding speed.
- Goals: portability (C99, explicit sizes), forward compatibility (reserved opcode space, versioning), fast VM dispatch (computed goto or switch fallback), and concise constant references using LEB128.

High-level Design
- Bytecode model:
  - Each function compiles to a "Chunk" containing a byte buffer of opcodes + operands and a constants table.
  - Functions are first-class objects with their own chunks and upvalue descriptors.
  - The VM executes bytecode using frames; frames hold a register window base and operand stack pointer.
- Encoding:
  - All fixed-size fields use explicit integer widths and little-endian order in files.
  - Immediate small integers and variable-size indices use unsigned LEB128 (ULEB128); signed immediates use signed LEB128 (SLEB128).
  - Opcodes are 1 byte (uint8_t). Operands follow per-opcode variable layouts.
- File format:
  - ".proxbc" container: header + function sections + constants + debug (optional).
  - Header contains magic, version, endianness, and section offsets.

Instruction Set (Summary)
- Categories:
  - Arithmetic: ADD, SUB, MUL, DIV, MOD, NEG
  - Comparison: EQ, NEQ, LT, LTE, GT, GTE
  - Logic: AND, OR, NOT
  - Control Flow: JMP, JMP_IF_TRUE, JMP_IF_FALSE, CALL, RET, FORWARD_RET
  - Stack/Register Ops: PUSH_CONST, LOAD_REG, STORE_REG, LOAD_UPVALUE, STORE_UPVALUE
  - Memory/Object Ops: NEW_OBJ, GET_FIELD, SET_FIELD, INDEX_GET, INDEX_SET
  - Function/object ops: CLOSURE, CLOSE_UPVALUE, MAKE_FUNCTION
  - Debug ops: DBG_LINE, DBG_LOC, NOP
  - VM Control: HALT, BREAKPOINT
- Each opcode has a name, numeric code, and operand format described below.

Operand types
- u8: 1 byte unsigned
- u16: 2 bytes unsigned (little-endian)
- u32: 4 bytes unsigned (little-endian)
- uleb128: variable-length unsigned LEB128
- sleb128: variable-length signed LEB128
- const_idx: index into the constants table (encoded as uleb128)
- reg: local register index (u8)
- arg_count: u8 or uleb128 depending on opcode

Register-lite + Stack hybrid model
- Each frame exposes a small contiguous register window indexed by reg indices 0..R-1 (R typically 16).
- Many arithmetic ops operate on registers for speed: e.g., ADD rA, rB, rC  ; rA = rB + rC
- The stack is used for pushing temporaries, function call arguments beyond the register window, and return values.
- Calling convention: The caller places arguments into consecutive registers starting at frame base+0 (or pushes onto stack if exceed registers). CALL encodes callee and arg_count; callee may be a constant, register, or expression producing a function object.

Instruction Encoding Rules
- Basic instruction format:
  - [opcode: u8] [operand bytes...]
- Variable-length operands follow LEB128 rules for compactness:
  - Use ULEB128 for constant indices, jump offsets (signed relative encoded as SLEB128 where jumps can be negative).
  - Use SLEB128 for signed immediates.
- Multi-byte fixed integers are stored little-endian.

Opcode numbering and reserved regions
- 0x00..0x1F: Core arithmetic/logic/control
- 0x20..0x3F: Stack/register ops
- 0x40..0x5F: Object and memory ops
- 0x60..0x7F: Function/closure ops
- 0x80..0x9F: Debug ops
- 0xA0..0xFF: Reserved for future extensions and native extensions

Detailed Instruction Set
- Format style uses compact pseudo-syntax:
  - opcode operands  ; semantics

Core arithmetic (register ops)
- OP_ADD rA, rB, rC
  - opcode: 0x01
  - operands: u8 rA, u8 rB, u8 rC
  - semantics: rA = rB + rC
- OP_SUB rA, rB, rC (0x02)
- OP_MUL rA, rB, rC (0x03)
- OP_DIV rA, rB, rC (0x04)
- OP_MOD rA, rB, rC (0x05)
- OP_NEG rA, rB (0x06) ; rA = -rB

Comparison (register ops)
- OP_EQ rA, rB, rC (0x10) ; rA = (rB == rC)
- OP_NEQ rA, rB, rC (0x11)
- OP_LT rA, rB, rC (0x12)
- OP_LTE rA, rB, rC (0x13)
- OP_GT rA, rB, rC (0x14)
- OP_GTE rA, rB, rC (0x15)

Logic
- OP_NOT rA, rB (0x20) ; rA = !rB
- OP_AND rA, rB, rC (0x21) ; rA = rB && rC
- OP_OR rA, rB, rC (0x22) ; rA = rB || rC

Stack / Constants / Registers
- OP_PUSH_CONST const_idx (0x30)
  - operands: uleb128 const_idx
  - semantics: push constants[const_idx] onto operand stack
- OP_LOAD_REG rA, reg (0x31)
  - operands: u8 rA, u8 reg
  - semantics: rA = frame->regs[reg]
- OP_STORE_REG reg, rA (0x32)
  - operands: u8 reg, u8 rA
  - semantics: frame->regs[reg] = rA
- OP_POP (0x33) ; pops from operand stack and discards
- OP_DUP (0x34) ; duplicates top of operand stack

Control flow
- OP_JMP offset (0x40)
  - operands: sleb128 offset (relative signed offset from next instruction)
- OP_JMP_IF_TRUE offset (0x41) ; conditional on truthiness of top of stack (pop)
- OP_JMP_IF_FALSE offset (0x42)
- OP_CALL target, argc (0x50)
  - operands: operand-coded target (see addressing modes) + u8 argc
  - semantics: push return frame, jump to function; return value pushed
- OP_RETURN (0x51)
  - operands: u8 ret_reg_count  ; number of registers used for return (0..)
  - semantics: return values are left in registers 0..ret_reg_count-1 of callee; runtime moves them to caller
- OP_TAIL_CALL target, argc (0x52) ; tail-call elimination friendly

Addressing Modes (how to reference a function/callee or value)
- A small addressing prefix byte follows for opcodes needing a "value" operand:
  - AM_CONST (0x01) + const_idx (uleb128)
  - AM_REG   (0x02) + u8 reg_index
  - AM_STACK (0x03) + uleb128 stack_slot_index
- Example CALL: [0x50] [addr_mode:0x01] [const_idx: uleb128] [argc:u8]

Function & Closure ops
- OP_MAKE_FUNCTION const_idx (0x60)
  - operands: const_idx -> constant contains function prototype / function descriptor
  - semantics: create a function object, push to stack
- OP_CLOSURE const_idx, upvalue_count (0x61)
  - operands: const_idx (function proto), uleb128 upvalue_count, then for each upvalue: u8 is_local (0/1), u8 index
- OP_CLOSE_UPVALUE reg (0x62) ; close upvalues referencing this reg
- OP_LOAD_UPVALUE rA, upvalue_index (0x63)
- OP_STORE_UPVALUE upvalue_index, rA (0x64)

Object/Memory ops
- OP_NEW_ARRAY rA, length_reg (0x70) ; create array of size in length_reg, result in rA
- OP_INDEX_GET rA, rObj, rIndex (0x71) ; rA = rObj[rIndex]
- OP_INDEX_SET rObj, rIndex, rValue (0x72) ; rObj[rIndex] = rValue
- OP_GET_FIELD rA, rObj, field_const_idx (0x73) ; field name is constant string in const_idx
- OP_SET_FIELD rObj, field_const_idx, rValue (0x74)
- OP_CONTEXT const_idx (0x75) ; create context from name at const_idx, push to stack
- OP_LAYER const_idx (0x76)   ; create layer from name at const_idx, push to stack
- OP_ACTIVATE (0x77)           ; pop context from stack and activate
- OP_END_ACTIVATE (0x78)       ; deactivate top context

Debug & Meta
- OP_DBG_LINE uleb128 line (0x90)
- OP_DBG_LOC uleb128 filename_const_idx, uleb128 line (0x91)
- OP_NOP (0xFE)
- OP_HALT (0xFF)

Constants Table Format
- A function chunk contains a constants table: an array of "Constant" entries.
- Constant entry:
  - type: u8 (0x01 = Number, 0x02 = String, 0x03 = FunctionProto, 0x04 = Bool, 0x05 = Null)
  - payload:
    - Number: IEEE-754 double (8 bytes little-endian)
    - String: uleb128 length, UTF-8 bytes
    - FunctionProto: serialized function prototype (arity, bytecode length, constants, upvalue count, debug metadata)
    - Bool: 1 byte (0 or 1)
    - Null: no payload
- Semantics:
  - const_idx in bytecode indexes into this table.
  - Constants are immutable and deduplicated at compile-time where feasible.

Function Object Layout (in-memory)
- ObjFunction:
  - uint32_t arity
  - uint16_t max_regs (register window size used by function)
  - Chunk chunk (owns its own constants)
  - upvalue_count (u16)
  - upvalue descriptors array: each descriptor indicates whether closed over local (is_local) and index.
- On serialization, FunctionProto constant contains the function's chunk and constant table; on deserialization, loader reconstructs ObjFunction.

Binary chunk (.proxbc) Example Header (file-level)
- Magic: 4 bytes "PROX"
- Version: u16 (0x0001)
- Endianness: u8 (1 = little, 2 = big)
- Reserved: u8 (padding)
- Sections:
  - u32 offset_functions
  - u32 offset_constants
  - u32 offset_debug
  - u32 reserved
- Sections layout:
  - At offset_functions: a sequence of function records: each function record: uleb128 name_const_idx (0 for anonymous), uleb128 arity, uleb128 max_regs, uleb128 bytecode_length, bytecode bytes, uleb128 constants_count, serialized constants...
  - Debug section optional.

Encoding Examples
- Short example: OP_ADD r0,r1,r2 encoded as:
  - 0x01 0x00 0x01 0x02
- Example: PUSH_CONST const_idx 5 encoded as:
  - 0x30 [uleb128(5)] -> bytes: 0x30 0x05

LEB128 Rationale
- LEB128 is chosen because it compresses small integers (typical constant indices and jump offsets) into 1 byte frequently while preserving the ability to encode large values and negative signed offsets (SLEB128) without fixed-size waste.
- It trades a small decode cost for code-size and cache benefits.

Decoding Rules
- Decoder reads opcode (u8) then consults a decode table for operand layout.
- For offsets (JMP), decoder reads SLEB128 and computes destination by adding offset to current instruction pointer after operand read.
- Constant lookups use the decoded const_idx to read from the chunk->constants array.

Forward Compat & Versioning
- Header contains version number; VM checks compatibility and rejects incompatible files.
- Opcode table reserves high opcode values for future extensions; decoder should treat unknown opcode as an error or NOP depending on a compatibility flag.

Disassembly
- A disassembler can decode chunk bytes using the decode rules and print annotated assembly lines with byte offsets, opcodes, operands, and constant annotations.

Sample bytecode snippets and annotated hex are provided in a companion file.

End of BYTECODE_SPEC.md
