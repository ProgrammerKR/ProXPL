from typing import List, Dict, Any
from core.ir.ir_format import IRModule, IRFunction, BasicBlock, Instruction, OpCode

class Optimizer:
    def __init__(self):
        pass

    def optimize(self, module: IRModule) -> IRModule:
        for func_name, func in module.functions.items():
            self.optimize_function(func)
        return module

    def optimize_function(self, func: IRFunction):
        changed = True
        while changed:
            changed = False
            # Run passes
            changed |= self.constant_folding(func)
            changed |= self.dead_code_elimination(func)
            # changed |= self.common_subexpression_elimination(func)

    def constant_folding(self, func: IRFunction) -> bool:
        """
        Scans instructions and folds constant expressions like:
        ADD t1, 5, 3  ->  MOVE t1, 8
        """
        changed = False
        for block in func.blocks:
            for i, instr in enumerate(block.instructions):
                if instr.opcode in (OpCode.ADD, OpCode.SUB, OpCode.MUL, OpCode.DIV, OpCode.MOD):
                    if isinstance(instr.arg1, (int, float)) and isinstance(instr.arg2, (int, float)):
                        # Fold
                        try:
                            res = 0
                            if instr.opcode == OpCode.ADD: res = instr.arg1 + instr.arg2
                            elif instr.opcode == OpCode.SUB: res = instr.arg1 - instr.arg2
                            elif instr.opcode == OpCode.MUL: res = instr.arg1 * instr.arg2
                            elif instr.opcode == OpCode.DIV: res = instr.arg1 / instr.arg2
                            elif instr.opcode == OpCode.MOD: res = instr.arg1 % instr.arg2
                            
                            # Replace with MOVE
                            new_instr = Instruction(OpCode.MOVE, arg1=res, result=instr.result)
                            block.instructions[i] = new_instr
                            changed = True
                        except ZeroDivisionError:
                            pass # Runtime error, don't fold
        return changed

    def dead_code_elimination(self, func: IRFunction) -> bool:
        """
        Removes instructions whose result is never used.
        Simplified version: assumes SSA-ish temps used only locally or returned.
        Uses a simple use-count analysis locally within block (global analysis is harder).
        """
        changed = False
        # 1. Collect used variables
        used = set()
        for block in func.blocks:
            for instr in block.instructions:
                if instr.opcode == OpCode.RETURN:
                    if instr.arg1: used.add(instr.arg1)
                elif instr.opcode in (OpCode.JUMP_IF,):
                     if instr.arg1: used.add(instr.arg1)
                elif instr.opcode == OpCode.CALL:
                     # Check args
                     # arg1 is dest, arg2 is func name, arg3... needs flexible handling
                     # For our simple IR, let's assume arg1 is dest, arg2 is name.
                     pass 
                else:
                    # Generic usage check for arg1/arg2 if they are temps
                    if isinstance(instr.arg1, str) and instr.arg1.startswith('t'): used.add(instr.arg1)
                    if isinstance(instr.arg2, str) and instr.arg2.startswith('t'): used.add(instr.arg2)

        # 2. Remove unused definitions
        for block in func.blocks:
            new_instrs = []
            for instr in block.instructions:
                is_dead = False
                if instr.opcode not in (OpCode.RETURN, OpCode.JUMP, OpCode.JUMP_IF, OpCode.LABEL, OpCode.CALL, OpCode.STORE, OpCode.SET_ATTR, OpCode.SET_INDEX):
                    # These instructions produce a value (MOVE, ADD, LOAD...).
                    # If result is temp and not in used set, it's dead.
                    if instr.result and isinstance(instr.result, str) and instr.result.startswith('t'):
                        if instr.result not in used:
                           is_dead = True
                
                if not is_dead:
                    new_instrs.append(instr)
                else:
                    changed = True
            
            block.instructions = new_instrs
            
        return changed

    def inline_expansion(self, module: IRModule):
        # Placeholder for function inlining
        pass
