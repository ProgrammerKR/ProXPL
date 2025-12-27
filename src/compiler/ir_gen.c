// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-22
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include "../../include/ir.h"
#include "../../include/ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    IRModule* module;
    IRFunction* currentFunc;
    IRBasicBlock* currentBlock;
    
    // Simple symbol table: Name -> SSA Index or Memory Location
    // In a full implementation, we'd use a scope-aware table.
    // For now, mapping names to SSA values (or stack allocas).
    struct {
       char* name;
       int ssaVal;
       bool isAlloca;
    } symbols[1024];
    int symbolCount;

    int nextReg;
} IRGen;

static int newReg(IRGen* gen) {
    int r = gen->nextReg++;
    if (r >= gen->currentFunc->nextSsaVal) {
        gen->currentFunc->nextSsaVal = r + 1;
    }
    return r;
}

static void emit(IRGen* gen, IRInstruction* instr) {
    if (!gen->currentBlock->first) {
        gen->currentBlock->first = instr;
    } else {
        gen->currentBlock->last->next = instr;
        instr->prev = gen->currentBlock->last;
    }
    gen->currentBlock->last = instr;
}

static int visitExpr(IRGen* gen, Expr* expr);

static int visitExpr(IRGen* gen, Expr* expr) {
    if (!expr) return -1;

    switch (expr->type) {
        case EXPR_LITERAL: {
            int r = newReg(gen);
            IRInstruction* instr = createIRInstruction(IR_OP_CONST, r);
            IROperand op;
            op.type = OPERAND_CONST;
            op.as.constant = expr->as.literal.value;
            addOperand(instr, op);
            emit(gen, instr);
            return r;
        }

        case EXPR_BINARY: {
            int left = visitExpr(gen, expr->as.binary.left);
            int right = visitExpr(gen, expr->as.binary.right);
            int r = newReg(gen);
            
            IROpcode opcode;
            if (strcmp(expr->as.binary.operator, "+") == 0) opcode = IR_OP_ADD;
            else if (strcmp(expr->as.binary.operator, "-") == 0) opcode = IR_OP_SUB;
            else if (strcmp(expr->as.binary.operator, "*") == 0) opcode = IR_OP_MUL;
            else if (strcmp(expr->as.binary.operator, "/") == 0) opcode = IR_OP_DIV;
            else if (strcmp(expr->as.binary.operator, "<") == 0) opcode = IR_OP_CMP_LT;
            else if (strcmp(expr->as.binary.operator, ">") == 0) opcode = IR_OP_CMP_GT;
            else if (strcmp(expr->as.binary.operator, "==") == 0) opcode = IR_OP_CMP_EQ;
            else {
                fprintf(stderr, "Unsupported IR operator: %s\n", expr->as.binary.operator);
                return -1;
            }

            IRInstruction* instr = createIRInstruction(opcode, r);
            IROperand opL, opR;
            opL.type = OPERAND_VAL; opL.as.ssaVal = left;
            opR.type = OPERAND_VAL; opR.as.ssaVal = right;
            addOperand(instr, opL);
            addOperand(instr, opR);
            emit(gen, instr);
            return r;
        }

        case EXPR_VARIABLE: {
            for (int i = 0; i < gen->symbolCount; i++) {
                if (strcmp(gen->symbols[i].name, expr->as.variable.name) == 0) {
                    if (gen->symbols[i].isAlloca) {
                        int r = newReg(gen);
                        IRInstruction* instr = createIRInstruction(IR_OP_LOAD_VAR, r);
                        IROperand op;
                        op.type = OPERAND_VAL; op.as.ssaVal = gen->symbols[i].ssaVal;
                        addOperand(instr, op);
                        emit(gen, instr);
                        return r;
                    }
                    return gen->symbols[i].ssaVal;
                }
            }
            fprintf(stderr, "Undefined IR variable: %s\n", expr->as.variable.name);
            return -1;
        }

        case EXPR_ASSIGN: {
             int val = visitExpr(gen, expr->as.assign.value);
             for (int i = 0; i < gen->symbolCount; i++) {
                if (strcmp(gen->symbols[i].name, expr->as.assign.name) == 0) {
                    if (gen->symbols[i].isAlloca) {
                        IRInstruction* instr = createIRInstruction(IR_OP_STORE_VAR, -1);
                        IROperand opDst, opVal;
                        opDst.type = OPERAND_VAL; opDst.as.ssaVal = gen->symbols[i].ssaVal;
                        opVal.type = OPERAND_VAL; opVal.as.ssaVal = val;
                        addOperand(instr, opDst);
                        addOperand(instr, opVal);
                        emit(gen, instr);
                    } else {
                        gen->symbols[i].ssaVal = val;
                    }
                    return val;
                }
            }
            return -1;
        }

        case EXPR_AWAIT: {
             int val = visitExpr(gen, expr->as.await_expr.expression);
             int r = newReg(gen);
             IRInstruction* instr = createIRInstruction(IR_OP_AWAIT, r);
             IROperand op;
             op.type = OPERAND_VAL; op.as.ssaVal = val;
             addOperand(instr, op);
             emit(gen, instr);
             return r;
        }

        default:
            return -1;
    }
}

static void visitStmt(IRGen* gen, Stmt* stmt) {
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_VAR_DECL: {
            int initVal = -1;
            if (stmt->as.var_decl.initializer) {
                initVal = visitExpr(gen, stmt->as.var_decl.initializer);
            }

            // For simplicity, we use 'alloca' pattern initially
            int r = newReg(gen);
            IRInstruction* alloc = createIRInstruction(IR_OP_ALLOCA, r);
            emit(gen, alloc);

            if (initVal != -1) {
                IRInstruction* store = createIRInstruction(IR_OP_STORE_VAR, -1);
                IROperand opDst, opVal;
                opDst.type = OPERAND_VAL; opDst.as.ssaVal = r;
                opVal.type = OPERAND_VAL; opVal.as.ssaVal = initVal;
                addOperand(store, opDst);
                addOperand(store, opVal);
                emit(gen, store);
            }

            gen->symbols[gen->symbolCount].name = strdup(stmt->as.var_decl.name);
            gen->symbols[gen->symbolCount].ssaVal = r;
            gen->symbols[gen->symbolCount].isAlloca = true;
            gen->symbolCount++;
            break;
        }

        case STMT_EXPRESSION:
            visitExpr(gen, stmt->as.expression.expression);
            break;

        case STMT_RETURN: {
            int retVal = -1;
            if (stmt->as.return_stmt.value) {
                retVal = visitExpr(gen, stmt->as.return_stmt.value);
            }
            IRInstruction* instr = createIRInstruction(IR_OP_RETURN, -1);
            if (retVal != -1) {
                IROperand op;
                op.type = OPERAND_VAL; op.as.ssaVal = retVal;
                addOperand(instr, op);
            }
            emit(gen, instr);
            break;
        }

        case STMT_IF: {
            int condReg = visitExpr(gen, stmt->as.if_stmt.condition);
            
            IRBasicBlock* thenBlock = createIRBasicBlock(gen->currentFunc);
            IRBasicBlock* elseBlock = stmt->as.if_stmt.else_branch ? createIRBasicBlock(gen->currentFunc) : NULL;
            IRBasicBlock* mergeBlock = createIRBasicBlock(gen->currentFunc);

            // Conditional jump
            IRInstruction* ji = createIRInstruction(IR_OP_JUMP_IF, -1);
            IROperand opCond, opThen;
            opCond.type = OPERAND_VAL; opCond.as.ssaVal = condReg;
            opThen.type = OPERAND_BLOCK; opThen.as.block = thenBlock;
            addOperand(ji, opCond);
            addOperand(ji, opThen);
            emit(gen, ji);

            // Unconditional jump to else or merge
            IRInstruction* j = createIRInstruction(IR_OP_JUMP, -1);
            IROperand opAlt;
            opAlt.type = OPERAND_BLOCK; opAlt.as.block = elseBlock ? elseBlock : mergeBlock;
            addOperand(j, opAlt);
            emit(gen, j);

            // 'then' branch
            gen->currentBlock = thenBlock;
            visitStmt(gen, stmt->as.if_stmt.then_branch);
            if (!thenBlock->last || thenBlock->last->opcode != IR_OP_RETURN) {
                IRInstruction* jm = createIRInstruction(IR_OP_JUMP, -1);
                IROperand opM;
                opM.type = OPERAND_BLOCK; opM.as.block = mergeBlock;
                addOperand(jm, opM);
                emit(gen, jm);
            }

            // 'else' branch
            if (elseBlock) {
                gen->currentBlock = elseBlock;
                visitStmt(gen, stmt->as.if_stmt.else_branch);
                if (!elseBlock->last || elseBlock->last->opcode != IR_OP_RETURN) {
                    IRInstruction* jm = createIRInstruction(IR_OP_JUMP, -1);
                    IROperand opM;
                    opM.type = OPERAND_BLOCK; opM.as.block = mergeBlock;
                    addOperand(jm, opM);
                    emit(gen, jm);
                }
            }

            gen->currentBlock = mergeBlock;
            break;
        }

        case STMT_WHILE: {
            IRBasicBlock* condBlock = createIRBasicBlock(gen->currentFunc);
            IRBasicBlock* loopBlock = createIRBasicBlock(gen->currentFunc);
            IRBasicBlock* afterBlock = createIRBasicBlock(gen->currentFunc);

            // Jump to condition
            IRInstruction* jc = createIRInstruction(IR_OP_JUMP, -1);
            IROperand opC;
            opC.type = OPERAND_BLOCK; opC.as.block = condBlock;
            addOperand(jc, opC);
            emit(gen, jc);

            // Condition block
            gen->currentBlock = condBlock;
            int condReg = visitExpr(gen, stmt->as.while_stmt.condition);
            IRInstruction* ji = createIRInstruction(IR_OP_JUMP_IF, -1);
            IROperand opCond, opLoop;
            opCond.type = OPERAND_VAL; opCond.as.ssaVal = condReg;
            opLoop.type = OPERAND_BLOCK; opLoop.as.block = loopBlock;
            addOperand(ji, opCond);
            addOperand(ji, opLoop);
            emit(gen, ji);

            IRInstruction* ja = createIRInstruction(IR_OP_JUMP, -1);
            IROperand opAfter;
            opAfter.type = OPERAND_BLOCK; opAfter.as.block = afterBlock;
            addOperand(ja, opAfter);
            emit(gen, ja);

            // Loop body
            gen->currentBlock = loopBlock;
            visitStmt(gen, stmt->as.while_stmt.body);
            if (!loopBlock->last || loopBlock->last->opcode != IR_OP_RETURN) {
                IRInstruction* jl = createIRInstruction(IR_OP_JUMP, -1);
                IROperand opBack;
                opBack.type = OPERAND_BLOCK; opBack.as.block = condBlock;
                addOperand(jl, opBack);
                emit(gen, jl);
            }

            gen->currentBlock = afterBlock;
            break;
        }

        case STMT_BLOCK: {
            StmtList* list = stmt->as.block.statements;
            for (int i = 0; i < list->count; i++) {
                visitStmt(gen, list->items[i]);
            }
            break;
        }

        case STMT_FUNC_DECL: {
            // Save current context
            IRFunction* prevFunc = gen->currentFunc;
            IRBasicBlock* prevBlock = gen->currentBlock;
            int prevNextReg = gen->nextReg;
            // Note: Symbol table handling here is simplified (global vs local). 
            // In a real compiler we'd push a new scope. For this skeleton, we reset symbols or use a new/nested Gen?
            // Reusing Gen but strictly we should save/restore symbol table state or use separate one.
            // For now, let's just clear symbols for the new function scope (simplified).
            
            // Create new function
            IRFunction* func = createIRFunction(stmt->as.func_decl.name, stmt->as.func_decl.isAsync);
            gen->currentFunc = func;
            gen->currentBlock = createIRBasicBlock(func);
            gen->nextReg = 0;
            // Reset symbols (TEMPORARY: destroys global view, good only for local vars in this func)
            // TODO: Proper Scoping
            int savedSymbolCount = gen->symbolCount;
            // gen->symbolCount = 0; // Keeping globals if we had them? 
            // Let's just append locals. Arguments first.
            
            StringList* params = stmt->as.func_decl.params;
            for(int i=0; i<params->count; i++) {
                 int r = newReg(gen);
                 // We need an instruction to 'receive' argument? Or just assume they are in registers 0..N?
                 // For now, map param name to register i
                 gen->symbols[gen->symbolCount].name = strdup(params->items[i]);
                 gen->symbols[gen->symbolCount].ssaVal = r;
                 gen->symbols[gen->symbolCount].isAlloca = false; 
                 gen->symbolCount++;
            }

            visitStmt(gen, createBlockStmt(stmt->as.func_decl.body, 0, 0)); // Treat body as block

            // Add to module
            if (gen->module->funcCount >= gen->module->funcCapacity) {
                gen->module->funcCapacity = gen->module->funcCapacity == 0 ? 8 : gen->module->funcCapacity * 2;
                gen->module->functions = (IRFunction**)realloc(gen->module->functions, sizeof(IRFunction*) * gen->module->funcCapacity);
            }
            gen->module->functions[gen->module->funcCount++] = func;
            
            func->nextSsaVal = gen->nextReg;
            computeCFGLinks(func);

            // Restore context
            gen->currentFunc = prevFunc;
            gen->currentBlock = prevBlock;
            gen->nextReg = prevNextReg;
            gen->symbolCount = savedSymbolCount;
            break;
        }

        default:
            break;
    }
}

IRModule* generateSSA_IR(StmtList* program) {
    IRGen gen;
    gen.module = createIRModule();
    // Main function wrapper
    gen.currentFunc = createIRFunction("main", false);
    gen.currentBlock = createIRBasicBlock(gen.currentFunc);
    gen.symbolCount = 0;
    gen.nextReg = 0;

    // Add function to module
    if (gen.module->funcCount >= gen.module->funcCapacity) {
        gen.module->funcCapacity = gen.module->funcCapacity == 0 ? 8 : gen.module->funcCapacity * 2;
        gen.module->functions = (IRFunction**)realloc(gen.module->functions, sizeof(IRFunction*) * gen.module->funcCapacity);
    }
    gen.module->functions[gen.module->funcCount++] = gen.currentFunc;

    for (int i = 0; i < program->count; i++) {
        visitStmt(&gen, program->items[i]);
    }

    gen.currentFunc->nextSsaVal = gen.nextReg;
    computeCFGLinks(gen.currentFunc);

    return gen.module;
}
