// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-23
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include "../../include/ir_opt.h"
#include <stdlib.h>
#include <string.h>

// Simple bitset-based dominator computation
// For larger programs, Lengauer-Tarjan would be needed.
void computeDominators(IRFunction* func, int** dominators) {
    int n = func->blockCount;
    for (int i = 0; i < n; i++) {
        // Initial state: everyone dominates everyone except for the start block
        for (int j = 0; j < n; j++) {
            dominators[i][j] = (i == 0) ? (j == 0) : 1;
        }
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 1; i < n; i++) { // Skip entry block
            IRBasicBlock* block = func->blocks[i];
            int* new_dom = (int*)malloc(sizeof(int) * n);
            for (int j = 0; j < n; j++) new_dom[j] = 1;

            for (int p = 0; p < block->predCount; p++) {
                int pred_id = block->predecessors[p]->id;
                for (int j = 0; j < n; j++) {
                    new_dom[j] &= dominators[pred_id][j];
                }
            }
            new_dom[i] = 1; // Self dominance

            for (int j = 0; j < n; j++) {
                if (dominators[i][j] != new_dom[j]) {
                    dominators[i][j] = new_dom[j];
                    changed = true;
                }
            }
            free(new_dom);
        }
    }
}

void computeDominanceFrontiers(IRFunction* func, int** dominators, int** df) {
    int n = func->blockCount;
    int* idom = (int*)malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) idom[i] = -1;

    // Find immediate dominators
    for (int i = 1; i < n; i++) {
        for (int d = 0; d < n; d++) {
            if (dominators[i][d] && i != d) {
                // Check if 'd' is the closest dominator
                bool is_idom = true;
                for (int other_d = 0; other_d < n; other_d++) {
                    if (dominators[i][other_d] && i != other_d && d != other_d) {
                        if (dominators[other_d][d]) { 
                            // other_d is further away than d (entry -> ... -> d -> other_d -> i)
                            // wait, no: if other_d dominates d, then d is closer to i.
                            // entry -> d -> other_d -> i
                        } else if (dominators[d][other_d]) {
                            // d is further away than other_d
                            is_idom = false;
                            break;
                        }
                    }
                }
                if (is_idom) idom[i] = d;
            }
        }
    }

    // Compute DF
    for (int i = 0; i < n; i++) {
        IRBasicBlock* n_block = func->blocks[i];
        if (n_block->predCount >= 2) {
            for (int p = 0; p < n_block->predCount; p++) {
                int runner = n_block->predecessors[p]->id;
                while (runner != idom[i] && runner != -1) {
                    df[runner][i] = 1;
                    if (runner == 0) break; 
                    runner = idom[runner];
                }
            }
        }
    }
    free(idom);
}

typedef struct {
    int* versions;
    int* stack;
    int stackTop;
    int stackCap;
} AllocaInfo;

static void pushVersion(AllocaInfo* info, int val) {
    if (info->stackTop >= info->stackCap) {
        info->stackCap = info->stackCap == 0 ? 8 : info->stackCap * 2;
        info->stack = (int*)realloc(info->stack, sizeof(int) * info->stackCap);
    }
    info->stack[info->stackTop++] = val;
}

static void renameRecursive(IRFunction* func, IRBasicBlock* block, int* idom, int** domChildren, int* childCount, AllocaInfo* infos, int allocaCount, int* allocas, int* reachingDefs, IRInstruction*** phiList) {
    int* pushedCounts = (int*)calloc(allocaCount, sizeof(int));

    IRInstruction* instr = block->first;
    while (instr) {
        if (instr->opcode == IR_OP_PHI) {
            // Match Phi to Alloca
            for (int a = 0; a < allocaCount; a++) {
                if (phiList[block->id][a] == instr) {
                    pushVersion(&infos[a], instr->result);
                    pushedCounts[a]++;
                    break;
                }
            }
        } else if (instr->opcode == IR_OP_STORE_VAR) {
            int target = instr->operands[0].as.ssaVal;
            int val = instr->operands[1].as.ssaVal;
            while (val >= 0 && reachingDefs[val] != -1) val = reachingDefs[val];

            for (int i = 0; i < allocaCount; i++) {
                if (allocas[i] == target) {
                    pushVersion(&infos[i], val);
                    pushedCounts[i]++;
                    instr->opcode = IR_OP_NOP; 
                    break;
                }
            }
        } else if (instr->opcode == IR_OP_LOAD_VAR) {
            int target = instr->operands[0].as.ssaVal;
            for (int i = 0; i < allocaCount; i++) {
                if (allocas[i] == target) {
                    int currentVersion = (infos[i].stackTop > 0) ? infos[i].stack[infos[i].stackTop - 1] : -1;
                    if (currentVersion != -1) {
                        reachingDefs[instr->result] = currentVersion;
                        instr->opcode = IR_OP_NOP;
                    }
                    break;
                }
            }
        } else {
            for (int i = 0; i < instr->operandCount; i++) {
                if (instr->operands[i].type == OPERAND_VAL) {
                    int s = instr->operands[i].as.ssaVal;
                    while (s >= 0 && reachingDefs[s] != -1) s = reachingDefs[s];
                    instr->operands[i].as.ssaVal = s;
                }
            }
        }
        instr = instr->next;
    }

    // Successor Phi handling
    for (int i = 0; i < block->succCount; i++) {
        IRBasicBlock* succ = block->successors[i];
        for (int a = 0; a < allocaCount; a++) {
            IRInstruction* phi = phiList[succ->id][a];
            if (phi) {
                int currentVersion = (infos[a].stackTop > 0) ? infos[a].stack[infos[a].stackTop - 1] : -1;
                // Even if -1, we might need to add something? Let's use -1 or nil.
                if (currentVersion != -1) {
                    IROperand opVal, opBlock;
                    opVal.type = OPERAND_VAL; opVal.as.ssaVal = currentVersion;
                    opBlock.type = OPERAND_BLOCK; opBlock.as.block = block;
                    addOperand(phi, opVal);
                    addOperand(phi, opBlock);
                }
            }
        }
    }

    // Recurse to dominator children
    for (int i = 0; i < childCount[block->id]; i++) {
        renameRecursive(func, func->blocks[domChildren[block->id][i]], idom, domChildren, childCount, infos, allocaCount, allocas, reachingDefs, phiList);
    }

    // Cleanup stacks
    for (int i = 0; i < allocaCount; i++) {
        infos[i].stackTop -= pushedCounts[i];
    }
    free(pushedCounts);
}

void constantFold(IRFunction* func) {
    // Map register index -> Value
    int maxReg = func->nextSsaVal + 1024;
    Value* values = (Value*)malloc(sizeof(Value) * maxReg);
    bool* isConst = (bool*)calloc(maxReg, sizeof(bool));

    for (int i = 0; i < func->blockCount; i++) {
        IRBasicBlock* block = func->blocks[i];
        IRInstruction* instr = block->first;
        while (instr) {
            if (instr->opcode == IR_OP_CONST) {
                isConst[instr->result] = true;
                values[instr->result] = instr->operands[0].as.constant;
            } else if (instr->opcode == IR_OP_ADD || instr->opcode == IR_OP_SUB || 
                       instr->opcode == IR_OP_MUL || instr->opcode == IR_OP_DIV) {
                
                Value left, right;
                bool c1 = false, c2 = false;

                if (instr->operands[0].type == OPERAND_CONST) {
                    left = instr->operands[0].as.constant;
                    c1 = true;
                } else if (instr->operands[0].type == OPERAND_VAL && instr->operands[0].as.ssaVal >= 0 && isConst[instr->operands[0].as.ssaVal]) {
                    left = values[instr->operands[0].as.ssaVal];
                    c1 = true;
                }

                if (instr->operands[1].type == OPERAND_CONST) {
                    right = instr->operands[1].as.constant;
                    c2 = true;
                } else if (instr->operands[1].type == OPERAND_VAL && instr->operands[1].as.ssaVal >= 0 && isConst[instr->operands[1].as.ssaVal]) {
                    right = values[instr->operands[1].as.ssaVal];
                    c2 = true;
                }

                if (c1 && c2) {
                    Value v_result;
                    bool folded = false;
                    if (IS_NUMBER(left) && IS_NUMBER(right)) {
                        double l = AS_NUMBER(left);
                        double r = AS_NUMBER(right);
                        if (instr->opcode == IR_OP_ADD) v_result = NUMBER_VAL(l + r);
                        else if (instr->opcode == IR_OP_SUB) v_result = NUMBER_VAL(l - r);
                        else if (instr->opcode == IR_OP_MUL) v_result = NUMBER_VAL(l * r);
                        else if (instr->opcode == IR_OP_DIV) v_result = NUMBER_VAL(l / r);
                        folded = true;
                    }

                    if (folded) {
                        instr->opcode = IR_OP_CONST;
                        instr->operandCount = 1;
                        instr->operands[0].type = OPERAND_CONST;
                        instr->operands[0].as.constant = v_result;
                        isConst[instr->result] = true;
                        values[instr->result] = v_result;
                    }
                }
            }
            instr = instr->next;
        }
    }
    free(values);
    free(isConst);
}

void deadCodeElimination(IRFunction* func) {
    // Simple DCE: Remove NOPs
    for (int i = 0; i < func->blockCount; i++) {
        IRBasicBlock* block = func->blocks[i];
        IRInstruction* instr = block->first;
        while (instr) {
            IRInstruction* next = instr->next;
            if (instr->opcode == IR_OP_NOP) {
                if (instr->prev) instr->prev->next = next;
                else block->first = next;
                if (next) next->prev = instr->prev;
                else block->last = instr->prev;
                
                free(instr->operands);
                free(instr);
            }
            instr = next;
        }
    }
}

void promoteMemoryToRegisters(IRFunction* func) {
    if (func->blockCount == 0) return;

    int n = func->blockCount;
    int** dominators = (int**)malloc(sizeof(int*) * n);
    for (int i = 0; i < n; i++) dominators[i] = (int*)calloc(n, sizeof(int));
    int** df = (int**)malloc(sizeof(int*) * n);
    for (int i = 0; i < n; i++) df[i] = (int*)calloc(n, sizeof(int));

    computeDominators(func, dominators);
    computeDominanceFrontiers(func, dominators, df);

    // Identify Allocas and their definitions
    int* allocas = (int*)malloc(sizeof(int) * 1024);
    int allocaCount = 0;
    int** defs = (int**)malloc(sizeof(int*) * 1024); // alloca_idx -> block bitset

    for (int i = 0; i < 1024; i++) defs[i] = (int*)calloc(n, sizeof(int));

    for (int i = 0; i < n; i++) {
        IRBasicBlock* block = func->blocks[i];
        IRInstruction* instr = block->first;
        while (instr) {
            if (instr->opcode == IR_OP_ALLOCA) {
                allocas[allocaCount++] = instr->result;
                // Note: we don't NOP yet because we need to know the mapping
            } else if (instr->opcode == IR_OP_STORE_VAR) {
                int target = instr->operands[0].as.ssaVal;
                // Find alloca index
                for (int a = 0; a < allocaCount; a++) {
                    if (allocas[a] == target) {
                        defs[a][i] = 1;
                        break;
                    }
                }
            }
            instr = instr->next;
        }
    }

    // 3. Iterative Phi Placement
    IRInstruction*** phiList = (IRInstruction***)malloc(sizeof(IRInstruction**) * n);
    for (int i = 0; i < n; i++) {
        phiList[i] = (IRInstruction**)malloc(sizeof(IRInstruction*) * allocaCount);
        for (int a = 0; a < allocaCount; a++) phiList[i][a] = NULL;
    }

    for (int a = 0; a < allocaCount; a++) {
        int* worklist = (int*)malloc(sizeof(int) * n);
        int wlCount = 0;
        int* hasPhi = (int*)calloc(n, sizeof(int));

        for (int i = 0; i < n; i++) if (defs[a][i]) worklist[wlCount++] = i;

        for (int i = 0; i < wlCount; i++) {
            int b = worklist[i];
            for (int f = 0; f < n; f++) {
                if (df[b][f] && !hasPhi[f]) {
                    IRInstruction* phi = createIRInstruction(IR_OP_PHI, func->nextSsaVal++);
                    phi->next = func->blocks[f]->first;
                    if (func->blocks[f]->first) func->blocks[f]->first->prev = phi;
                    func->blocks[f]->first = phi;
                    if (!func->blocks[f]->last) func->blocks[f]->last = phi;

                    phiList[f][a] = phi; // Store per alloca indexed by 'a'

                    hasPhi[f] = 1;
                    bool inWl = false;
                    for (int j = 0; j < wlCount; j++) if (worklist[j] == f) { inWl = true; break; }
                    if (!inWl) worklist[wlCount++] = f;
                }
            }
        }
        free(worklist);
        free(hasPhi);
    }

    // 4. Renaming
    int* idom = (int*)malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) idom[i] = -1;
    for (int i = 1; i < n; i++) {
        for (int d = 0; d < n; d++) {
            if (dominators[i][d] && i != d) {
                bool is_idom = true;
                for (int other_d = 0; other_d < n; other_d++) {
                    if (dominators[i][other_d] && i != other_d && d != other_d && dominators[other_d][d]) {
                        is_idom = false; break;
                    }
                }
                if (is_idom) idom[i] = d;
            }
        }
    }

    int** domChildren = (int**)malloc(sizeof(int*) * n);
    int* childCount = (int*)calloc(n, sizeof(int));
    for (int i = 0; i < n; i++) domChildren[i] = (int*)malloc(sizeof(int) * n);
    for (int i = 1; i < n; i++) {
        if (idom[i] != -1) domChildren[idom[i]][childCount[idom[i]]++] = i;
    }

    AllocaInfo* infos = (AllocaInfo*)calloc(allocaCount, sizeof(AllocaInfo));
    int* reachingDefs = (int*)malloc(sizeof(int) * (func->nextSsaVal + 1024));
    for (int i = 0; i < func->nextSsaVal + 1024; i++) reachingDefs[i] = -1;

    renameRecursive(func, func->entry, idom, domChildren, childCount, infos, allocaCount, allocas, reachingDefs, phiList);

    // NOP out all alloca instructions
    for (int i = 0; i < n; i++) {
        IRInstruction* instr = func->blocks[i]->first;
        while (instr) {
            if (instr->opcode == IR_OP_ALLOCA) instr->opcode = IR_OP_NOP;
            instr = instr->next;
        }
    }

    // Cleanup
    for (int i = 0; i < n; i++) {
        free(dominators[i]);
        free(df[i]);
        free(domChildren[i]);
        free(phiList[i]);
    }
    free(dominators);
    free(df);
    free(domChildren);
    free(childCount);
    free(idom);
    free(phiList);
    free(reachingDefs);
    for (int i = 0; i < allocaCount; i++) {
        free(infos[i].stack);
        free(defs[i]);
    }
    free(infos);
    free(defs);
    free(allocas);
}
