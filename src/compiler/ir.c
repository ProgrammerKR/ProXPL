// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-22
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include "../../include/ir.h"
#include <stdlib.h>
#include <string.h>

IRModule* createIRModule() {
    IRModule* module = (IRModule*)malloc(sizeof(IRModule));
    module->functions = NULL;
    module->funcCount = 0;
    module->funcCapacity = 0;
    return module;
}

IRFunction* createIRFunction(const char* name, bool isAsync) {
    IRFunction* func = (IRFunction*)malloc(sizeof(IRFunction));
    func->name = strdup(name);
    func->entry = NULL;
    func->blocks = NULL;
    func->blockCount = 0;
    func->blockCapacity = 0;
    func->nextSsaVal = 0;
    func->isAsync = isAsync;
    return func;
}

IRBasicBlock* createIRBasicBlock(IRFunction* func) {
    IRBasicBlock* block = (IRBasicBlock*)malloc(sizeof(IRBasicBlock));
    block->id = func->blockCount;
    block->first = NULL;
    block->last = NULL;
    block->predecessors = NULL;
    block->predCount = 0;
    block->predCapacity = 0;
    block->successors = NULL;
    block->succCount = 0;
    block->succCapacity = 0;
    block->parent = func;

    // Add to function
    if (func->blockCount >= func->blockCapacity) {
        func->blockCapacity = func->blockCapacity == 0 ? 8 : func->blockCapacity * 2;
        func->blocks = (IRBasicBlock**)realloc(func->blocks, sizeof(IRBasicBlock*) * func->blockCapacity);
    }
    func->blocks[func->blockCount++] = block;

    if (func->entry == NULL) func->entry = block;

    return block;
}

IRInstruction* createIRInstruction(IROpcode opcode, int result) {
    IRInstruction* instr = (IRInstruction*)malloc(sizeof(IRInstruction));
    instr->opcode = opcode;
    instr->result = result;
    instr->operands = NULL;
    instr->operandCount = 0;
    instr->next = NULL;
    instr->prev = NULL;
    return instr;
}

void addOperand(IRInstruction* instr, IROperand op) {
    instr->operands = (IROperand*)realloc(instr->operands, sizeof(IROperand) * (instr->operandCount + 1));
    instr->operands[instr->operandCount++] = op;
}

static const char* irOpName(IROpcode op) {
    switch (op) {
        case IR_OP_NOP: return "nop";
        case IR_OP_CONST: return "const";
        case IR_OP_ADD: return "add";
        case IR_OP_SUB: return "sub";
        case IR_OP_MUL: return "mul";
        case IR_OP_DIV: return "div";
        case IR_OP_NEG: return "neg";
        case IR_OP_CMP_LT: return "cmp_lt";
        case IR_OP_CMP_GT: return "cmp_gt";
        case IR_OP_CMP_EQ: return "cmp_eq";
        case IR_OP_NOT: return "not";
        case IR_OP_JUMP: return "jump";
        case IR_OP_JUMP_IF: return "jump_if";
        case IR_OP_PHI: return "phi";
        case IR_OP_CALL: return "call";
        case IR_OP_RETURN: return "ret";
        case IR_OP_LOAD_VAR: return "load";
        case IR_OP_STORE_VAR: return "store";
        case IR_OP_GET_MEMBER: return "get_member";
        case IR_OP_SET_MEMBER: return "set_member";
        case IR_OP_ALLOCA: return "alloca";
        case IR_OP_AWAIT: return "await";
        default: return "unknown";
    }
}

static void addEdge(IRBasicBlock* from, IRBasicBlock* to) {
    // Add to successors
    for (int i = 0; i < from->succCount; i++) if (from->successors[i] == to) return; // Already linked
    if (from->succCount >= from->succCapacity) {
        from->succCapacity = from->succCapacity == 0 ? 4 : from->succCapacity * 2;
        from->successors = (IRBasicBlock**)realloc(from->successors, sizeof(IRBasicBlock*) * from->succCapacity);
    }
    from->successors[from->succCount++] = to;

    // Add to predecessors
    for (int i = 0; i < to->predCount; i++) if (to->predecessors[i] == from) return;
    if (to->predCount >= to->predCapacity) {
        to->predCapacity = to->predCapacity == 0 ? 4 : to->predCapacity * 2;
        to->predecessors = (IRBasicBlock**)realloc(to->predecessors, sizeof(IRBasicBlock*) * to->predCapacity);
    }
    to->predecessors[to->predCount++] = from;
}

void computeCFGLinks(IRFunction* func) {
    for (int i = 0; i < func->blockCount; i++) {
        IRBasicBlock* block = func->blocks[i];
        block->predCount = 0;
        block->succCount = 0;
    }

    for (int i = 0; i < func->blockCount; i++) {
        IRBasicBlock* block = func->blocks[i];
        IRInstruction* instr = block->first;
        while (instr) {
            if (instr->opcode == IR_OP_JUMP) {
                addEdge(block, instr->operands[0].as.block);
            } else if (instr->opcode == IR_OP_JUMP_IF) {
                addEdge(block, instr->operands[1].as.block);
            }
            instr = instr->next;
        }
    }
}

void dumpIR(IRModule* module) {
    for (int i = 0; i < module->funcCount; i++) {
        IRFunction* func = module->functions[i];
        printf("function %s {\n", func->name);
        for (int j = 0; j < func->blockCount; j++) {
            IRBasicBlock* block = func->blocks[j];
            printf("  block%d:\n", block->id);
            IRInstruction* instr = block->first;
            while (instr) {
                printf("    ");
                if (instr->result != -1) printf("%%v%d = ", instr->result);
                printf("%s", irOpName(instr->opcode));
                for (int k = 0; k < instr->operandCount; k++) {
                    if (instr->opcode == IR_OP_PHI && k % 2 == 0) printf( (k == 0) ? " " : " | ");
                    else printf(", ");

                    IROperand* op = &instr->operands[k];
                    if (op->type == OPERAND_VAL) printf("%%v%d", op->as.ssaVal);
                    else if (op->type == OPERAND_CONST) {
                        printValue(op->as.constant);
                    }
                    else if (op->type == OPERAND_BLOCK) printf("block%d", op->as.block->id);
                }
                printf("\n");
                instr = instr->next;
            }
        }
        printf("}\n");
    }
}

void freeIRModule(IRModule* module) {
    for (int i = 0; i < module->funcCount; i++) {
        IRFunction* func = module->functions[i];
        for (int j = 0; j < func->blockCount; j++) {
            IRBasicBlock* block = func->blocks[j];
            IRInstruction* curr = block->first;
            while (curr) {
                IRInstruction* next = curr->next;
                free(curr->operands);
                free(curr);
                curr = next;
            }
            free(block->predecessors);
            free(block->successors);
            free(block);
        }
        free(func->blocks);
        free(func->name);
        free(func);
    }
    free(module->functions);
    free(module);
}
