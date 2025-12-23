#include "../../include/backend_llvm.h"
#include "../../include/object.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <vector>
#include <map>
#include <iostream>
#include <memory>

// Do NOT use 'using namespace llvm;' due to clash with our 'Value' type.

class LLVMEmitter {
    std::unique_ptr<llvm::LLVMContext> Context;
    std::unique_ptr<llvm::Module> ModuleOb;
    std::unique_ptr<llvm::IRBuilder<>> Builder;
    std::map<IRBasicBlock*, llvm::BasicBlock*> blockMap;
    std::vector<llvm::Value*> ssaValues;

public:
    LLVMEmitter() {
        Context = std::make_unique<llvm::LLVMContext>();
        ModuleOb = std::make_unique<llvm::Module>("ProXPL Module", *Context);
        Builder = std::make_unique<llvm::IRBuilder<>>(*Context);
        
        setupRuntimeTypes();
    }

    void setupRuntimeTypes() {
        // Value prox_rt_add(Value a, Value b);
        llvm::FunctionType *BinOpType = llvm::FunctionType::get(
            Builder->getInt64Ty(),
            {Builder->getInt64Ty(), Builder->getInt64Ty()},
            false
        );
        llvm::Function::Create(BinOpType, llvm::Function::ExternalLinkage, "prox_rt_add", ModuleOb.get());
        
        // void prox_rt_print(Value v);
        llvm::FunctionType *PrintType = llvm::FunctionType::get(
            Builder->getVoidTy(),
            {Builder->getInt64Ty()},
            false
        );
        llvm::Function::Create(PrintType, llvm::Function::ExternalLinkage, "prox_rt_print", ModuleOb.get());

        // Value prox_rt_const_string(char* chars, int length);
        llvm::FunctionType *ConstStrType = llvm::FunctionType::get(
            Builder->getInt64Ty(),
            {Builder->getPtrTy(), Builder->getInt32Ty()},
            false
        );
        llvm::Function::Create(ConstStrType, llvm::Function::ExternalLinkage, "prox_rt_const_string", ModuleOb.get());
    }

    void emitModule(IRModule* module) {
        for (int i = 0; i < module->funcCount; i++) {
            emitFunction(module->functions[i]);
        }
        ModuleOb->print(llvm::outs(), nullptr);
    }

    void emitFunction(IRFunction* func) {
        // All functions return Value (Int64)
        llvm::FunctionType *FT = llvm::FunctionType::get(Builder->getInt64Ty(), false);
        llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, func->name, ModuleOb.get());

        ssaValues.clear();
        ssaValues.resize(func->nextSsaVal + 2048, nullptr);
        blockMap.clear();

        // Pass 1: Create all blocks
        for (int i = 0; i < func->blockCount; i++) {
            char name[64];
            snprintf(name, sizeof(name), "block%d", func->blocks[i]->id);
            blockMap[func->blocks[i]] = llvm::BasicBlock::Create(*Context, name, F);
        }

        // Pass 2: Emit instructions (except Phi operands)
        for (int i = 0; i < func->blockCount; i++) {
            IRBasicBlock* irBlock = func->blocks[i];
            llvm::BasicBlock* llvmBlock = blockMap[irBlock];
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
                    llvm::PHINode* phi = llvm::cast<llvm::PHINode>(ssaValues[instr->result]);
                    for (int k = 0; k < instr->operandCount; k += 2) {
                        llvm::Value* val = getOperand(instr->operands[k]);
                        llvm::BasicBlock* incomingBB = blockMap[instr->operands[k+1].as.block];
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
             Builder->CreateRet(llvm::ConstantInt::get(*Context, llvm::APInt(64, nilVal, false)));
         }

        // Verify function
        std::string err;
        llvm::raw_string_ostream os(err);
        if (llvm::verifyFunction(*F, &os)) {
            std::cerr << "LLVM Verification Error: " << os.str() << "\n";
        }
    }

    void emitInstruction(IRInstruction* instr) {
        switch (instr->opcode) {
            case IR_OP_CONST: {
                llvm::Value* v = nullptr;
                if (IS_NUMBER(instr->operands[0].as.constant)) {
                    // Just a double encoded as int64
                    double num = AS_NUMBER(instr->operands[0].as.constant);
                    uint64_t bits;
                    memcpy(&bits, &num, sizeof(double));
                    v = llvm::ConstantInt::get(*Context, llvm::APInt(64, bits, false));
                } else if (IS_STRING(instr->operands[0].as.constant)) {
                    // Define global string constant
                    ObjString* strObj = AS_STRING(instr->operands[0].as.constant);
                    llvm::Constant *StrConstant = llvm::ConstantDataArray::getString(*Context, strObj->chars);
                    llvm::GlobalVariable *ValidStr = new llvm::GlobalVariable(*ModuleOb, StrConstant->getType(), true,
                        llvm::GlobalValue::PrivateLinkage, StrConstant, ".str");
                    
                    // Call runtime to create ObjString
                    llvm::Function *AllocFunc = ModuleOb->getFunction("prox_rt_const_string");
                    llvm::Value* Zero = Builder->getInt32(0);
                    llvm::Value* Args[] = { Zero, Zero };
                    // GEP to get pointer to char array
                    llvm::Value* StrPtr = Builder->CreateInBoundsGEP(StrConstant->getType(), ValidStr, Args);
                    
                    v = Builder->CreateCall(AllocFunc, {
                        StrPtr,
                        Builder->getInt32(strObj->length)
                    }, "strObj");
                }
                ssaValues[instr->result] = v;
                break;
            }
            case IR_OP_ADD: {
                llvm::Value* L = getOperand(instr->operands[0]);
                llvm::Value* R = getOperand(instr->operands[1]);
                llvm::Function *AddFunc = ModuleOb->getFunction("prox_rt_add");
                if (L && R && AddFunc) ssaValues[instr->result] = Builder->CreateCall(AddFunc, {L, R}, "addtmp");
                break;
            }
            // TODO: Implement other math ops similarly with runtime helpers OR inline check
            
            case IR_OP_JUMP: {
                Builder->CreateBr(blockMap[instr->operands[0].as.block]);
                break;
            }
            case IR_OP_JUMP_IF: {
                llvm::Value* Cond = getOperand(instr->operands[0]);
                llvm::BasicBlock* Then = blockMap[instr->operands[1].as.block];
                llvm::BasicBlock* Else = blockMap[instr->operands[2].as.block];
                
                // Temporary: Treat 0 as false (legacy behavior until full type lowering)
                 if (Cond) {
                    llvm::Value* boolCond = Builder->CreateICmpNE(Cond, llvm::ConstantInt::get(*Context, llvm::APInt(64, 0)), "ifcond");
                    Builder->CreateCondBr(boolCond, Then, Else);
                 }
                break;
            }
            case IR_OP_PHI: {
                ssaValues[instr->result] = Builder->CreatePHI(Builder->getInt64Ty(), instr->operandCount / 2, "phitmp");
                break;
            }
            case IR_OP_RETURN: {
                llvm::Value* V = getOperand(instr->operands[0]);
                // Default return NIL
                if (!V) {
                   uint64_t nilVal = 0x7ffc000000000001; 
                   V = llvm::ConstantInt::get(*Context, llvm::APInt(64, nilVal, false));
                }
                Builder->CreateRet(V);
                break;
            }
            default:
                break;
        }
    }

    llvm::Value* getOperand(IROperand& op) {
        if (op.type == OPERAND_CONST) {
             if (IS_NUMBER(op.as.constant)) {
                // Return int64 representation of double
                double num = AS_NUMBER(op.as.constant);
                uint64_t bits;
                memcpy(&bits, &num, sizeof(double));
                return llvm::ConstantInt::get(*Context, llvm::APInt(64, bits, false));
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

