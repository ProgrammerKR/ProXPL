from enum import Enum, auto
from dataclasses import dataclass, field
from typing import Any, List, Dict, Set, Callable, Optional

class TypeKind(Enum):
    # Primitives
    INT = auto()
    FLOAT = auto()
    BOOL = auto()
    STRING = auto()
    NULL = auto()
    BYTES = auto()
    
    # Collections
    LIST = auto()
    DICT = auto()
    SET = auto()
    
    # Callable
    FUNCTION = auto()
    NATIVE_FUNC = auto()
    
    # Error
    ERROR = auto()

@dataclass
class Value:
    kind: TypeKind
    value: Any = None
    
    def __repr__(self):
        return f"Value({self.kind.name}, {self.value})"

# Type Definitions for Type Hints/Checking
DATA_TYPES = [
    "Int", "Float", "Bool", "String", 
    "List", "Dict", "Set", 
    "Null", 
    "Function", "NativeFunc", 
    "Bytes", "Error"
]
