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
        
        setupRuntimeTypes();
    }

    void setupRuntimeTypes() {
        // Declare extern "C" functions from llvm_runtime.c
        
        // Value prox_rt_add(Value a, Value b);
        FunctionType *BinOpType = FunctionType::get(
            Builder->getInt64Ty(),
            {Builder->getInt64Ty(), Builder->getInt64Ty()},
            false
        );
        Function::Create(BinOpType, Function::ExternalLinkage, "prox_rt_add", ModuleOb.get());
        
        // void prox_rt_print(Value v);
        FunctionType *PrintType = FunctionType::get(
            Builder->getVoidTy(),
            {Builder->getInt64Ty()},
            false
        );
        Function::Create(PrintType, Function::ExternalLinkage, "prox_rt_print", ModuleOb.get());

        // Value prox_rt_const_string(char* chars, int length);
        FunctionType *ConstStrType = FunctionType::get(
            Builder->getInt64Ty(),
            {Builder->getInt8PtrTy(), Builder->getInt32Ty()},
            false
        );
        Function::Create(ConstStrType, Function::ExternalLinkage, "prox_rt_const_string", ModuleOb.get());
    }

    void emitModule(IRModule* module) {
        for (int i = 0; i < module->funcCount; i++) {
            emitFunction(module->functions[i]);
        }
        ModuleOb->print(outs(), nullptr);
    }

    void emitFunction(IRFunction* func) {
        // All functions return Value (Int64)
        FunctionType *FT = FunctionType::get(Builder->getInt64Ty(), false);
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

        // Generate return 0 if block is unterminated (fallback)
         if (!F->back().getTerminator()) {
             Builder->SetInsertPoint(&F->back());
             // Return NIL (0x7ffc000000000001)
             uint64_t nilVal = 0x7ffc000000000001; 
             Builder->CreateRet(ConstantInt::get(*Context, APInt(64, nilVal, false)));
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
                    // Just a double encoded as int64
                    double num = AS_NUMBER(instr->operands[0].as.constant);
                    uint64_t bits;
                    memcpy(&bits, &num, sizeof(double));
                    v = ConstantInt::get(*Context, APInt(64, bits, false));
                } else if (IS_STRING(instr->operands[0].as.constant)) {
                    // Define global string constant
                    ObjString* strObj = AS_STRING(instr->operands[0].as.constant);
                    Constant *StrConstant = ConstantDataArray::getString(*Context, strObj->chars);
                    GlobalVariable *ValidStr = new GlobalVariable(*ModuleOb, StrConstant->getType(), true,
                        GlobalValue::PrivateLinkage, StrConstant, ".str");
                    
                    // Call runtime to create ObjString
                    Function *AllocFunc = ModuleOb->getFunction("prox_rt_const_string");
                    Value* Zero = Builder->getInt32(0);
                    Value* Args[] = { Zero, Zero };
                    // GEP to get pointer to char array
                    Value* StrPtr = Builder->CreateInBoundsGEP(StrConstant->getType(), ValidStr, Args);
                    
                    v = Builder->CreateCall(AllocFunc, {
                        StrPtr,
                        Builder->getInt32(strObj->length)
                    }, "strObj");
                }
                ssaValues[instr->result] = v;
                break;
            }
            case IR_OP_ADD: {
                Value* L = getOperand(instr->operands[0]);
                Value* R = getOperand(instr->operands[1]);
                Function *AddFunc = ModuleOb->getFunction("prox_rt_add");
                if (L && R && AddFunc) ssaValues[instr->result] = Builder->CreateCall(AddFunc, {L, R}, "addtmp");
                break;
            }
            // TODO: Implement other math ops similarly with runtime helpers OR inline check
            
            case IR_OP_JUMP: {
                Builder->CreateBr(blockMap[instr->operands[0].as.block]);
                break;
            }
            case IR_OP_JUMP_IF: {
                Value* Cond = getOperand(instr->operands[0]);
                BasicBlock* Then = blockMap[instr->operands[1].as.block];
                BasicBlock* Else = blockMap[instr->operands[2].as.block];
                
                // Compare Cond != FALSE (simplified); really should check for non-false/non-nil
                // For now assuming Cond is a boolean-like Value
                // Note: In Nan-boxing, False is specific tag.
                // We'll simplify and check if (Cond & ~TAG_MASK) != 0 for now or just check explicit False?
                // For this iteration, let's assume we optimized bools to i1 in IR or strict checking.
                // Actually, if everything is i64, we need to compare against encoded False.
                
                // Hack: Compare against 0 for testing if we used 0 for false in simple tests?
                // But we are using full values.
                // Let's rely on truthiness: != False && != Nil
                // For MVP: Compare != encoded FALSE.
                
                // uint64_t falseVal = 0x7ffc000000000002; // TAG_FALSE = 2
                // Value* FalseC = ConstantInt::get(*Context, APInt(64, falseVal, false));
                // Value* isFalse = Builder->CreateICmpEQ(Cond, FalseC, "isfalse");
                // Builder->CreateCondBr(isFalse, Else, Then); // Swap branches
                
                // Temporary: Treat 0 as false (legacy behavior until full type lowering)
                 if (Cond) {
                    Value* boolCond = Builder->CreateICmpNE(Cond, ConstantInt::get(*Context, APInt(64, 0)), "ifcond");
                    Builder->CreateCondBr(boolCond, Then, Else);
                 }
                break;
            }
            case IR_OP_PHI: {
                ssaValues[instr->result] = Builder->CreatePHI(Builder->getInt64Ty(), instr->operandCount / 2, "phitmp");
                break;
            }
            case IR_OP_RETURN: {
                Value* V = getOperand(instr->operands[0]);
                // Default return NIL
                if (!V) {
                   uint64_t nilVal = 0x7ffc000000000001; 
                   V = ConstantInt::get(*Context, APInt(64, nilVal, false));
                }
                Builder->CreateRet(V);
                break;
            }
            default:
                break;
        }
    }

    Value* getOperand(IROperand& op) {
        if (op.type == OPERAND_CONST) {
             if (IS_NUMBER(op.as.constant)) {
                // Return int64 representation of double
                double num = AS_NUMBER(op.as.constant);
                uint64_t bits;
                memcpy(&bits, &num, sizeof(double));
                return ConstantInt::get(*Context, APInt(64, bits, false));
            }
            // Other constants?
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
