/*
  Prototype Register-Based VM for ProXPL
  --------------------------------------
  Structure: 32-bit instructions (Opcode:8, A:8, B:8, C:8)
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// --- Instruction Format ---
// | Opcode (8) | A (8) | B (8) | C (8) |
// A: Destination Register
// B: Source Register 1
// C: Source Register 2 / Immediate

typedef uint32_t Instruction;

#define OP_MASK 0xFF
#define REG_MASK 0xFF

#define GET_OP(i)   ((i) & OP_MASK)
#define GET_A(i)    (((i) >> 8) & REG_MASK)
#define GET_B(i)    (((i) >> 16) & REG_MASK)
#define GET_C(i)    (((i) >> 24) & REG_MASK)

#define MK_INS(op, a, b, c) \
    ((op) | ((a) << 8) | ((b) << 16) | ((c) << 24))

// --- Opcodes ---
enum OpCode {
    OP_HALT = 0,
    OP_LOADK, // R[A] = Consts[B]
    OP_MOV,   // R[A] = R[B]
    OP_ADD,   // R[A] = R[B] + R[C]
    OP_SUB,
    OP_PRINT  // print R[A]
};

// --- VM State ---
#define MAX_REGS 256
#define MAX_CONSTS 256

typedef struct {
    double numbers[MAX_CONSTS];
} ConstTable;

typedef struct {
    Instruction* code;
    size_t count;
    ConstTable* consts;
} ProtoChunk;

typedef struct {
    double registers[MAX_REGS]; // Simplified Value type for prototype
    Instruction* ip;
} RegisterVM;

// --- Interpreter Loop ---
void run_register_vm(RegisterVM* vm, ProtoChunk* chunk) {
    vm->ip = chunk->code;
    
    printf("Starting Register VM execution...\n");

    for (;;) {
        Instruction ins = *vm->ip++;
        uint8_t op = GET_OP(ins);
        
        // Computed goto would go here in production
        switch (op) {
            case OP_HALT:
                printf("HALT encountered.\n");
                return;

            case OP_LOADK: {
                uint8_t target = GET_A(ins);
                uint8_t k_idx = GET_B(ins);
                vm->registers[target] = chunk->consts->numbers[k_idx];
                // printf("LOADK R[%d] = %f\n", target, vm->registers[target]);
                break;
            }

            case OP_MOV: {
                uint8_t dest = GET_A(ins);
                uint8_t src = GET_B(ins);
                vm->registers[dest] = vm->registers[src];
                break;
            }

            case OP_ADD: {
                uint8_t dest = GET_A(ins);
                uint8_t src1 = GET_B(ins);
                uint8_t src2 = GET_C(ins);
                // Type checking would happen here in full VM
                vm->registers[dest] = vm->registers[src1] + vm->registers[src2];
                // printf("ADD R[%d] = %f + %f = %f\n", dest, vm->registers[src1], vm->registers[src2], vm->registers[dest]);
                break;
            }

            case OP_PRINT: {
                uint8_t src = GET_A(ins);
                printf("OUT: %f\n", vm->registers[src]);
                break;
            }

            default:
                printf("Unknown Opcode: %d\n", op);
                return;
        }
    }
}

// --- Test Driver ---
int main() {
    // Defines a simple program:
    // val1 = 10.5
    // val2 = 20.5
    // result = val1 + val2
    // print result
    
    Instruction code[] = {
        MK_INS(OP_LOADK, 0, 0, 0), // R0 = Const[0] (10.5)
        MK_INS(OP_LOADK, 1, 1, 0), // R1 = Const[1] (20.5)
        MK_INS(OP_ADD, 2, 0, 1),   // R2 = R0 + R1
        MK_INS(OP_PRINT, 2, 0, 0), // PRINT R2
        MK_INS(OP_HALT, 0, 0, 0)
    };

    ConstTable constants;
    constants.numbers[0] = 10.5;
    constants.numbers[1] = 20.5;

    ProtoChunk chunk = { .code = code, .count = 5, .consts = &constants };
    RegisterVM vm;

    run_register_vm(&vm, &chunk);

    return 0;
}
