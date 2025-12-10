from typing import List, Any, Dict, Optional
from dataclasses import dataclass
from core.ir.ir_format import OpCode, Instruction, IRModule, IRFunction

class VMError(Exception):
    pass

@dataclass
class Frame:
    function: IRFunction
    ip: int # Instruction Pointer
    locals: Dict[str, Any] # Local variables
    return_context: Optional['Frame'] = None

class VM:
    def __init__(self):
        self.stack: List[Any] = []
        self.frames: List[Frame] = []
        self.globals: Dict[str, Any] = {}
        self.ip: int = 0
        self.current_frame: Optional[Frame] = None
        self.running: bool = False

    def load(self, module: IRModule):
        # In a real bytecode VM, we'd serialize IR to bytes.
        # Here we interpret the IR directly for simplicity, acting as a High-Level VM.
        self.module = module
    
    def run(self, entry_point="main"):
        if entry_point not in self.module.functions:
            raise VMError(f"Entry point '{entry_point}' not found.")
        
        main_func = self.module.functions[entry_point]
        self.current_frame = Frame(main_func, 0, {})
        self.frames.append(self.current_frame)
        self.running = True
        
        # Flatten blocks for linear execution simulation
        # In reality, blocks are linearized during code gen.
        # We will dynamically traverse blocks here for simplification.
        self.execute_frame()

    def execute_frame(self):
        # Simple block-based interpreter
        # We start at the first block
        current_block = self.current_frame.function.blocks[0]
        ip = 0
        
        while self.running:
            if ip >= len(current_block.instructions):
                # Fallthrough or end of function without return (should imply return null)
                self.stack.append(None)
                return

            instr = current_block.instructions[ip]
            
            # --- Arithmetic ---
            if instr.opcode == OpCode.ADD:
                val = self.get_val(instr.arg1) + self.get_val(instr.arg2)
                self.set_val(instr.result, val)
                
            elif instr.opcode == OpCode.SUB:
                val = self.get_val(instr.arg1) - self.get_val(instr.arg2)
                self.set_val(instr.result, val)
            
            elif instr.opcode == OpCode.MUL:
                val = self.get_val(instr.arg1) * self.get_val(instr.arg2)
                self.set_val(instr.result, val)

            elif instr.opcode == OpCode.DIV:
                val = self.get_val(instr.arg1) / self.get_val(instr.arg2)
                self.set_val(instr.result, val)

            # --- Data Movement ---
            elif instr.opcode == OpCode.MOVE:
                val = self.get_val(instr.arg1)
                self.set_val(instr.result, val)

            elif instr.opcode == OpCode.LOAD:
                 # Load global or local logic
                 pass

            # --- Control Flow ---
            elif instr.opcode == OpCode.RETURN:
                ret_val = self.get_val(instr.arg1) if instr.arg1 is not None else None
                # Pop frame
                if len(self.frames) > 1:
                    self.frames.pop()
                    self.current_frame = self.frames[-1]
                    # Simulate pushing return value to caller's stack/temp
                    # Logic needed to know where to put it in caller
                    # Simplification: Global stack or register
                    pass
                else:
                    print(f"Program exited with: {ret_val}")
                    self.running = False
                return # Exit this frame loop
            
            elif instr.opcode == OpCode.JUMP:
                target_label = instr.arg1
                current_block = self.find_block(target_label)
                ip = 0
                continue
            
            elif instr.opcode == OpCode.JUMP_IF:
                cond = self.get_val(instr.arg1)
                if cond:
                    current_block = self.find_block(instr.arg2) # true label
                else:
                    current_block = self.find_block(instr.arg3) # false label
                ip = 0
                continue
            
            elif instr.opcode == OpCode.CALL:
                func_name = instr.arg2
                if func_name in self.module.functions:
                     # Recurse
                     new_func = self.module.functions[func_name]
                     new_frame = Frame(new_func, 0, {})
                     # Map args... (instr.arg3 etc should be list of args)
                     self.frames.append(new_frame)
                     # Execute effectively separate instance
                     # Note: This recursive structure limits stack depth to Python's recursion limit
                     # Real VM would use explicit state machine loop
                     # But for this simulation it's fine.
                     pass
                elif func_name == "print":
                    # Intrinsic
                    val = self.get_val(instr.arg3[0]) if isinstance(instr.arg3, list) else self.get_val(instr.arg1)
                    print(val)
            
            ip += 1

    def find_block(self, label: str):
        for b in self.current_frame.function.blocks:
            if b.id == label:
                return b
        raise VMError(f"Label {label} not found.")

    def get_val(self, operand: Any):
        if isinstance(operand, (int, float, str, bool, type(None))):
            return operand
        if isinstance(operand, str):
            # Check locals
            if operand in self.current_frame.locals:
                return self.current_frame.locals[operand]
            # Check globals
            if operand in self.globals:
                return self.globals[operand]
        raise VMError(f"Undefined operand: {operand}")

    def set_val(self, dest: str, value: Any):
        if dest:
            self.current_frame.locals[dest] = value
