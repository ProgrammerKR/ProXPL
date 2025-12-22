// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-22
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#ifndef PROX_IR_H
#define PROX_IR_H

#include "common.h"
#include "value.h"

typedef enum {
    IR_OP_NOP,
    IR_OP_CONST,
    IR_OP_ADD,
    IR_OP_SUB,
    IR_OP_MUL,
    IR_OP_DIV,
    IR_OP_NEG,
    IR_OP_CMP_LT,
    IR_OP_CMP_GT,
    IR_OP_CMP_EQ,
    IR_OP_NOT,
    IR_OP_JUMP,
    IR_OP_JUMP_IF,
    IR_OP_PHI,
    IR_OP_CALL,
    IR_OP_RETURN,
    IR_OP_LOAD_VAR,
    IR_OP_STORE_VAR,
    IR_OP_GET_MEMBER,
    IR_OP_SET_MEMBER,
    IR_OP_ALLOCA,
} IROpcode;

typedef struct IRInstruction IRInstruction;
typedef struct IRBasicBlock IRBasicBlock;

typedef enum {
    OPERAND_CONST,
    OPERAND_VAL, // SSA Value (Register index)
    OPERAND_BLOCK // Target block for jumps
} OperandType;

typedef struct {
    OperandType type;
    union {
        Value constant;
        int ssaVal;
        IRBasicBlock* block;
    } as;
} IROperand;

struct IRInstruction {
    IROpcode opcode;
    int result; // Destination SSA register
    IROperand* operands;
    int operandCount;
    IRInstruction* next;
    IRInstruction* prev;
};

struct IRBasicBlock {
    int id;
    IRInstruction* first;
    IRInstruction* last;
    
    IRBasicBlock** predecessors;
    int predCount;
    int predCapacity;

    IRBasicBlock** successors;
    int succCount;
    int succCapacity;

    struct IRFunction* parent;
};

typedef struct IRFunction {
    char* name;
    IRBasicBlock* entry;
    IRBasicBlock** blocks;
    int blockCount;
    int blockCapacity;
    
    int nextSsaVal; // For unique register generation
} IRFunction;

typedef struct IRModule {
    IRFunction** functions;
    int funcCount;
    int funcCapacity;
} IRModule;

// Constructor functions
IRModule* createIRModule();
IRFunction* createIRFunction(const char* name);
IRBasicBlock* createIRBasicBlock(IRFunction* func);
IRInstruction* createIRInstruction(IROpcode opcode, int result);
void addOperand(IRInstruction* instr, IROperand op);

void computeCFGLinks(IRFunction* func);
void dumpIR(IRModule* module);
void freeIRModule(IRModule* module);

#endif // PROX_IR_H
