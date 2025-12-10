from dataclasses import dataclass
from typing import List, Optional, Any
from abc import ABC, abstractmethod

# Base
@dataclass(kw_only=True)
class ASTNode(ABC):
    line: int = 0
    column: int = 0
    @abstractmethod
    def accept(self, visitor): pass

# --- Expressions ---
class Expr(ASTNode): pass

@dataclass
class Binary(Expr):
    left: Expr; operator: str; right: Expr
    def accept(self, v): return v.visit_binary(self)

@dataclass
class Unary(Expr):
    operator: str; right: Expr
    def accept(self, v): return v.visit_unary(self)

@dataclass
class Literal(Expr):
    value: Any
    def accept(self, v): return v.visit_literal(self)

@dataclass
class Grouping(Expr):
    expression: Expr
    def accept(self, v): return v.visit_grouping(self)

@dataclass
class Variable(Expr):
    name: str
    def accept(self, v): return v.visit_variable(self)

@dataclass
class Assign(Expr):
    name: str; value: Expr
    def accept(self, v): return v.visit_assign(self)

@dataclass
class Logical(Expr):
    left: Expr; operator: str; right: Expr
    def accept(self, v): return v.visit_logical(self)

@dataclass
class Call(Expr):
    callee: Expr; arguments: List[Expr]
    def accept(self, v): return v.visit_call(self)

@dataclass
class Get(Expr):
    object: Expr; name: str
    def accept(self, v): return v.visit_get(self)

@dataclass
class Set(Expr):
    object: Expr; name: str; value: Expr
    def accept(self, v): return v.visit_set(self)

@dataclass
class Index(Expr):
    target: Expr; index: Expr
    def accept(self, v): return v.visit_index(self)

@dataclass
class ListExpr(Expr):
    elements: List[Expr]
    def accept(self, v): return v.visit_list(self)

@dataclass
class DictionaryExpr(Expr): # New
    pairs: List[tuple] # (key_expr, value_expr)
    def accept(self, v): return v.visit_dict(self)

@dataclass
class Ternary(Expr): # New
    condition: Expr; true_branch: Expr; false_branch: Expr
    def accept(self, v): return v.visit_ternary(self)

@dataclass
class Lambda(Expr): # New
    params: List[str]; body: List['Stmt']
    def accept(self, v): return v.visit_lambda(self)

# --- Statements ---
class Stmt(ASTNode): pass

@dataclass
class ExpressionStmt(Stmt):
    expression: Expr
    def accept(self, v): return v.visit_expr_stmt(self)

@dataclass
class VarDecl(Stmt):
    name: str; initializer: Optional[Expr]; is_const: bool
    def accept(self, v): return v.visit_var_decl(self)

@dataclass
class FuncDecl(Stmt):
    name: str; params: List[str]; body: List[Stmt]
    def accept(self, v): return v.visit_func_decl(self)

@dataclass
class ClassDecl(Stmt):
    name: str; superclass: Optional[Variable]; methods: List[FuncDecl]
    def accept(self, v): return v.visit_class_decl(self)

@dataclass
class UseDecl(Stmt):
    modules: List[str] # List of module names/paths
    def accept(self, v): return v.visit_use_decl(self)

@dataclass
class If(Stmt):
    condition: Expr; then_branch: Stmt; else_branch: Optional[Stmt]
    def accept(self, v): return v.visit_if(self)

@dataclass
class While(Stmt):
    condition: Expr; body: Stmt
    def accept(self, v): return v.visit_while(self)

@dataclass
class For(Stmt):
    initializer: Optional[Stmt]; condition: Optional[Expr]; increment: Optional[Expr]; body: Stmt
    def accept(self, v): return v.visit_for(self)

@dataclass
class Return(Stmt):
    value: Optional[Expr]
    def accept(self, v): return v.visit_return(self)

@dataclass
class Block(Stmt):
    statements: List[Stmt]
    def accept(self, v): return v.visit_block(self)

@dataclass
class Break(Stmt): # New
    def accept(self, v): return v.visit_break(self)

@dataclass
class Continue(Stmt): # New
    def accept(self, v): return v.visit_continue(self)

@dataclass
class Switch(Stmt): # New
    value: Expr; cases: List[tuple]; default_case: Optional[List[Stmt]]
    def accept(self, v): return v.visit_switch(self)

@dataclass
class TryCatch(Stmt): # New
    try_block: List[Stmt]; catch_var: str; catch_block: List[Stmt]; finally_block: Optional[List[Stmt]]
    def accept(self, v): return v.visit_try_catch(self)

# Visitor
class ASTVisitor(ABC):
    @abstractmethod
    def visit_binary(self, n): pass
    @abstractmethod
    def visit_unary(self, n): pass
    @abstractmethod
    def visit_literal(self, n): pass
    @abstractmethod
    def visit_grouping(self, n): pass
    @abstractmethod
    def visit_variable(self, n): pass
    @abstractmethod
    def visit_assign(self, n): pass
    @abstractmethod
    def visit_logical(self, n): pass
    @abstractmethod
    def visit_call(self, n): pass
    @abstractmethod
    def visit_get(self, n): pass
    @abstractmethod
    def visit_set(self, n): pass
    @abstractmethod
    def visit_index(self, n): pass
    @abstractmethod
    def visit_list(self, n): pass
    @abstractmethod
    def visit_dict(self, n): pass # New
    @abstractmethod
    def visit_ternary(self, n): pass # New
    @abstractmethod
    def visit_lambda(self, n): pass # New
    
    @abstractmethod
    def visit_expr_stmt(self, n): pass
    @abstractmethod
    def visit_var_decl(self, n): pass
    @abstractmethod
    def visit_func_decl(self, n): pass
    @abstractmethod
    def visit_class_decl(self, n): pass
    @abstractmethod
    def visit_use_decl(self, n): pass
    @abstractmethod
    def visit_if(self, n): pass
    @abstractmethod
    def visit_while(self, n): pass
    @abstractmethod
    def visit_for(self, n): pass
    @abstractmethod
    def visit_return(self, n): pass
    @abstractmethod
    def visit_block(self, n): pass
    @abstractmethod
    def visit_break(self, n): pass # New
    @abstractmethod
    def visit_continue(self, n): pass # New
    @abstractmethod
    def visit_switch(self, n): pass # New
    @abstractmethod
    def visit_try_catch(self, n): pass # New
