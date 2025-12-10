from enum import Enum, auto
from dataclasses import dataclass, field
from typing import List, Optional, Any, Dict, Union

# SSA-inspired three-address code structure
# Instruction: OPCODE DEST, SRC1, SRC2

class OpCode(Enum):
    # Arithmetic
    ADD = auto()
    SUB = auto()
    MUL = auto()
    DIV = auto()
    MOD = auto()
    
    # Logic / Comparison
    EQ = auto()
    NEQ = auto()
    LT = auto()
    LTE = auto()
    GT = auto()
    GTE = auto()
    AND = auto()
    OR = auto()
    NOT = auto()
    
    # Data Movement
    LOAD = auto()   # LOAD dest, literal/var
    STORE = auto()  # STORE var, src
    MOVE = auto()   # MOVE dest, src
    
    # Control Flow
    LABEL = auto()  # LABEL label_name
    JUMP = auto()   # JUMP label
    JUMP_IF = auto() # JUMP_IF condition, true_label, false_label
    CALL = auto()   # CALL dest, func_name, args...
    RETURN = auto() # RETURN value
    
    # Complex Types
    NEW_LIST = auto()
    NEW_DICT = auto()
    GET_ATTR = auto()
    SET_ATTR = auto()
    GET_INDEX = auto()
    SET_INDEX = auto()
    
    # Special
    PHI = auto() # For SSA phi nodes (future)
    NOOP = auto()

@dataclass
class Instruction:
    opcode: OpCode
    arg1: Any = None # dest or first operand
    arg2: Any = None # second operand
    arg3: Any = None # third operand (e.g. jump targets)
    result: Any = None # destination for result if needed separate from arg1

    def __repr__(self):
        parts = [self.opcode.name]
        if self.result is not None:
            parts.append(f"{self.result} =")
        if self.arg1 is not None: parts.append(str(self.arg1))
        if self.arg2 is not None: parts.append(str(self.arg2))
        if self.arg3 is not None: parts.append(str(self.arg3))
        return " ".join(parts)

@dataclass
class BasicBlock:
    id: str
    instructions: List[Instruction] = field(default_factory=list)
    predecessors: List['BasicBlock'] = field(default_factory=list)
    successors: List['BasicBlock'] = field(default_factory=list)

    def add(self, instr: Instruction):
        self.instructions.append(instr)
    
    def __repr__(self):
        return f"Block {self.id}:\n" + "\n".join([f"  {i}" for i in self.instructions])

class IRFunction:
    def __init__(self, name: str):
        self.name = name
        self.blocks: List[BasicBlock] = []
        self.temp_counter = 0
        self.label_counter = 0

    def new_block(self, prefix="lbl"):
        name = f"{prefix}_{self.label_counter}"
        self.label_counter += 1
        block = BasicBlock(name)
        self.blocks.append(block)
        return block

    def new_temp(self):
        name = f"t{self.temp_counter}"
        self.temp_counter += 1
        return name

class IRModule:
    def __init__(self):
        self.functions: Dict[str, IRFunction] = {}
        self.globals: List[Instruction] = [] # Instructions for global scope initialization

    def add_function(self, func: IRFunction):
        self.functions[func.name] = func

    def __repr__(self):
        out = "Module IR:\n"
        for instr in self.globals:
            out += f"Global: {instr}\n"
        for name, func in self.functions.items():
            out += f"\nFunction {name}:\n"
            for block in func.blocks:
                out += str(block) + "\n"
        return out
