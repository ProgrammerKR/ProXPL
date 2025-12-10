from typing import Any, List, Callable
from dataclasses import dataclass, field
from core.ir.ir_format import OpCode, IRModule
from stdlib.stdlib import StdLib

@dataclass
class Frame:
    code: Any
    ip: int = 0
    locals: dict = field(default_factory=dict)
    stack: List[Any] = field(default_factory=list)

class VM:
    def __init__(self):
        self.frames: List[Frame] = []
        self.globals = {}
        self.natives = {}
        
        # Register StdLib
        StdLib.register(self)

    def define_native(self, name: str, func: Callable):
        self.natives[name] = func

    def run(self, ir_module: IRModule):
        # Simplified execution for now
        # In a real VM, we'd execute the main function's blocks
        print("[VM] Starting execution...")
        pass

    def call_native(self, name, args):
        if name in self.natives:
            return self.natives[name](args)
        raise RuntimeError(f"Native function '{name}' not found.")
