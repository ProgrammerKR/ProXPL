from enum import Enum, auto
from dataclasses import dataclass
from typing import Any, Dict

class TokenType(Enum):
    # --- Keywords (45) ---
    # Declarations
    LET = auto()
    CONST = auto()
    FUNC = auto()
    CLASS = auto()
    ENUM = auto()
    STRUCT = auto()
    INTERFACE = auto()
    
    # Control Flow
    IF = auto()
    ELSE = auto()
    WHILE = auto()
    FOR = auto()
    SWITCH = auto()
    CASE = auto()
    DEFAULT = auto()
    BREAK = auto()
    CONTINUE = auto()
    RETURN = auto()
    TRY = auto()
    CATCH = auto()
    FINALLY = auto()
    THROW = auto()
    DEFER = auto()

    # Async
    ASYNC = auto()
    AWAIT = auto()

    # Modules
    IMPORT = auto()
    FROM = auto()
    EXPORT = auto()
    AS = auto()

    # Values/Types
    TRUE = auto()
    FALSE = auto()
    NULL = auto()
    VOID = auto()
    
    # OOP
    THIS = auto()
    SUPER = auto()
    STATIC = auto()
    PUBLIC = auto()
    PRIVATE = auto()
    PROTECTED = auto()
    EXTENDS = auto()
    IMPLEMENTS = auto()
    ABSTRACT = auto()

    # Operators/Checks
    IN = auto()
    IS = auto()
    TYPEOF = auto()
    NATIVE = auto()

    # --- Literals ---
    IDENTIFIER = auto()
    NUMBER = auto()
    STRING = auto()
    CHAR = auto()

    # --- Operators (42) ---
    # Arithmetic (6)
    PLUS = auto()       # +
    MINUS = auto()      # -
    STAR = auto()       # *
    SLASH = auto()      # /
    PERCENT = auto()    # %
    POWER = auto()      # **

    # Comparison (6)
    EQ = auto()         # ==
    NEQ = auto()        # !=
    LT = auto()         # <
    GT = auto()         # >
    LTE = auto()        # <=
    GTE = auto()        # >=

    # Logical (3)
    AND = auto()        # &&
    OR = auto()         # ||
    BANG = auto()       # !

    # Bitwise (6)
    BIT_AND = auto()    # &
    BIT_OR = auto()     # |
    BIT_XOR = auto()    # ^
    BIT_NOT = auto()    # ~
    LSHIFT = auto()     # <<
    RSHIFT = auto()     # >>

    # Assignment (12)
    ASSIGN = auto()         # =
    PLUS_ASSIGN = auto()    # +=
    MINUS_ASSIGN = auto()   # -=
    MUL_ASSIGN = auto()     # *=
    DIV_ASSIGN = auto()     # /=
    MOD_ASSIGN = auto()     # %=
    POW_ASSIGN = auto()     # **=
    AND_ASSIGN = auto()     # &=
    OR_ASSIGN = auto()      # |=
    XOR_ASSIGN = auto()     # ^=
    LSHIFT_ASSIGN = auto()  # <<=
    RSHIFT_ASSIGN = auto()  # >>=

    # Special (4)
    DOT = auto()        # .
    Q_DOT = auto()      # ?.
    ARROW = auto()      # =>
    RANGE = auto()      # ..

    # Ternary / Null (3)
    Q_Q = auto()        # ??
    QUESTION = auto()   # ?
    COLON = auto()      # :

    # Increment / Decrement (2)
    INC = auto()        # ++
    DEC = auto()        # --

    # --- Delimiters ---
    LPAREN = auto()     # (
    RPAREN = auto()     # )
    LBRACE = auto()     # {
    RBRACE = auto()     # }
    LBRACKET = auto()   # [
    RBRACKET = auto()   # ]
    COMMA = auto()      # ,
    SEMICOLON = auto()  # ;

    EOF = auto()

@dataclass
class Token:
    type: TokenType
    value: Any
    line: int
    column: int

    def __repr__(self):
        return f"Token({self.type.name}, {repr(self.value)}, line={self.line}, col={self.column})"

# Exactly 45 Keywords
KEYWORDS: Dict[str, TokenType] = {
    'let': TokenType.LET,
    'const': TokenType.CONST,
    'func': TokenType.FUNC,
    'class': TokenType.CLASS,
    'enum': TokenType.ENUM,
    'struct': TokenType.STRUCT,
    'interface': TokenType.INTERFACE,
    'if': TokenType.IF,
    'else': TokenType.ELSE,
    'while': TokenType.WHILE,
    'for': TokenType.FOR,
    'switch': TokenType.SWITCH,
    'case': TokenType.CASE,
    'default': TokenType.DEFAULT,
    'break': TokenType.BREAK,
    'continue': TokenType.CONTINUE,
    'return': TokenType.RETURN,
    'try': TokenType.TRY,
    'catch': TokenType.CATCH,
    'finally': TokenType.FINALLY,
    'throw': TokenType.THROW,
    'defer': TokenType.DEFER,
    'async': TokenType.ASYNC,
    'await': TokenType.AWAIT,
    'import': TokenType.IMPORT,
    'from': TokenType.FROM,
    'export': TokenType.EXPORT,
    'as': TokenType.AS,
    'true': TokenType.TRUE,
    'false': TokenType.FALSE,
    'null': TokenType.NULL,
    'void': TokenType.VOID,
    'this': TokenType.THIS,
    'super': TokenType.SUPER,
    'static': TokenType.STATIC,
    'public': TokenType.PUBLIC,
    'private': TokenType.PRIVATE,
    'protected': TokenType.PROTECTED,
    'extends': TokenType.EXTENDS,
    'implements': TokenType.IMPLEMENTS,
    'abstract': TokenType.ABSTRACT,
    'in': TokenType.IN,
    'is': TokenType.IS,
    'typeof': TokenType.TYPEOF,
    'native': TokenType.NATIVE
}

# List of all operators for documentation checks
OPERATORS = [
    TokenType.PLUS, TokenType.MINUS, TokenType.STAR, TokenType.SLASH, TokenType.PERCENT, TokenType.POWER,
    TokenType.EQ, TokenType.NEQ, TokenType.LT, TokenType.GT, TokenType.LTE, TokenType.GTE,
    TokenType.AND, TokenType.OR, TokenType.BANG,
    TokenType.BIT_AND, TokenType.BIT_OR, TokenType.BIT_XOR, TokenType.BIT_NOT, TokenType.LSHIFT, TokenType.RSHIFT,
    TokenType.ASSIGN, TokenType.PLUS_ASSIGN, TokenType.MINUS_ASSIGN, TokenType.MUL_ASSIGN, TokenType.DIV_ASSIGN, 
    TokenType.MOD_ASSIGN, TokenType.POW_ASSIGN, TokenType.AND_ASSIGN, TokenType.OR_ASSIGN, TokenType.XOR_ASSIGN,
    TokenType.LSHIFT_ASSIGN, TokenType.RSHIFT_ASSIGN,
    TokenType.DOT, TokenType.Q_DOT, TokenType.ARROW, TokenType.RANGE,
    TokenType.Q_Q, TokenType.QUESTION, TokenType.COLON,
    TokenType.INC, TokenType.DEC
]
