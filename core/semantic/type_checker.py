from core.ast import ast_nodes as ast
from typing import List

class SemanticError(Exception):
    def __init__(self, message, line):
        super().__init__(f"SemanticError: {message} at line {line}")

class TypeChecker(ast.ASTVisitor):
    def __init__(self):
        self.scope_stack = [{}] # Global scope
        self.errors = []
        self.in_loop = False
        self.in_func = False

    def check(self, nodes: List[ast.Stmt]):
        for node in nodes:
            try:
                node.accept(self)
            except SemanticError as e:
                self.errors.append(str(e))
        return self.errors

    def define(self, name, type_info="var"):
        if name in self.scope_stack[-1]:
            raise SemanticError(f"Variable '{name}' already defined in this scope.", 0)
        self.scope_stack[-1][name] = type_info

    def resolve(self, name):
        for scope in reversed(self.scope_stack):
            if name in scope:
                return scope[name]
        return None

    def push_scope(self):
        self.scope_stack.append({})

    def pop_scope(self):
        self.scope_stack.pop()

    # --- Visitor ---

    def visit_block(self, node: ast.Block):
        self.push_scope()
        for stmt in node.statements:
            stmt.accept(self)
        self.pop_scope()

    def visit_var_decl(self, node: ast.VarDecl):
        try:
            self.define(node.name)
        except SemanticError as e:
            # Re-raise with line info if possible, or handle
             raise SemanticError(str(e).replace("at line 0", f"at line {node.line}"), node.line)
        if node.initializer:
            node.initializer.accept(self)

    def visit_func_decl(self, node: ast.FuncDecl):
        self.define(node.name, "func")
        self.push_scope()
        prev_func = self.in_func
        self.in_func = True
        for param in node.params:
            self.define(param, "param")
        for stmt in node.body:
            stmt.accept(self)
        self.in_func = prev_func
        self.pop_scope()
    
    def visit_class_decl(self, node: ast.ClassDecl):
        self.define(node.name, "class")
        self.push_scope()
        self.define("this", "instance")
        if node.superclass:
            if not self.resolve(node.superclass.name):
                raise SemanticError(f"Superclass '{node.superclass.name}' not defined.", node.line)
        for method in node.methods:
            method.accept(self)
        self.pop_scope()

    def visit_if(self, node: ast.If):
        node.condition.accept(self)
        node.then_branch.accept(self)
        if node.else_branch:
            node.else_branch.accept(self)

    def visit_while(self, node: ast.While):
        node.condition.accept(self)
        prev_loop = self.in_loop
        self.in_loop = True
        node.body.accept(self)
        self.in_loop = prev_loop

    def visit_for(self, node: ast.For):
        self.push_scope()
        if node.initializer: node.initializer.accept(self)
        if node.condition: node.condition.accept(self)
        if node.increment: node.increment.accept(self)
        
        prev_loop = self.in_loop
        self.in_loop = True
        node.body.accept(self)
        self.in_loop = prev_loop
        
        self.pop_scope()

    def visit_switch(self, node: ast.Switch):
        node.value.accept(self)
        for val, stmts in node.cases:
            val.accept(self)
            self.push_scope()
            for s in stmts: s.accept(self)
            self.pop_scope()
        if node.default_case:
            self.push_scope()
            for s in node.default_case: s.accept(self)
            self.pop_scope()

    def visit_try_catch(self, node: ast.TryCatch):
        self.push_scope()
        for s in node.try_block: s.accept(self)
        self.pop_scope()
        
        self.push_scope()
        self.define(node.catch_var, "error")
        for s in node.catch_block: s.accept(self)
        self.pop_scope()
        
        if node.finally_block:
            self.push_scope()
            for s in node.finally_block: s.accept(self)
            self.pop_scope()

    def visit_break(self, node: ast.Break):
        if not self.in_loop:
            raise SemanticError("Break outside of loop.", node.line)

    def visit_continue(self, node: ast.Continue):
        if not self.in_loop:
            raise SemanticError("Continue outside of loop.", node.line)

    def visit_return(self, node: ast.Return):
        if not self.in_func:
            raise SemanticError("Return outside of function.", node.line)
        if node.value:
            node.value.accept(self)

    def visit_expr_stmt(self, node: ast.ExpressionStmt):
        node.expression.accept(self)

    # Expressions
    def visit_binary(self, node: ast.Binary):
        node.left.accept(self)
        node.right.accept(self)

    def visit_unary(self, node: ast.Unary):
        node.right.accept(self)

    def visit_grouping(self, node: ast.Grouping):
        node.expression.accept(self)

    def visit_literal(self, node: ast.Literal): pass

    def visit_variable(self, node: ast.Variable):
        if not self.resolve(node.name):
            raise SemanticError(f"Undefined variable '{node.name}'.", node.line)

    def visit_assign(self, node: ast.Assign):
        if not self.resolve(node.name):
             raise SemanticError(f"Undefined variable '{node.name}'.", node.line)
        node.value.accept(self)

    def visit_logical(self, node: ast.Logical):
        node.left.accept(self)
        node.right.accept(self)

    def visit_call(self, node: ast.Call):
        node.callee.accept(self)
        for arg in node.arguments: arg.accept(self)

    def visit_get(self, node: ast.Get):
        node.object.accept(self)

    def visit_set(self, node: ast.Set):
        node.object.accept(self)
        node.value.accept(self)

    def visit_index(self, node: ast.Index):
        node.target.accept(self)
        node.index.accept(self)

    def visit_list(self, node: ast.ListExpr):
        for el in node.elements: el.accept(self)

    def visit_dict(self, node: ast.DictionaryExpr):
        for k, v in node.pairs:
            k.accept(self)
            v.accept(self)

    def visit_ternary(self, node: ast.Ternary):
        node.condition.accept(self)
        node.true_branch.accept(self)
        node.false_branch.accept(self)

    def visit_lambda(self, node: ast.Lambda):
        self.push_scope()
        prev_func = self.in_func
        self.in_func = True
        for p in node.params:
            self.define(p, "param")
        for s in node.body:
            s.accept(self)
        self.in_func = prev_func
        self.pop_scope()
