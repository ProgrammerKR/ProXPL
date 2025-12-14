#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>

// Include C AST headers
extern "C" {
    #include "ast.h"
}

using namespace llvm;

class ProXPLCompiler {
public:
    ProXPLCompiler() {
        Context = std::make_unique<LLVMContext>();
        ModuleOb = std::make_unique<Module>("ProXPL Module", *Context);
        Builder = std::make_unique<IRBuilder<>>(*Context);
    }

    void compile(StmtList* program) {
        // Create main function: int main()
        FunctionType *MainFuncType = FunctionType::get(Builder->getInt32Ty(), false);
        Function *MainFunc = Function::Create(MainFuncType, Function::ExternalLinkage, "main", ModuleOb.get());
        
        BasicBlock *EntryBB = BasicBlock::Create(*Context, "entry", MainFunc);
        Builder->SetInsertPoint(EntryBB);

        // Compile statements
        if (program) {
            for (int i = 0; i < program->count; i++) {
                visitStmt(program->items[i]);
            }
        }

        // Return 0 if no explicit return
        if (!MainFunc->back().getTerminator()) {
             Builder->CreateRet(Builder->getInt32(0));
        }

        // Verify validity
        verifyFunction(*MainFunc);
    }

    void dump() {
        ModuleOb->print(errs(), nullptr);
    }

private:
    std::unique_ptr<LLVMContext> Context;
    std::unique_ptr<Module> ModuleOb;
    std::unique_ptr<IRBuilder<>> Builder;
    std::map<std::string, AllocaInst*> NamedValues; // Stack variables

    // Helper: Create an alloca in the entry block of the function
    AllocaInst* createEntryBlockAlloca(Function *TheFunction, const std::string &VarName, Type* type) {
        IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
        return TmpB.CreateAlloca(type, 0, VarName.c_str());
    }
    
    // Helper: Get LLVM Type from AST TypeKind
    Type* getLLVMType(TypeKind kind) {
        switch (kind) {
            case TYPE_INT: return Builder->getInt32Ty(); // 32-bit int
            case TYPE_FLOAT: return Builder->getDoubleTy(); // 64-bit float
            case TYPE_BOOL: return Builder->getInt1Ty();
            case TYPE_VOID: return Builder->getVoidTy();
            // TODO: String, etc.
            default: return Builder->getDoubleTy(); // Default to double (VM behavior)
        }
    }

    Value* visitExpr(Expr* expr) {
        if (!expr) return nullptr;
        
        switch (expr->type) {
            case EXPR_LITERAL: {
                // Check value type from AST Literal Value
                if (IS_BOOL(expr->as.literal.value)) {
                    return ConstantInt::get(*Context, APInt(1, AS_BOOL(expr->as.literal.value) ? 1 : 0, true));
                } else if (IS_NUMBER(expr->as.literal.value)) {
                    // Start as Double by default from VM value
                    return ConstantFP::get(*Context, APFloat(AS_NUMBER(expr->as.literal.value)));
                }
                return nullptr;
            }
            
            case EXPR_VARIABLE: {
                // Load from stack
                AllocaInst* A = NamedValues[expr->as.variable.name];
                if (!A) {
                    std::cerr << "Unknown variable: " << expr->as.variable.name << "\n";
                    return nullptr;
                }
                return Builder->CreateLoad(A->getAllocatedType(), A, expr->as.variable.name);
            }

            case EXPR_ASSIGN: {
                 Value* Val = visitExpr(expr->as.assign.value);
                 if (!Val) return nullptr;
                 
                 AllocaInst* A = NamedValues[expr->as.assign.name];
                 if (!A) {
                     std::cerr << "Unknown variable assignment: " << expr->as.assign.name << "\n";
                     return nullptr;
                 }
                 
                 // Type Cast if necessary?
                 // Simple store for now
                 Builder->CreateStore(Val, A);
                 return Val;
            }

            case EXPR_BINARY: {
                Value *L = visitExpr(expr->as.binary.left);
                Value *R = visitExpr(expr->as.binary.right);
                if (!L || !R) return nullptr;
                
                // Determine Mode based on Inferred Type or Operand Types
                // Assuming AST TypeChecker has run, we can look at logic.
                // Or simply look at LLVM Value type
                bool isFloat = L->getType()->isDoubleTy() || R->getType()->isDoubleTy();
                
                // Auto-promote for mixed - simpler than checking AST
                if (L->getType()->isIntegerTy() && R->getType()->isDoubleTy()) {
                    L = Builder->CreateSIToFP(L, Builder->getDoubleTy(), "intcast");
                } else if (L->getType()->isDoubleTy() && R->getType()->isIntegerTy()) {
                    R = Builder->CreateSIToFP(R, Builder->getDoubleTy(), "intcast");
                }

                std::string op = expr->as.binary.operator;
                if (op == "+") return isFloat ? Builder->CreateFAdd(L, R, "addtmp") : Builder->CreateAdd(L, R, "addtmp");
                if (op == "-") return isFloat ? Builder->CreateFSub(L, R, "subtmp") : Builder->CreateSub(L, R, "subtmp");
                if (op == "*") return isFloat ? Builder->CreateFMul(L, R, "multmp") : Builder->CreateMul(L, R, "multmp");
                if (op == "/") return isFloat ? Builder->CreateFDiv(L, R, "divtmp") : Builder->CreateSDiv(L, R, "divtmp"); // SDiv for signed int
                
                if (op == "<") return isFloat ? Builder->CreateFCmpULT(L, R, "cmptmp") : Builder->CreateICmpSLT(L, R, "cmptmp");
                if (op == ">") return isFloat ? Builder->CreateFCmpUGT(L, R, "cmptmp") : Builder->CreateICmpSGT(L, R, "cmptmp");
                
                return nullptr;
            }
            default:
                std::cerr << "Unknown expression type: " << expr->type << "\n";
                return nullptr;
        }
    }

    void visitStmt(Stmt* stmt) {
        if (!stmt) return;

        switch (stmt->type) {
            case STMT_VAR_DECL: {
                Function *TheFunction = Builder->GetInsertBlock()->getParent();
                const char* varName = stmt->as.var_decl.name;
                
                // Determine Type: 
                // 1. From explicit type in AST (set by TypeChecker)
                // 2. Or default to Double
                Type* llvmType = getLLVMType(stmt->as.var_decl.type.kind);
                
                // Initialize Value
                Value* InitVal = nullptr;
                if (stmt->as.var_decl.initializer) {
                    InitVal = visitExpr(stmt->as.var_decl.initializer);
                    // Match type if needed
                    if (InitVal->getType() != llvmType) {
                        // Cast logic here if strict
                    }
                } else {
                    // Default init?
                    InitVal = Constant::getNullValue(llvmType);
                }
                
                AllocaInst* Alloca = createEntryBlockAlloca(TheFunction, varName, llvmType);
                Builder->CreateStore(InitVal, Alloca);
                
                // Remember valid variable in scope
                NamedValues[varName] = Alloca; 
                break;
            }
            
            case STMT_EXPRESSION:
                visitExpr(stmt->as.expression.expression);
                break;
                
            case STMT_RETURN:
                if (stmt->as.return_stmt.value) {
                     Value* V = visitExpr(stmt->as.return_stmt.value);
                     // If main returns int, but V is double, Cast
                     if (V->getType()->isDoubleTy()) {
                         V = Builder->CreateFPToSI(V, Builder->getInt32Ty(), "cast_ret");
                     }
                     Builder->CreateRet(V);
                } else {
                     Builder->CreateRetVoid();
                }
                break;

            case STMT_IF: {
                Value* CondV = visitExpr(stmt->as.if_stmt.condition);
                if (!CondV) return;

                // Convert to bool if needed
                if (!CondV->getType()->isIntegerTy(1)) {
                    CondV = Builder->CreateFCmpONE(CondV, ConstantFP::get(*Context, APFloat(0.0)), "ifcond");
                }

                Function *TheFunction = Builder->GetInsertBlock()->getParent();
                
                BasicBlock *ThenBB = BasicBlock::Create(*Context, "then", TheFunction);
                BasicBlock *ElseBB = BasicBlock::Create(*Context, "else");
                BasicBlock *MergeBB = BasicBlock::Create(*Context, "ifcont");

                // Check if 'else' exists
                bool hasElse = (stmt->as.if_stmt.else_branch != nullptr);
                
                Builder->CreateCondBr(CondV, ThenBB, hasElse ? ElseBB : MergeBB);

                // Emit Then
                Builder->SetInsertPoint(ThenBB);
                visitStmt(stmt->as.if_stmt.then_branch);
                // Branch to Merge if not returned
                if (!ThenBB->getTerminator()) Builder->CreateBr(MergeBB);

                // Emit Else
                if (hasElse) {
                    TheFunction->getBasicBlockList().push_back(ElseBB);
                    Builder->SetInsertPoint(ElseBB);
                    visitStmt(stmt->as.if_stmt.else_branch);
                     // Branch to Merge if not returned
                    if (!ElseBB->getTerminator()) Builder->CreateBr(MergeBB);
                }

                // Emit Merge
                TheFunction->getBasicBlockList().push_back(MergeBB);
                Builder->SetInsertPoint(MergeBB);
                break;
            }

            case STMT_BLOCK: {
                 // Scoping logic for NamedValues would be needed here for true block scope
                 // For now, flat scope
                 StmtList* list = stmt->as.block.statements;
                 for (int i=0; i < list->count; i++) {
                    visitStmt(list->items[i]);
                 }
                 break;
            }

            default:
                std::cerr << "Unknown statement type: " << stmt->type << "\n";
                break;
        }
    }
};

// --- C-Compatible API ---
extern "C" {
    void generateCode(StmtList* program) {
        ProXPLCompiler compiler;
        compiler.compile(program);
        compiler.dump(); // Print IR to stdout
    }
}
