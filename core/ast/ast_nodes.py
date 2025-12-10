from dataclasses import dataclass, field
from typing import List, Optional, Any, Union
from abc import ABC, abstractmethod

# Base Node
@dataclass
class ASTNode(ABC):
    line: int = 0
    column: int = 0

    @abstractmethod
    def accept(self, visitor):
        pass

# Expressions
@dataclass
class Expr(ASTNode):
    pass

@dataclass
class Binary(Expr):
    left: Expr
    operator: str # Token value or type name
    right: Expr
    
    def accept(self, visitor):
        return visitor.visit_binary(self)

@dataclass
class Unary(Expr):
    operator: str
    right: Expr
    
    def accept(self, visitor):
        return visitor.visit_unary(self)

@dataclass
class Literal(Expr):
    value: Any
    
    def accept(self, visitor):
        return visitor.visit_literal(self)

@dataclass
class Grouping(Expr):
    expression: Expr
    
    def accept(self, visitor):
        return visitor.visit_grouping(self)

@dataclass
class Variable(Expr):
    name: str
    
    def accept(self, visitor):
        return visitor.visit_variable(self)

@dataclass
class Assign(Expr):
    name: str # variable name
    value: Expr
    
    def accept(self, visitor):
        return visitor.visit_assign(self)

@dataclass
class Logical(Expr):
    left: Expr
    operator: str # 'and' or 'or'
    right: Expr
    
    def accept(self, visitor):
        return visitor.visit_logical(self)

@dataclass
class Call(Expr):
    callee: Expr
    arguments: List[Expr]
    
    def accept(self, visitor):
        return visitor.visit_call(self)

@dataclass
class Get(Expr):
    object: Expr
    name: str
    
    def accept(self, visitor):
        return visitor.visit_get(self)

@dataclass
class Set(Expr):
    object: Expr
    name: str
    value: Expr
    
    def accept(self, visitor):
        return visitor.visit_set(self)

@dataclass
class ListExpr(Expr):
    elements: List[Expr]
    
    def accept(self, visitor):
        return visitor.visit_list(self)

@dataclass
class Index(Expr):
    target: Expr
    index: Expr
    
    def accept(self, visitor):
        return visitor.visit_index(self)

# Statements
@dataclass
class Stmt(ASTNode):
    pass

@dataclass
class ExpressionStmt(Stmt):
    expression: Expr
    
    def accept(self, visitor):
        return visitor.visit_expression_stmt(self)

@dataclass
class VarDecl(Stmt):
    name: str
    initializer: Optional[Expr]
    is_const: bool = False
    
    def accept(self, visitor):
        return visitor.visit_var_decl(self)

@dataclass
class Block(Stmt):
    statements: List[Stmt]
    
    def accept(self, visitor):
        return visitor.visit_block(self)

@dataclass
class If(Stmt):
    condition: Expr
    then_branch: Stmt
    else_branch: Optional[Stmt]
    
    def accept(self, visitor):
        return visitor.visit_if(self)

@dataclass
class While(Stmt):
    condition: Expr
    body: Stmt
    
    def accept(self, visitor):
        return visitor.visit_while(self)

@dataclass
class For(Stmt):
    initializer: Optional[Stmt] # e.g. let i = 0
    condition: Optional[Expr]   # i < 10
    increment: Optional[Expr]   # i = i + 1
    body: Stmt
    
    def accept(self, visitor):
        return visitor.visit_for(self)

@dataclass
class FuncDecl(Stmt):
    name: str
    params: List[str]
    body: List[Stmt]
    
    def accept(self, visitor):
        return visitor.visit_func_decl(self)

@dataclass
class Return(Stmt):
    keyword: Any # Token, mostly for location info
    value: Optional[Expr]
    
    def accept(self, visitor):
        return visitor.visit_return(self)

@dataclass
class ClassDecl(Stmt):
    name: str
    methods: List[FuncDecl]
    superclass: Optional[Variable] = None
    
    def accept(self, visitor):
        return visitor.visit_class_decl(self)

# Visitor Interface
class ASTVisitor(ABC):
    @abstractmethod
    def visit_binary(self, node: Binary): pass
    @abstractmethod
    def visit_unary(self, node: Unary): pass
    @abstractmethod
    def visit_literal(self, node: Literal): pass
    @abstractmethod
    def visit_grouping(self, node: Grouping): pass
    @abstractmethod
    def visit_variable(self, node: Variable): pass
    @abstractmethod
    def visit_assign(self, node: Assign): pass
    @abstractmethod
    def visit_logical(self, node: Logical): pass
    @abstractmethod
    def visit_call(self, node: Call): pass
    @abstractmethod
    def visit_get(self, node: Get): pass
    @abstractmethod
    def visit_set(self, node: Set): pass
    @abstractmethod
    def visit_list(self, node: ListExpr): pass
    @abstractmethod
    def visit_index(self, node: Index): pass
    
    @abstractmethod
    def visit_expression_stmt(self, node: ExpressionStmt): pass
    @abstractmethod
    def visit_var_decl(self, node: VarDecl): pass
    @abstractmethod
    def visit_block(self, node: Block): pass
    @abstractmethod
    def visit_if(self, node: If): pass
    @abstractmethod
    def visit_while(self, node: While): pass
    @abstractmethod
    def visit_for(self, node: For): pass
    @abstractmethod
    def visit_func_decl(self, node: FuncDecl): pass
    @abstractmethod
    def visit_return(self, node: Return): pass
    @abstractmethod
    def visit_class_decl(self, node: ClassDecl): pass
