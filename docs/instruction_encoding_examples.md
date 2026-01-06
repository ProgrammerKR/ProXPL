# instruction_encoding_examples.md

This file lists six ProXPL snippet examples and their emitted bytecode (hex) with annotated assembly.

1) Hello World (print string)
Source (pseudo-ProXPL):
- print("Hello, World!")

Constants:
0: "Hello, World!"
1: "print"  (native function reference name for test)

Bytecode (assembly):
0000: PUSH_CONST 0
0002: CALL AM_CONST const_idx=1 argc=1
0006: HALT

Hex bytes:
30 00 50 01 01 01 ff
Explanation:
- 0x30 (PUSH_CONST) + 0x00 (uleb128 const idx 0)
- 0x50 (CALL), 0x01 (AM_CONST), 0x01 (uleb128 const idx 1), 0x01 (argc)
- 0xFF HALT

2) Simple Arithmetic
Source:
- a = 2 + 3 * 4

Compiled to registers:
- consts: 0 -> 2, 1 -> 3, 2 -> 4
Assembly:
0000: PUSH_CONST 0
0002: PUSH_CONST 1
0004: PUSH_CONST 2
0006: CALL AM_CONST const_idx=... (for multiply)  ; in simple compiled register model we'd use regs; example simplified:
0008: HALT

Hex (example compact):
30 00 30 01 30 02 FF

3) Function Call (user function)
Source:
func add(x, y) { return x + y; }
z = add(10, 20)

Example compiled:
- function proto constant at const idx 2
Constants:
0: 10
1: 20
2: <FunctionProto add>

Assembly:
0000: PUSH_CONST 0
0002: PUSH_CONST 1
0004: PUSH_CONST 2
0006: CALL AM_STACK idx=0 argc=2
000A: HALT

Hex (example):
30 00 30 01 30 02 50 03 02 ff
Note: 0x03 used to denote AM_STACK here as an example, followed by stack index (0) encoded as uleb128, then argc 2.

4) Loop (sum 1..N)
Source:
sum = 0
i = 1
while (i <= N) { sum = sum + i; i = i + 1; }

Assembly sketch:
0000: PUSH_CONST sum_init (0)
0002: STORE_REG r0
0004: PUSH_CONST i_init (1)
0006: STORE_REG r1
0008: JMP    to_cond
0010: loop_body:
0010: LOAD_REG r2, r0
0012: LOAD_REG r3, r1
0014: ADD r2, r2, r3
0017: STORE_REG r0, r2
0020: LOAD_REG r3, r1
0022: PUSH_CONST 1
0024: ADD r3, r3, rX
0027: STORE_REG r1, r3
002A: to_cond:
002A: LOAD_REG r3, r1
002C: PUSH_CONST N
002E: LTE r3, r3, rConstN
0031: JMP_IF_TRUE offset_to_loop_body
0035: HALT

This shows use of reg operations and conditional jump (sleb128 offset).

5) Conditional (if)
Source:
if (x > 0) { print("positive"); } else { print("non-positive"); }

Constants:
0: "positive"
1: "non-positive"

Assembly:
0000: LOAD_REG r0, r_x
0002: PUSH_CONST 0
0004: GT r1, r0, rConst0
0007: JMP_IF_FALSE offset_else
0009: PUSH_CONST 0
000B: CALL AM_CONST const_idx=print argc=1
000F: JMP offset_end
0011: else_label:
0011: PUSH_CONST 1
0013: CALL AM_CONST const_idx=print argc=1
0017: end:
0017: HALT

6) Closure / Capture
Source:
func makeAdder(a) {
  return func(b) { return a + b; }
}
add5 = makeAdder(5)
result = add5(10)

Example function proto layout:
- Outer function proto contains inner function as a constant (function proto)
- CLOSURE const_idx(inner_proto), upvalue_count=1, upvalue descriptors: (is_local=1, index_of_a)

Assembly (simplified):
0000: PUSH_CONST const_inner_proto
0002: CLOSURE const_idx=1 upvalue_count=1 [is_local=1, index=0]
0006: STORE_REG r0  ; store created closure
0008: PUSH_CONST 5
000A: CALL AM_REG reg=0 argc=1
000D: HALT

Notes
- The hex sequences above are illustrative; actual emitted bytes will vary depending on exact LEB128 encodings and chosen addressing mode encodings.
- Use the provided disassembler `disasm.c` to decode real binary chunks into readable assembly.
