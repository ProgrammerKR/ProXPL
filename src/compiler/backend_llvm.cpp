#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include "../../include/backend_llvm.h"
#include "../../include/object.h"

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
        setupCoroIntrinsics();
    }

    void setupCoroIntrinsics() {
        // i8* @llvm.coro.begin(token, i8*)
        llvm::FunctionType *CoroBeginType = llvm::FunctionType::get(
            Builder->getPtrTy(),
            {llvm::Type::getTokenTy(*Context), Builder->getPtrTy()},
            false
        );
        llvm::Function::Create(CoroBeginType, llvm::Function::ExternalLinkage, "llvm.coro.begin", ModuleOb.get());

        // token @llvm.coro.id(i32, i8*, i8*, i8*)
        llvm::FunctionType *CoroIdType = llvm::FunctionType::get(
            llvm::Type::getTokenTy(*Context),
            {Builder->getInt32Ty(), Builder->getPtrTy(), Builder->getPtrTy(), Builder->getPtrTy()},
            false
        );
        llvm::Function::Create(CoroIdType, llvm::Function::ExternalLinkage, "llvm.coro.id", ModuleOb.get());

        // i32 @llvm.coro.size.i64()
        llvm::FunctionType *CoroSizeType = llvm::FunctionType::get(
            Builder->getInt64Ty(),
            {},
            false
        );
        llvm::Function::Create(CoroSizeType, llvm::Function::ExternalLinkage, "llvm.coro.size.i64", ModuleOb.get());

        // i8 @llvm.coro.suspend(token, i1)
        llvm::FunctionType *CoroSuspendType = llvm::FunctionType::get(
            Builder->getInt8Ty(),
            {llvm::Type::getTokenTy(*Context), Builder->getInt1Ty()},
            false
        );
        llvm::Function::Create(CoroSuspendType, llvm::Function::ExternalLinkage, "llvm.coro.suspend", ModuleOb.get());
        
        // i1 @llvm.coro.end(i8*, i1)
        llvm::FunctionType *CoroEndType = llvm::FunctionType::get(
            Builder->getInt1Ty(),
            {Builder->getPtrTy(), Builder->getInt1Ty()},
            false
        );
        llvm::Function::Create(CoroEndType, llvm::Function::ExternalLinkage, "llvm.coro.end", ModuleOb.get());
        
        // i8* @llvm.coro.free(token, i8*)
        llvm::FunctionType *CoroFreeType = llvm::FunctionType::get(
            Builder->getPtrTy(),
            {llvm::Type::getTokenTy(*Context), Builder->getPtrTy()},
            false
        );
        llvm::Function::Create(CoroFreeType, llvm::Function::ExternalLinkage, "llvm.coro.free", ModuleOb.get());
        
        // Runtime helper: ObjTask* prox_rt_new_task(void* hdl)
        llvm::FunctionType *NewTaskType = llvm::FunctionType::get(
             Builder->getInt64Ty(), // Value (ObjTask* encoded)
             {Builder->getPtrTy(), Builder->getPtrTy()},
             false
        );
        llvm::Function::Create(NewTaskType, llvm::Function::ExternalLinkage, "prox_rt_new_task", ModuleOb.get());
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
        llvm::FunctionType *FT = llvm::FunctionType::get(Builder->getInt64Ty(), std::vector<llvm::Type*>(), false);
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
        
        // Async Setup
        llvm::Value* CoroId = nullptr;
        llvm::Value* CoroHdl = nullptr;
        
        if (func->isAsync) {
             llvm::BasicBlock* EntryBB = &F->getEntryBlock();
             Builder->SetInsertPoint(EntryBB);
             
             llvm::Function* FCoroId = ModuleOb->getFunction("llvm.coro.id");
             llvm::Function* FCoroSize = ModuleOb->getFunction("llvm.coro.size.i64");
             llvm::Function* FCoroBegin = ModuleOb->getFunction("llvm.coro.begin");
             
             // %id = call token @llvm.coro.id(i32 0, i8* null, i8* null, i8* null)
             CoroId = Builder->CreateCall(FCoroId, {
                 Builder->getInt32(0),
                 llvm::ConstantPointerNull::get(Builder->getPtrTy()),
                 llvm::ConstantPointerNull::get(Builder->getPtrTy()),
                 llvm::ConstantPointerNull::get(Builder->getPtrTy())
             }, "coro.id");

             // %size = call i32 @llvm.coro.size.i64()
             llvm::Value* Size = Builder->CreateCall(FCoroSize, {}, "coro.size");
             
             // %alloc = call i8* @malloc(i64 %size) -- we need malloc. Use system malloc or our GC alloc?
             // Simplest: just use malloc for now or standard alloc.
             // We can use a dummy alloc for now or just rely on elision if optimization is on.
             // But for -O0 we need an allocator.
             // Let's declare malloc.
             llvm::Function* Malloc = ModuleOb->getFunction("malloc");
             if (!Malloc) {
                 llvm::FunctionType* MallocType = llvm::FunctionType::get(Builder->getPtrTy(), {Builder->getInt64Ty()}, false);
                 Malloc = llvm::Function::Create(MallocType, llvm::Function::ExternalLinkage, "malloc", ModuleOb.get());
             }
             llvm::Value* Alloc = Builder->CreateCall(Malloc, {Size}, "coro.alloc");

             // %hdl = call i8* @llvm.coro.begin(token %id, i8* %alloc)
             CoroHdl = Builder->CreateCall(FCoroBegin, {CoroId, Alloc}, "coro.hdl");
             
             // Create a Task object to return initially
             llvm::Function* NewTask = ModuleOb->getFunction("prox_rt_new_task");
             llvm::Function* ResumeFn = ModuleOb->getFunction("prox_rt_resume");
             // ResumeFn should exist since setupSchedulerHelpers called before emitModule
             llvm::Value* TaskObj = Builder->CreateCall(NewTask, {CoroHdl, ResumeFn}, "taskObj");
             
             // We need to return this TaskObj properly when function 'starts' (suspends at init).
             // But LLVM coroutines split functions. We need to handle `IR_OP_RETURN` specially too.
             // Actually, the initial return happens after the first suspend point if we want to return the handle.
             // Standard practice: Suspend immediately at start?
             // Or rely on implicit behavior.
             // Let's implement manual initial suspend point if needed, or just let 'await' trigger it.
             // Wait, for `async func`, the caller expects a `Task` back immediately.
             // So we MUST suspend at entry to return the Task.
             
             llvm::Function* FCoroSuspend = ModuleOb->getFunction("llvm.coro.suspend");
             // suspend(token, final=false)
             llvm::Value* SuspendResult = Builder->CreateCall(FCoroSuspend, {
                 llvm::ConstantTokenNone::get(*Context), 
                 Builder->getInt1(0)
             });
             
             // Switch on suspend result:
             // 0: Resume (fallthrough to function body)
             // 1: Cleanup (goto cleanup)
             // 2: Suspend (return TaskObj)
             
             llvm::BasicBlock* ResumeBB = llvm::BasicBlock::Create(*Context, "coro.resume", F);
             llvm::BasicBlock* CleanupBB = llvm::BasicBlock::Create(*Context, "coro.cleanup", F);
             llvm::BasicBlock* SuspendBB = llvm::BasicBlock::Create(*Context, "coro.suspend", F);
             
             llvm::SwitchInst* SI = Builder->CreateSwitch(SuspendResult, SuspendBB, 2);
             SI->addCase(Builder->getInt8(0), ResumeBB);
             SI->addCase(Builder->getInt8(1), CleanupBB);
             
             // Suspend Path: Return TaskObj
             Builder->SetInsertPoint(SuspendBB);
             Builder->CreateRet(TaskObj);
             
             // Cleanup Path: Free memory
             Builder->SetInsertPoint(CleanupBB);
             llvm::Function* FCoroFree = ModuleOb->getFunction("llvm.coro.free");
             llvm::Value* MemToFree = Builder->CreateCall(FCoroFree, {CoroId, CoroHdl});
             llvm::Function* Free = ModuleOb->getFunction("free");
             if (!Free) {
                 llvm::FunctionType* FreeType = llvm::FunctionType::get(Builder->getVoidTy(), {Builder->getPtrTy()}, false);
                 Free = llvm::Function::Create(FreeType, llvm::Function::ExternalLinkage, "free", ModuleOb.get());
             }
             Builder->CreateCall(Free, {MemToFree});
             // Return NIL/Undef (caller shouldn't see this)
             Builder->CreateRet(llvm::ConstantInt::get(*Context, llvm::APInt(64, 0x7ffc000000000001, false))); // NIL

             // Resume Path: Actual function body
             // Remap entry block to ResumeBB for subsequent instructions? 
             // Ideally we should have injected this logic BEFORE the first block's instructions.
             // But we are in `emitFunction` before iterating blocks.
             // The loop below `for (int i=0; ...)` visits blocks.
             // We need to jump from ResumeBB to the first logic block.
             Builder->SetInsertPoint(ResumeBB);
             Builder->CreateBr(blockMap[func->blocks[0]]); 
             
             // BUT wait, `blocks[0]` was already mapped to a BasicBlock.
             // That block is empty right now (instructions will be added in Pass 2).
             // Does `blocks[0]` have predecessors?
             // If we jump to it from ResumeBB, it's fine.
        }

        // Pass 2: Emit instructions (except Phi operands)
        for (int i = 0; i < func->blockCount; i++) {
            IRBasicBlock* irBlock = func->blocks[i];
            llvm::BasicBlock* llvmBlock = blockMap[irBlock];
            Builder->SetInsertPoint(llvmBlock);

            IRInstruction* instr = irBlock->first;
            while (instr) {
                emitInstruction(instr, CoroHdl); // Pass coroutine handle if needed
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
             if (func->isAsync) {
                 // For async, falling off end means task complete with NIL
                 // TODO: Mark task complete in runtime?
                 // coro.end
                 llvm::Function* FCoroEnd = ModuleOb->getFunction("llvm.coro.end");
                 Builder->CreateCall(FCoroEnd, {llvm::ConstantPointerNull::get(Builder->getPtrTy()), Builder->getInt1(0)});
                 Builder->CreateUnreachable(); // Should not return normally from here in coro structure
             } else {
                 uint64_t nilVal = 0x7ffc000000000001; 
                 Builder->CreateRet(llvm::ConstantInt::get(*Context, llvm::APInt(64, nilVal, false)));
             }
         }

        // Verify function
        std::string err;
        llvm::raw_string_ostream os(err);
        if (llvm::verifyFunction(*F, &os)) {
            std::cerr << "LLVM Verification Error in " << func->name << ": " << os.str() << "\n";
            // F->dump();
        }
    }

    void emitInstruction(IRInstruction* instr, llvm::Value* CoroHdl = nullptr) {
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
                // if (L && R && AddFunc) 
                ssaValues[instr->result] = Builder->CreateCall(AddFunc, {L, R}, "addtmp");
                break;
            }
            
            case IR_OP_AWAIT: {
                llvm::Value* TaskToAwait = getOperand(instr->operands[0]);

                if (!CoroHdl) {
                    // Sync await (e.g. in main)
                    // Call prox_rt_run_and_wait(TaskToAwait)
                    llvm::Function* FRunWait = ModuleOb->getFunction("prox_rt_run_and_wait");
                    if (FRunWait) {
                        ssaValues[instr->result] = Builder->CreateCall(FRunWait, {TaskToAwait}, "await_res");
                    }
                    return;
                }
                
                // We need to:
                // 1. Check if task is done? (Runtime optimization)
                // 2. If not, suspend. 
                // 3. Register callback or put in scheduler? 
                
                // For simplicity: Always suspend, let scheduler check status.
                // Call runtime to register await: prox_rt_await(CoroHdl, TaskToAwait)
                // BUT we don't have prox_rt_await declared yet.
                // And await should return the result of the task.
                
                // Call runtime to register await
                llvm::Function* FAwait = ModuleOb->getFunction("prox_rt_await");
                if (FAwait) {
                    Builder->CreateCall(FAwait, {TaskToAwait});
                }
                
                // For now, emit llvm.coro.suspend.
                llvm::Function* FCoroSuspend = ModuleOb->getFunction("llvm.coro.suspend");
                llvm::Value* SuspendResult = Builder->CreateCall(FCoroSuspend, {
                    llvm::ConstantTokenNone::get(*Context), 
                     Builder->getInt1(0)
                });
                
                llvm::BasicBlock* ResumeBB = llvm::BasicBlock::Create(*Context, "await.resume", Builder->GetInsertBlock()->getParent());
                llvm::BasicBlock* CleanupBB = llvm::BasicBlock::Create(*Context, "await.cleanup", Builder->GetInsertBlock()->getParent());
                llvm::BasicBlock* SuspendBB = llvm::BasicBlock::Create(*Context, "await.suspend", Builder->GetInsertBlock()->getParent());
                
                llvm::SwitchInst* SI = Builder->CreateSwitch(SuspendResult, SuspendBB, 2);
                SI->addCase(Builder->getInt8(0), ResumeBB);
                SI->addCase(Builder->getInt8(1), CleanupBB);
                
                // Suspend: Return (back to scheduler)
                Builder->SetInsertPoint(SuspendBB);
                // What to return? For await, we don't return a new task, we just yield control.
                // We should return something to the caller (scheduler).
                // Usually void or boolean. 
                // But our function signature returns Value (Int64).
                // Return NIL/Placeholder.
                Builder->CreateRet(llvm::ConstantInt::get(*Context, llvm::APInt(64, 0x7ffc000000000001, false))); // NIL

                // Cleanup
                Builder->SetInsertPoint(CleanupBB);
                 // ... free ...
                 llvm::Function* FCoroFree = ModuleOb->getFunction("llvm.coro.free");
                 (void)FCoroFree;
                 // We need ID for free... complex to propagate ID everywhere.
                 // For skeleton, trap.
                 Builder->CreateUnreachable();

                // Resume
                Builder->SetInsertPoint(ResumeBB);
                // Return value of await?
                // Assume runtime put result in a special slot or registers?
                // For now, return TaskToAwait (dummy result) or NIL.
                ssaValues[instr->result] = TaskToAwait; 
                break;
            }

            case IR_OP_JUMP: {
                Builder->CreateBr(blockMap[instr->operands[0].as.block]);
                break;
            }
            case IR_OP_JUMP_IF: {
                llvm::Value* Cond = getOperand(instr->operands[0]);
                llvm::BasicBlock* Then = blockMap[instr->operands[1].as.block];
                llvm::BasicBlock* Else = blockMap[instr->operands[2].as.block];
                
                // Temporary: Treat 0 as false
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
                if (!V) {
                   uint64_t nilVal = 0x7ffc000000000001; 
                   V = llvm::ConstantInt::get(*Context, llvm::APInt(64, nilVal, false));
                }
                
                if (CoroHdl) {
                    // Async return: Mark task as complete with value V
                    // prox_rt_resolve_task(CurrentTask, V) ??
                    // For now, just end.
                    llvm::Function* FCoroEnd = ModuleOb->getFunction("llvm.coro.end");
                    Builder->CreateCall(FCoroEnd, {llvm::ConstantPointerNull::get(Builder->getPtrTy()), Builder->getInt1(0)});
                    Builder->CreateUnreachable();
                } else {
                    Builder->CreateRet(V);
                }
                break;
            }
            default:
                break;
        }
    }
    

    void setupSchedulerHelpers() {
        // void prox_rt_resume(i8* hdl)
        llvm::FunctionType *ResumeType = llvm::FunctionType::get(
            Builder->getVoidTy(),
            {Builder->getPtrTy()},
            false
        );
        llvm::Function* FResume = llvm::Function::Create(ResumeType, llvm::Function::ExternalLinkage, "prox_rt_resume", ModuleOb.get());
        
        // Implement prox_rt_resume wrapper
        llvm::BasicBlock* Entry = llvm::BasicBlock::Create(*Context, "entry", FResume);
        llvm::IRBuilder<> ResBuilder(Entry);
        llvm::Function* FCoroResume = llvm::Intrinsic::getOrInsertDeclaration(ModuleOb.get(), llvm::Intrinsic::coro_resume);
        // Note: getDeclaration is deprecated but for some LLVM versions getOrInsertDeclaration is for non-intrinsics?
        // Actually, for intrinsics, getDeclaration is still standard in many versions.
        // But the warning says use getOrInsertDeclaration? 
        // Wait, Intrinsic::getDeclaration returns Function*.
        // getOrInsertDeclaration returns FunctionCallee.
        // Let's stick to getDeclaration for now if it works, or suppress warning.
        // If compilation failed with error, we change. It was a WARNING.
        // I will keep it as is, or suppress warning if strict.
        // User asked to fix ERRORS. This is a warning.
        // But let's try to update if possible.
        // llvm::Function* FCoroResume = llvm::Intrinsic::getDeclaration(ModuleOb.get(), llvm::Intrinsic::coro_resume);
        ResBuilder.CreateCall(FCoroResume, {FResume->arg_begin()});
        ResBuilder.CreateRetVoid();

        // void prox_rt_await(Value task)
        llvm::FunctionType *AwaitType = llvm::FunctionType::get(
            Builder->getVoidTy(),
            {Builder->getInt64Ty()},
            false
        );
        llvm::Function::Create(AwaitType, llvm::Function::ExternalLinkage, "prox_rt_await", ModuleOb.get());
    
        // Value prox_rt_run_and_wait(Value task)
        llvm::FunctionType *RunWaitType = llvm::FunctionType::get(
            Builder->getInt64Ty(),
            {Builder->getInt64Ty()},
            false
        );
        llvm::Function::Create(RunWaitType, llvm::Function::ExternalLinkage, "prox_rt_run_and_wait", ModuleOb.get());
    }

    // Call this from setupRuntimeTypes or Constructor
    
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
    emitter.setupSchedulerHelpers(); // Add helpers
    emitter.emitModule(module);
}

