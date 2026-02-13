// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
//

/*
  Disassembler utility: reads a chunk and prints a human-readable
  listing of instructions with constant annotations.
*/
#include "../../include/bytecode.h"
#include "../../include/value.h"
#include "../../include/object.h"
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

static void print_value(Value v) {
    if (IS_NULL(v)) {
        printf("null");
    } else if (IS_BOOL(v)) {
        printf(AS_BOOL(v) ? "true" : "false");
    } else if (IS_NUMBER(v)) {
        printf("%g", AS_NUMBER(v));
    } else if (IS_OBJ(v)) {
        if (IS_STRING(v)) {
            printf("\"%s\"", AS_CSTRING(v));
        } else {
            printf("<obj>");
        }
    } else {
        printf("<unknown>");
    }
}

static size_t simple_instruction(const char* name, size_t offset) {
    printf("%s\n", name);
    return offset + 1;
}

static size_t byte_instruction(const char* name, const Chunk* chunk, size_t offset) {
    uint8_t slot = chunk->code[offset + 1];
    printf("%-16s %4d\n", name, slot);
    return offset + 2;
}

static size_t constant_instruction(const char* name, const Chunk* chunk, size_t offset) {
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    print_value(consttable_get(chunk, constant));
    printf("'\n");
    return offset + 2;
}

static size_t jump_instruction(const char* name, int sign, const Chunk* chunk, size_t offset) {
    uint16_t jump = (uint16_t)(chunk->code[offset + 1] << 8);
    jump |= chunk->code[offset + 2];
    printf("%-16s %4d -> %d\n", name, offset,
           offset + 3 + sign * jump);
    return offset + 3;
}

void disasm_chunk(const Chunk *chunk) {
    printf("== Disassembly ==\n");
    for (size_t offset = 0; offset < (size_t)chunk->count;) {
        printf("%04zu ", offset);
        if (offset > 0 && 
            (chunk->lines && chunk->lines[offset] == chunk->lines[offset - 1])) {
            printf("   | ");
        } else {
            if (chunk->lines) printf("%4d ", chunk->lines[offset]);
            else printf("   ? ");
        }

        uint8_t instruction = chunk->code[offset];
        switch (instruction) {
            case OP_CONSTANT:
                offset = constant_instruction("OP_CONSTANT", chunk, offset);
                break;
            case OP_NOP:
                offset = simple_instruction("OP_NOP", offset);
                break;
            case OP_NIL:
                offset = simple_instruction("OP_NIL", offset);
                break;
            case OP_TRUE:
                offset = simple_instruction("OP_TRUE", offset);
                break;
            case OP_FALSE:
                offset = simple_instruction("OP_FALSE", offset);
                break;
            case OP_POP:
                offset = simple_instruction("OP_POP", offset);
                break;
            case OP_DUP:
                offset = simple_instruction("OP_DUP", offset);
                break;
            case OP_BUILD_LIST:
                offset = byte_instruction("OP_BUILD_LIST", chunk, offset); 
                break;
            case OP_BUILD_MAP:
                offset = byte_instruction("OP_BUILD_MAP", chunk, offset);
                break;
            case OP_GET_INDEX:
                offset = simple_instruction("OP_GET_INDEX", offset);
                break;
            case OP_SET_INDEX:
                offset = simple_instruction("OP_SET_INDEX", offset);
                break;
            case OP_GET_LOCAL:
                offset = byte_instruction("OP_GET_LOCAL", chunk, offset);
                break;
            case OP_SET_LOCAL:
                offset = byte_instruction("OP_SET_LOCAL", chunk, offset);
                break;
            case OP_GET_GLOBAL:
                offset = constant_instruction("OP_GET_GLOBAL", chunk, offset);
                break;
            case OP_DEFINE_GLOBAL:
                offset = constant_instruction("OP_DEFINE_GLOBAL", chunk, offset);
                break;
            case OP_SET_GLOBAL:
                offset = constant_instruction("OP_SET_GLOBAL", chunk, offset);
                break;
            case OP_GET_UPVALUE:
                offset = byte_instruction("OP_GET_UPVALUE", chunk, offset);
                break;
            case OP_SET_UPVALUE:
                offset = byte_instruction("OP_SET_UPVALUE", chunk, offset);
                break;
            case OP_GET_PROPERTY:
                offset = constant_instruction("OP_GET_PROPERTY", chunk, offset);
                break;
            case OP_SET_PROPERTY:
                offset = constant_instruction("OP_SET_PROPERTY", chunk, offset);
                break;
            case OP_GET_SUPER:
                offset = constant_instruction("OP_GET_SUPER", chunk, offset);
                break;
            case OP_EQUAL:
                offset = simple_instruction("OP_EQUAL", offset);
                break;
            case OP_GREATER:
                offset = simple_instruction("OP_GREATER", offset);
                break;
            case OP_LESS:
                offset = simple_instruction("OP_LESS", offset);
                break;
            case OP_ADD:
                offset = simple_instruction("OP_ADD", offset);
                break;
            case OP_SUBTRACT:
                offset = simple_instruction("OP_SUBTRACT", offset);
                break;
            case OP_MULTIPLY:
                offset = simple_instruction("OP_MULTIPLY", offset);
                break;
            case OP_DIVIDE:
                offset = simple_instruction("OP_DIVIDE", offset);
                break;
            case OP_MODULO:
                offset = simple_instruction("OP_MODULO", offset);
                break;
            case OP_NOT:
                offset = simple_instruction("OP_NOT", offset);
                break;
            case OP_NEGATE:
                offset = simple_instruction("OP_NEGATE", offset);
                break;
            case OP_PRINT:
                offset = simple_instruction("OP_PRINT", offset);
                break;
            case OP_JUMP:
                offset = jump_instruction("OP_JUMP", 1, chunk, offset);
                break;
            case OP_JUMP_IF_FALSE:
                offset = jump_instruction("OP_JUMP_IF_FALSE", 1, chunk, offset);
                break;
            case OP_LOOP:
                offset = jump_instruction("OP_LOOP", -1, chunk, offset);
                break;
            case OP_CALL:
                offset = byte_instruction("OP_CALL", chunk, offset);
                break;
            case OP_INVOKE:
                offset = constant_instruction("OP_INVOKE", chunk, offset);
                // Note: Invoke also has an arg count byte
                printf("                %d args\n", chunk->code[offset]);
                offset++;
                break;
            case OP_CLOSURE: {
                offset++;
                uint8_t constant = chunk->code[offset++];
                printf("%-16s %4d ", "OP_CLOSURE", constant);
                print_value(consttable_get(chunk, constant));
                printf("\n");
                break;
            }
            case OP_CLOSE_UPVALUE:
                offset = simple_instruction("OP_CLOSE_UPVALUE", offset);
                break;
            case OP_RETURN:
                offset = simple_instruction("OP_RETURN", offset);
                break;
            case OP_CLASS:
                offset = constant_instruction("OP_CLASS", chunk, offset);
                break;
            case OP_INHERIT:
                offset = simple_instruction("OP_INHERIT", offset);
                break;
            case OP_METHOD:
                offset = constant_instruction("OP_METHOD", chunk, offset);
                break;
            case OP_USE:
                offset = constant_instruction("OP_USE", chunk, offset);
                break;
            case OP_INTERFACE:
                offset = constant_instruction("OP_INTERFACE", chunk, offset);
                break;
            case OP_MAKE_FOREIGN:
                offset = simple_instruction("OP_MAKE_FOREIGN", offset);
                break;
            case OP_MAT_MUL:
                offset = simple_instruction("OP_MAT_MUL", offset);
                break;
            case OP_MAKE_TENSOR:
                offset = simple_instruction("OP_MAKE_TENSOR", offset);
                break;
            case OP_HALT:
                offset = simple_instruction("OP_HALT", offset);
                break;
            default:
                printf("Unknown opcode %d\n", instruction);
                offset++;
                break;
        }
    }
}
