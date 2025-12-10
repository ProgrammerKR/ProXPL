from typing import Dict, Any, Optional, List
from core.ast import ast_nodes as ast
from enum import Enum, auto

class Type(Enum):
    INT = auto()
    FLOAT = auto()
    BOOL = auto()
    STRING = auto()
    VOID = auto()
    NULL = auto()
    ANY = auto() # For dynamic resolution or error recovery

class SemanticError(Exception):
    def __init__(self, message, line):
        super().__init__(f"SemanticError: {message} at line {line}")

class SymbolTable:
    def __init__(self, enclosing=None):
        self.symbols: Dict[str, Any] = {}
        self.enclosing: Optional[SymbolTable] = enclosing

    def define(self, name: str, value: Any):
        self.symbols[name] = value

    def get(self, name: str):
        if name in self.symbols:
            return self.symbols[name]
        
        if self.enclosing:
            return self.enclosing.get(name)
        
        return None

    def exists(self, name: str) -> bool:
        if name in self.symbols:
            return True
        if self.enclosing:
            return self.enclosing.exists(name)
        return False

    def exists_in_current_scope(self, name: str) -> bool:
        return name in self.symbols

class TypeChecker(ast.ASTVisitor):
    def __init__(self):
        self.global_scope = SymbolTable()
        self.current_scope = self.global_scope
        self.errors = []

    def check(self, statements: List[ast.Stmt]):
        for stmt in statements:
            try:
                stmt.accept(self)
            except SemanticError as e:
                self.errors.append(str(e))
        return self.errors

    def enter_scope(self):
        self.current_scope = SymbolTable(self.current_scope)

    def exit_scope(self):
        self.current_scope = self.current_scope.enclosing

    # --- Visitor Methods ---

    def visit_block(self, node: ast.Block):
        self.enter_scope()
        for stmt in node.statements:
            stmt.accept(self)
        self.exit_scope()

    def visit_var_decl(self, node: ast.VarDecl):
        if self.current_scope.exists_in_current_scope(node.name):
            raise SemanticError(f"Variable '{node.name}' already declared in this scope.", node.line)
        
        if node.initializer:
            node.initializer.accept(self)
        
        self.current_scope.define(node.name, "var") # We can store type info here later

    def visit_func_decl(self, node: ast.FuncDecl):
        if self.current_scope.exists_in_current_scope(node.name):
            raise SemanticError(f"Function '{node.name}' already declared in this scope.", node.line)
        
        self.current_scope.define(node.name, "func")
        
        self.enter_scope()
        for param in node.params:
            self.current_scope.define(param, "param")
            
        for stmt in node.body:
            stmt.accept(self)
        self.exit_scope()

    def visit_class_decl(self, node: ast.ClassDecl):
        self.current_scope.define(node.name, "class")
        self.enter_scope()
        self.current_scope.define("this", "instance") # Define 'this'
        
        for method in node.methods:
            method.accept(self)
            
        self.exit_scope()

    def visit_variable(self, node: ast.Variable):
        if not self.current_scope.exists(node.name):
            raise SemanticError(f"Undefined variable '{node.name}'.", node.line)

    def visit_assign(self, node: ast.Assign):
        if not self.current_scope.exists(node.name):
            raise SemanticError(f"Undefined variable '{node.name}' in assignment.", node.line)
        node.value.accept(self)

    def visit_if(self, node: ast.If):
        node.condition.accept(self)
        node.then_branch.accept(self)
        if node.else_branch:
            node.else_branch.accept(self)

    def visit_while(self, node: ast.While):
        node.condition.accept(self)
        node.body.accept(self)

    def visit_for(self, node: ast.For):
        self.enter_scope()
        if node.initializer:
            node.initializer.accept(self)
        if node.condition:
            node.condition.accept(self)
        if node.increment:
            node.increment.accept(self)
        node.body.accept(self)
        self.exit_scope()

    def visit_return(self, node: ast.Return):
        if node.value:
            node.value.accept(self)

    def visit_expression_stmt(self, node: ast.ExpressionStmt):
        node.expression.accept(self)

    def visit_binary(self, node: ast.Binary):
        node.left.accept(self)
        node.right.accept(self)

    def visit_unary(self, node: ast.Unary):
        node.right.accept(self)

    def visit_grouping(self, node: ast.Grouping):
        node.expression.accept(self)

    def visit_literal(self, node: ast.Literal):
        pass # Literals are always valid

    def visit_logical(self, node: ast.Logical):
        node.left.accept(self)
        node.right.accept(self)

    def visit_call(self, node: ast.Call):
        node.callee.accept(self)
        for arg in node.arguments:
            arg.accept(self)

    def visit_get(self, node: ast.Get):
        node.object.accept(self)

    def visit_set(self, node: ast.Set):
        node.object.accept(self)
        node.value.accept(self)

    def visit_list(self, node: ast.ListExpr):
        for element in node.elements:
            element.accept(self)

    def visit_index(self, node: ast.Index):
        node.target.accept(self)
        node.index.accept(self)
