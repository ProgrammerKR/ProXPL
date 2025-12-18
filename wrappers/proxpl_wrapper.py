import ctypes
import os
import sys

# Define types
c_uint8 = ctypes.c_uint8
c_int = ctypes.c_int
c_uint64 = ctypes.c_uint64
c_void_p = ctypes.c_void_p

# Value is uint64_t (NaN-boxed)
Value = c_uint64

# Constants matching C
STACK_MAX = 256

class ValueArray(ctypes.Structure):
    _fields_ = [
        ("capacity", c_int),
        ("count", c_int),
        ("values", ctypes.POINTER(Value)),
    ]

class Chunk(ctypes.Structure):
    _fields_ = [
        ("count", c_int),
        ("capacity", c_int),
        ("code", ctypes.POINTER(c_uint8)),
        ("lines", ctypes.POINTER(c_int)),
        ("constants", ValueArray),
    ]

class VM(ctypes.Structure):
    _fields_ = [
        ("chunk", ctypes.POINTER(Chunk)),
        ("ip", ctypes.POINTER(c_uint8)),
        ("stack", Value * STACK_MAX),
        ("stackTop", ctypes.POINTER(Value)),
    ]

# Load Library
def load_proxpl_lib(dll_path=None):
    if not dll_path:
        # Default to current dir or build dir
        base_dir = os.path.dirname(os.path.abspath(__file__))
        dll_name = "proxpl.dll" if os.name == 'nt' else "libproxpl.so"
        # Try a few common locations
        paths = [
            os.path.join(base_dir, dll_name),
            os.path.join(base_dir, "..", "build", dll_name),
            os.path.join(base_dir, "..", "src", dll_name),
        ]
        for p in paths:
            if os.path.exists(p):
                dll_path = p
                break
        
        if not dll_path:
            raise FileNotFoundError(f"Could not find {dll_name}. Please build it first.")

    lib = ctypes.CDLL(dll_path)
    
    # Define Signatures
    lib.initVM.argtypes = [ctypes.POINTER(VM)]
    lib.initVM.restype = None
    
    lib.freeVM.argtypes = [ctypes.POINTER(VM)]
    lib.freeVM.restype = None
    
    lib.interpret.argtypes = [ctypes.POINTER(VM), ctypes.POINTER(Chunk)]
    lib.interpret.restype = c_int # InterpretResult enum
    
    lib.initChunk.argtypes = [ctypes.POINTER(Chunk)]
    lib.initChunk.restype = None
    
    lib.freeChunk.argtypes = [ctypes.POINTER(Chunk)]
    lib.freeChunk.restype = None
    
    lib.writeChunk.argtypes = [ctypes.POINTER(Chunk), c_uint8, c_int]
    lib.writeChunk.restype = None
    
    lib.addConstant.argtypes = [ctypes.POINTER(Chunk), Value]
    lib.addConstant.restype = c_int
    
    return lib

class ProXVM:
    def __init__(self, dll_path=None):
        self.lib = load_proxpl_lib(dll_path)
        self.vm = VM()
        self.lib.initVM(ctypes.byref(self.vm))
        
    def __del__(self):
        # self.lib.freeVM(ctypes.byref(self.vm)) # Careful with GC order
        pass

    def run_bytecode(self, opcodes, constants_list):
        # Create a new chunk
        chunk = Chunk()
        self.lib.initChunk(ctypes.byref(chunk))
        
        # Add constants
        # Note: We need a helper to convert Py objects to Value (NaN boxed)
        # For now, simplistic number support
        for c in constants_list:
            val = self._to_value(c)
            self.lib.addConstant(ctypes.byref(chunk), val)
            
        # Write opcodes
        for op in opcodes:
            self.lib.writeChunk(ctypes.byref(chunk), op, 1) # line 1
            
        # Run
        result = self.lib.interpret(ctypes.byref(self.vm), ctypes.byref(chunk))
        
        # Clean
        self.lib.freeChunk(ctypes.byref(chunk))
        return result

    def _to_value(self, py_obj):
        # Implement NaN boxing encoding here or call a C helper if exposed
        # This is tricky in pure Python without bit manipulation helper.
        # But for double (Number), we can use struct.pack
        import struct
        if isinstance(py_obj, (int, float)):
            # Just a double
            return struct.unpack('<Q', struct.pack('<d', float(py_obj)))[0]
        # Todo: Booleans, Null, Objects
        return 0 # Null

if __name__ == "__main__":
    print("ProXPL Wrapper loaded.")
    # Example usage:
    # vm = ProXVM()
    # OP_CONSTANT = 0
    # OP_RETURN = 12
    # vm.run_bytecode([OP_CONSTANT, 0, OP_RETURN], [1.23])
