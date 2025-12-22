#include "../../include/backend_llvm.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <vector>
#include <map>
#include <iostream>

using namespace llvm;

class LLVMEmitter {
    std::unique_ptr<LLVMContext> Context;
    std::unique_ptr<Module> ModuleOb;
    std::unique_ptr<IRBuilder<>> Builder;
    std::map<IRBasicBlock*, BasicBlock*> blockMap;
    std::vector<Value*> ssaValues;

public:
    LLVMEmitter() {
        Context = std::make_unique<LLVMContext>();
        ModuleOb = std::make_unique<Module>("ProXPL Module", *Context);
        Builder = std::make_unique<IRBuilder<>>(*Context);
    }

    void emitModule(IRModule* module) {
        for (int i = 0; i < module->funcCount; i++) {
            emitFunction(module->functions[i]);
        }
        ModuleOb->print(outs(), nullptr);
    }

    void emitFunction(IRFunction* func) {
        // For now, all functions return int32
        FunctionType *FT = FunctionType::get(Builder->getInt32Ty(), false);
        Function *F = Function::Create(FT, Function::ExternalLinkage, func->name, ModuleOb.get());

        ssaValues.clear();
        ssaValues.resize(func->nextSsaVal + 2048, nullptr);
        blockMap.clear();

        // Pass 1: Create all blocks
        for (int i = 0; i < func->blockCount; i++) {
            char name[64];
            sprintf(name, "block%d", func->blocks[i]->id);
            blockMap[func->blocks[i]] = BasicBlock::Create(*Context, name, F);
        }

        // Pass 2: Emit instructions (except Phi operands)
        for (int i = 0; i < func->blockCount; i++) {
            IRBasicBlock* irBlock = func->blocks[i];
            BasicBlock* llvmBlock = blockMap[irBlock];
            Builder->SetInsertPoint(llvmBlock);

            IRInstruction* instr = irBlock->first;
            while (instr) {
                emitInstruction(instr);
                instr = instr->next;
            }
        }

        // Pass 3: Fill in Phi operands
        for (int i = 0; i < func->blockCount; i++) {
            IRInstruction* instr = func->blocks[i]->first;
            while (instr) {
                if (instr->opcode == IR_OP_PHI) {
                    PHINode* phi = cast<PHINode>(ssaValues[instr->result]);
                    for (int k = 0; k < instr->operandCount; k += 2) {
                        Value* val = getOperand(instr->operands[k]);
                        BasicBlock* incomingBB = blockMap[instr->operands[k+1].as.block];
                        if (val && incomingBB) {
                            phi->addIncoming(val, incomingBB);
                        }
                    }
                }
                instr = instr->next;
            }
        }

        // Verify function
        std::string err;
        raw_string_ostream os(err);
        if (verifyFunction(*F, &os)) {
            std::cerr << "LLVM Verification Error: " << os.str() << "\n";
        }
    }

    void emitInstruction(IRInstruction* instr) {
        switch (instr->opcode) {
            case IR_OP_CONST: {
                Value* v = nullptr;
                if (IS_NUMBER(instr->operands[0].as.constant)) {
                    v = ConstantInt::get(*Context, APInt(32, (uint64_t)AS_NUMBER(instr->operands[0].as.constant), true));
                }
                ssaValues[instr->result] = v;
                break;
            }
            case IR_OP_ADD: {
                Value* L = getOperand(instr->operands[0]);
                Value* R = getOperand(instr->operands[1]);
                if (L && R) ssaValues[instr->result] = Builder->CreateAdd(L, R, "addtmp");
                break;
            }
            case IR_OP_SUB: {
                Value* L = getOperand(instr->operands[0]);
                Value* R = getOperand(instr->operands[1]);
                if (L && R) ssaValues[instr->result] = Builder->CreateSub(L, R, "subtmp");
                break;
            }
            case IR_OP_MUL: {
                Value* L = getOperand(instr->operands[0]);
                Value* R = getOperand(instr->operands[1]);
                if (L && R) ssaValues[instr->result] = Builder->CreateMul(L, R, "multmp");
                break;
            }
            case IR_OP_DIV: {
                Value* L = getOperand(instr->operands[0]);
                Value* R = getOperand(instr->operands[1]);
                if (L && R) ssaValues[instr->result] = Builder->CreateSDiv(L, R, "divtmp");
                break;
            }
            case IR_OP_CMP_GT: {
                Value* L = getOperand(instr->operands[0]);
                Value* R = getOperand(instr->operands[1]);
                if (L && R) {
                    Value* cmp = Builder->CreateICmpSGT(L, R, "cmptmp");
                    ssaValues[instr->result] = Builder->CreateZExt(cmp, Builder->getInt32Ty(), "booltmp");
                }
                break;
            }
            case IR_OP_CMP_LT: {
                Value* L = getOperand(instr->operands[0]);
                Value* R = getOperand(instr->operands[1]);
                if (L && R) {
                    Value* cmp = Builder->CreateICmpSLT(L, R, "cmptmp");
                    ssaValues[instr->result] = Builder->CreateZExt(cmp, Builder->getInt32Ty(), "booltmp");
                }
                break;
            }
            case IR_OP_CMP_EQ: {
                Value* L = getOperand(instr->operands[0]);
                Value* R = getOperand(instr->operands[1]);
                if (L && R) {
                    Value* cmp = Builder->CreateICmpEQ(L, R, "cmptmp");
                    ssaValues[instr->result] = Builder->CreateZExt(cmp, Builder->getInt32Ty(), "booltmp");
                }
                break;
            }
            case IR_OP_JUMP: {
                Builder->CreateBr(blockMap[instr->operands[0].as.block]);
                break;
            }
            case IR_OP_JUMP_IF: {
                Value* Cond = getOperand(instr->operands[0]);
                BasicBlock* Then = blockMap[instr->operands[1].as.block];
                BasicBlock* Else = blockMap[instr->operands[2].as.block];
                if (Cond) {
                    Value* boolCond = Builder->CreateICmpNE(Cond, ConstantInt::get(*Context, APInt(32, 0)), "ifcond");
                    Builder->CreateCondBr(boolCond, Then, Else);
                }
                break;
            }
            case IR_OP_PHI: {
                // Initialize Phi with no operands
                ssaValues[instr->result] = Builder->CreatePHI(Builder->getInt32Ty(), instr->operandCount / 2, "phitmp");
                break;
            }
            case IR_OP_RETURN: {
                Value* V = getOperand(instr->operands[0]);
                if (V) Builder->CreateRet(V);
                else Builder->CreateRet(Builder->getInt32(0));
                break;
            }
            default:
                break;
        }
    }

    Value* getOperand(IROperand& op) {
        if (op.type == OPERAND_CONST) {
            if (IS_NUMBER(op.as.constant)) {
                return ConstantInt::get(*Context, APInt(32, (uint64_t)AS_NUMBER(op.as.constant), true));
            }
            return nullptr;
        } else if (op.type == OPERAND_VAL) {
            if (op.as.ssaVal >= 0 && (size_t)op.as.ssaVal < ssaValues.size()) {
                return ssaValues[op.as.ssaVal];
            }
        }
        return nullptr;
    }
};

extern "C" void emitLLVM(IRModule* module) {
    LLVMEmitter emitter;
    emitter.emitModule(module);
}
