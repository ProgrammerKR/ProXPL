// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

/*
 * ProXPL Bytecode Verifier
 * Validates bytecode before execution to prevent crashes and security issues.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/bytecode.h"
#include "../../include/vm.h"
#include "../../include/common.h"

typedef struct {
    uint8_t* code;
    int count;
    ValueArray* constants;
    ExceptionHandlerTable* handlers;
    int stackDepth;
    int maxStackDepth;
    bool hadError;
    const char* errorMessage;
} Verifier;

static void verifierError(Verifier* v, const char* message) {
    if (!v->hadError) {
        v->hadError = true;
        v->errorMessage = message;
    }
}

static bool verifyInstruction(Verifier* v, int ip) {
    if (ip < 0 || ip >= v->count) {
        verifierError(v, "Instruction pointer out of bounds.");
        return false;
    }

    uint8_t instruction = v->code[ip];
    int nextIp = ip + 1;

    switch (instruction) {
        case OP_CONSTANT: {
            if (nextIp >= v->count) {
                verifierError(v, "OP_CONSTANT missing constant index.");
                return false;
            }
            uint8_t constIdx = v->code[nextIp];
            if (constIdx >= (uint8_t)v->constants->count) {
                verifierError(v, "OP_CONSTANT references invalid constant.");
                return false;
            }
            v->stackDepth++;
            if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            return true;
        }
        case OP_CONSTANT_LONG: {
            if (nextIp + 3 >= v->count) {
                verifierError(v, "OP_CONSTANT_LONG missing constant index bytes.");
                return false;
            }
            uint32_t constIdx = (uint32_t)v->code[nextIp] |
                                ((uint32_t)v->code[nextIp + 1] << 8) |
                                ((uint32_t)v->code[nextIp + 2] << 16);
            if (constIdx >= (uint32_t)v->constants->count) {
                verifierError(v, "OP_CONSTANT_LONG references invalid constant.");
                return false;
            }
            v->stackDepth++;
            if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            return true;
        }
        case OP_NIL:
        case OP_TRUE:
        case OP_FALSE: {
            v->stackDepth++;
            if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            return true;
        }
        case OP_POP: {
            if (v->stackDepth <= 0) {
                verifierError(v, "Stack underflow on OP_POP.");
                return false;
            }
            v->stackDepth--;
            return true;
        }
        case OP_DUP: {
            if (v->stackDepth <= 0) {
                verifierError(v, "Stack underflow on OP_DUP.");
                return false;
            }
            v->stackDepth++;
            if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            return true;
        }
        case OP_BUILD_LIST: {
            if (nextIp >= v->count) {
                verifierError(v, "OP_BUILD_LIST missing count.");
                return false;
            }
            uint8_t count = v->code[nextIp];
            if (v->stackDepth < count) {
                verifierError(v, "Stack underflow on OP_BUILD_LIST.");
                return false;
            }
            v->stackDepth -= count;
            v->stackDepth++;
            if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            return true;
        }
        case OP_BUILD_MAP: {
            if (nextIp >= v->count) {
                verifierError(v, "OP_BUILD_MAP missing count.");
                return false;
            }
            uint8_t count = v->code[nextIp];
            if (v->stackDepth < count * 2) {
                verifierError(v, "Stack underflow on OP_BUILD_MAP.");
                return false;
            }
            v->stackDepth -= count * 2;
            v->stackDepth++;
            if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            return true;
        }
        case OP_GET_INDEX:
        case OP_SET_INDEX: {
            if (v->stackDepth < 2) {
                verifierError(v, "Stack underflow on index operation.");
                return false;
            }
            if (instruction == OP_SET_INDEX) {
                if (v->stackDepth < 3) {
                    verifierError(v, "Stack underflow on OP_SET_INDEX.");
                    return false;
                }
                v->stackDepth -= 2;
            } else {
                v->stackDepth--;
            }
            return true;
        }
        case OP_GET_LOCAL: {
            if (nextIp >= v->count) {
                verifierError(v, "OP_GET_LOCAL missing slot.");
                return false;
            }
            v->stackDepth++;
            if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            return true;
        }
        case OP_GET_LOCAL_0:
        case OP_GET_LOCAL_1:
        case OP_GET_LOCAL_2:
        case OP_GET_LOCAL_3: {
            v->stackDepth++;
            if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            return true;
        }
        case OP_SET_LOCAL: {
            if (nextIp >= v->count) {
                verifierError(v, "OP_SET_LOCAL missing slot.");
                return false;
            }
            if (v->stackDepth <= 0) {
                verifierError(v, "Stack underflow on OP_SET_LOCAL.");
                return false;
            }
            return true;
        }
        case OP_SET_LOCAL_0:
        case OP_SET_LOCAL_1:
        case OP_SET_LOCAL_2:
        case OP_SET_LOCAL_3: {
            if (v->stackDepth <= 0) {
                verifierError(v, "Stack underflow on OP_SET_LOCAL.");
                return false;
            }
            return true;
        }
        case OP_GET_GLOBAL:
        case OP_DEFINE_GLOBAL:
        case OP_SET_GLOBAL: {
            if (nextIp >= v->count) {
                verifierError(v, "Global operation missing name index.");
                return false;
            }
            uint8_t nameIdx = v->code[nextIp];
            if (instruction != OP_SET_GLOBAL) {
                v->stackDepth++;
                if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            } else {
                if (v->stackDepth <= 0) {
                    verifierError(v, "Stack underflow on OP_SET_GLOBAL.");
                    return false;
                }
            }
            return true;
        }
        case OP_GET_UPVALUE:
        case OP_SET_UPVALUE: {
            if (nextIp >= v->count) {
                verifierError(v, "Upvalue operation missing index.");
                return false;
            }
            if (instruction == OP_GET_UPVALUE) {
                v->stackDepth++;
                if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            } else {
                if (v->stackDepth <= 0) {
                    verifierError(v, "Stack underflow on OP_SET_UPVALUE.");
                    return false;
                }
            }
            return true;
        }
        case OP_GET_PROPERTY:
        case OP_SET_PROPERTY: {
            if (v->stackDepth < 1) {
                verifierError(v, "Stack underflow on property access.");
                return false;
            }
            if (instruction == OP_SET_PROPERTY && v->stackDepth < 2) {
                verifierError(v, "Stack underflow on OP_SET_PROPERTY.");
                return false;
            }
            if (instruction == OP_SET_PROPERTY) {
                v->stackDepth -= 2;
                v->stackDepth++;
            } else {
                v->stackDepth--;
            }
            return true;
        }
        case OP_GET_SUPER: {
            if (v->stackDepth < 1) {
                verifierError(v, "Stack underflow on OP_GET_SUPER.");
                return false;
            }
            v->stackDepth--;
            v->stackDepth++;
            return true;
        }
        case OP_EQUAL:
        case OP_GREATER:
        case OP_LESS: {
            if (v->stackDepth < 2) {
                verifierError(v, "Stack underflow on comparison.");
                return false;
            }
            v->stackDepth -= 2;
            v->stackDepth++;
            if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            return true;
        }
        case OP_ADD:
        case OP_SUBTRACT:
        case OP_MULTIPLY:
        case OP_DIVIDE:
        case OP_MODULO:
        case OP_BIT_AND:
        case OP_BIT_OR:
        case OP_BIT_XOR:
        case OP_LEFT_SHIFT:
        case OP_RIGHT_SHIFT:
        case OP_MAT_MUL: {
            if (v->stackDepth < 2) {
                verifierError(v, "Stack underflow on binary operation.");
                return false;
            }
            v->stackDepth -= 2;
            v->stackDepth++;
            if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            return true;
        }
        case OP_NOT:
        case OP_NEGATE: {
            if (v->stackDepth < 1) {
                verifierError(v, "Stack underflow on unary operation.");
                return false;
            }
            return true;
        }
        case OP_PRINT: {
            if (v->stackDepth < 1) {
                verifierError(v, "Stack underflow on OP_PRINT.");
                return false;
            }
            v->stackDepth--;
            return true;
        }
        case OP_JUMP: {
            if (nextIp + 1 >= v->count) {
                verifierError(v, "OP_JUMP missing offset.");
                return false;
            }
            uint16_t offset = (uint16_t)v->code[nextIp] | ((uint16_t)v->code[nextIp + 1] << 8);
            int target = ip + 2 + offset;
            if (target < 0 || target >= v->count) {
                verifierError(v, "OP_JUMP target out of bounds.");
                return false;
            }
            return true;
        }
        case OP_JUMP_IF_FALSE: {
            if (nextIp + 1 >= v->count) {
                verifierError(v, "OP_JUMP_IF_FALSE missing offset.");
                return false;
            }
            uint16_t offset = (uint16_t)v->code[nextIp] | ((uint16_t)v->code[nextIp + 1] << 8);
            int target = ip + 2 + offset;
            if (target < 0 || target >= v->count) {
                verifierError(v, "OP_JUMP_IF_FALSE target out of bounds.");
                return false;
            }
            if (v->stackDepth < 1) {
                verifierError(v, "Stack underflow on OP_JUMP_IF_FALSE.");
                return false;
            }
            return true;
        }
        case OP_LOOP: {
            if (nextIp + 1 >= v->count) {
                verifierError(v, "OP_LOOP missing offset.");
                return false;
            }
            uint16_t offset = (uint16_t)v->code[nextIp] | ((uint16_t)v->code[nextIp + 1] << 8);
            int target = ip + 2 - offset;
            if (target < 0 || target >= v->count) {
                verifierError(v, "OP_LOOP target out of bounds.");
                return false;
            }
            return true;
        }
        case OP_CALL: {
            if (nextIp >= v->count) {
                verifierError(v, "OP_CALL missing arg count.");
                return false;
            }
            uint8_t argCount = v->code[nextIp];
            if (v->stackDepth < argCount + 1) {
                verifierError(v, "Stack underflow on OP_CALL.");
                return false;
            }
            v->stackDepth -= argCount;
            return true;
        }
        case OP_INVOKE:
        case OP_SUPER_INVOKE: {
            if (nextIp >= v->count) {
                verifierError(v, "Invoke missing name index.");
                return false;
            }
            uint8_t nameIdx = v->code[nextIp];
            if (nameIdx >= (uint8_t)v->constants->count) {
                verifierError(v, "Invoke references invalid constant.");
                return false;
            }
            if (nextIp + 1 >= v->count) {
                verifierError(v, "Invoke missing arg count.");
                return false;
            }
            uint8_t argCount = v->code[nextIp + 1];
            if (v->stackDepth < argCount + 1) {
                verifierError(v, "Stack underflow on invoke.");
                return false;
            }
            v->stackDepth -= argCount;
            return true;
        }
        case OP_CLOSURE: {
            if (nextIp >= v->count) {
                verifierError(v, "OP_CLOSURE missing function index.");
                return false;
            }
            uint8_t funcIdx = v->code[nextIp];
            if (funcIdx >= (uint8_t)v->constants->count) {
                verifierError(v, "OP_CLOSURE references invalid constant.");
                return false;
            }
            v->stackDepth++;
            if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            return true;
        }
        case OP_CLOSE_UPVALUE: {
            if (v->stackDepth < 1) {
                verifierError(v, "Stack underflow on OP_CLOSE_UPVALUE.");
                return false;
            }
            v->stackDepth--;
            return true;
        }
        case OP_RETURN: {
            return true;
        }
        case OP_CLASS: {
            if (nextIp >= v->count) {
                verifierError(v, "OP_CLASS missing name index.");
                return false;
            }
            v->stackDepth++;
            if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            return true;
        }
        case OP_INHERIT: {
            if (v->stackDepth < 2) {
                verifierError(v, "Stack underflow on OP_INHERIT.");
                return false;
            }
            v->stackDepth--;
            return true;
        }
        case OP_METHOD: {
            if (nextIp >= v->count) {
                verifierError(v, "OP_METHOD missing name index.");
                return false;
            }
            if (v->stackDepth < 1) {
                verifierError(v, "Stack underflow on OP_METHOD.");
                return false;
            }
            v->stackDepth--;
            return true;
        }
        case OP_USE: {
            if (nextIp >= v->count) {
                verifierError(v, "OP_USE missing module index.");
                return false;
            }
            uint8_t modIdx = v->code[nextIp];
            if (modIdx >= (uint8_t)v->constants->count) {
                verifierError(v, "OP_USE references invalid constant.");
                return false;
            }
            return true;
        }
        case OP_TRY:
        case OP_CATCH:
        case OP_END_TRY: {
            return true;
        }
        case OP_CONTEXT:
        case OP_LAYER:
        case OP_ACTIVATE:
        case OP_END_ACTIVATE: {
            return true;
        }
        case OP_INTERFACE:
        case OP_TRAIT:
        case OP_IMPLEMENT: {
            if (nextIp >= v->count) {
                verifierError(v, "Trait/interface operation missing name index.");
                return false;
            }
            return true;
        }
        case OP_MAKE_FOREIGN: {
            if (nextIp >= v->count) {
                verifierError(v, "OP_MAKE_FOREIGN missing data.");
                return false;
            }
            v->stackDepth++;
            if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            return true;
        }
        case OP_UNWRAP: {
            if (v->stackDepth < 1) {
                verifierError(v, "Stack underflow on OP_UNWRAP.");
                return false;
            }
            return true;
        }
        case OP_MAKE_TENSOR: {
            if (nextIp >= v->count) {
                verifierError(v, "OP_MAKE_TENSOR missing dim count.");
                return false;
            }
            uint8_t dimCount = v->code[nextIp];
            int needed = 1 + 4 + dimCount * 4;
            if (nextIp + needed >= v->count) {
                verifierError(v, "OP_MAKE_TENSOR data truncated.");
                return false;
            }
            if (dimCount > 16) {
                verifierError(v, "OP_MAKE_TENSOR dim count exceeds limit.");
                return false;
            }
            v->stackDepth++;
            if (v->stackDepth > v->maxStackDepth) v->maxStackDepth = v->stackDepth;
            return true;
        }
        case OP_NOP:
            return true;
        case OP_HALT:
            return true;
        default:
            verifierError(v, "Unknown opcode.");
            return false;
    }

    return true;
}

bool verifyChunk(Chunk* chunk, const char* sourceName) {
    Verifier v;
    memset(&v, 0, sizeof(v));
    v.code = chunk->code;
    v.count = chunk->count;
    v.constants = &chunk->constants;
    v.handlers = &chunk->exceptionHandlers;

    if (chunk->count == 0) return true;

    for (int ip = 0; ip < chunk->count; ) {
        if (!verifyInstruction(&v, ip)) {
            fprintf(stderr, "Bytecode verification failed in %s at ip=%d: %s\n",
                    sourceName ? sourceName : "<unknown>", ip, v.errorMessage);
            return false;
        }

        uint8_t instruction = chunk->code[ip];
        int instructionSize = 1;

        switch (instruction) {
            case OP_CONSTANT:
                instructionSize = 2;
                break;
            case OP_CONSTANT_LONG:
                instructionSize = 4;
                break;
            case OP_BUILD_LIST:
            case OP_BUILD_MAP:
            case OP_GET_LOCAL:
            case OP_SET_LOCAL:
            case OP_GET_GLOBAL:
            case OP_DEFINE_GLOBAL:
            case OP_SET_GLOBAL:
            case OP_GET_UPVALUE:
            case OP_SET_UPVALUE:
                instructionSize = 2;
                break;
            case OP_JUMP:
            case OP_JUMP_IF_FALSE:
            case OP_LOOP:
                instructionSize = 3;
                break;
            case OP_CALL:
            case OP_INVOKE:
            case OP_SUPER_INVOKE:
            case OP_CLOSURE:
            case OP_USE:
            case OP_METHOD:
            case OP_CLASS:
            case OP_INTERFACE:
            case OP_TRAIT:
            case OP_IMPLEMENT:
            case OP_MAKE_FOREIGN:
                instructionSize = 2;
                break;
            case OP_MAKE_TENSOR: {
                if (ip + 1 < chunk->count) {
                    uint8_t dimCount = chunk->code[ip + 1];
                    instructionSize = 1 + 1 + 4 + dimCount * 4;
                } else {
                    instructionSize = 2;
                }
                break;
            }
            default:
                instructionSize = 1;
                break;
        }

        ip += instructionSize;
    }

    if (v.maxStackDepth > STACK_MAX) {
        fprintf(stderr, "Bytecode verification failed in %s: max stack depth %d exceeds limit %d\n",
                sourceName ? sourceName : "<unknown>", v.maxStackDepth, STACK_MAX);
        return false;
    }

    return true;
}
