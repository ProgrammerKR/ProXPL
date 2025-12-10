from core.ir.ir_format import IRModule, OpCode

class CPUCodeGen:
    """
    Translates ProXPL IR into native CPU instructions (Assembly).
    Currently supports: x86_64 (Intel syntax)
    """

    def __init__(self):
        self.output = []

    def generate(self, ir_module: IRModule) -> str:
        self.emit("section .text")
        self.emit("global main")
        
        for name, func in ir_module.functions.items():
            self.emit_function(func)
            
        return "\n".join(self.output)

    def emit_function(self, func):
        self.emit(f"{func.name}:")
        # Prologue
        self.emit("  push rbp")
        self.emit("  mov rbp, rsp")
        
        for block in func.blocks:
            self.emit(f"  ; Block {block.id}")
            for instr in block.instructions:
                self.emit_instr(instr)
                
        # Epilogue (simplified, assume return handled in IR)
        # self.emit("  pop rbp")
        # self.emit("  ret")

    def emit_instr(self, instr):
        # Very rough mapping
        if instr.opcode == OpCode.ADD:
            # Assume args are registers for now
            self.emit(f"  add {instr.arg1}, {instr.arg2} ; Store in {instr.arg1}")
        elif instr.opcode == OpCode.RETURN:
            self.emit("  pop rbp")
            self.emit("  ret")
        elif instr.opcode == OpCode.CALL:
            self.emit(f"  call {instr.arg2}")
            
    def emit(self, line):
        self.output.append(line)

codegen = CPUCodeGen()
