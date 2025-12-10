from typing import List, Optional
from core.lexer.lexer import TokenType, Token
from core.ast import ast_nodes as ast

class ParseError(Exception):
    def __init__(self, token, message):
        super().__init__(f"ParseError: {message} at {token.line}:{token.column}")

class Parser:
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.current = 0

    def parse(self) -> List[ast.Stmt]:
        statements = []
        while not self.is_at_end():
            decl = self.declaration()
            if decl: statements.append(decl)
        return statements

    # --- Declarations ---
    def declaration(self):
        try:
            if self.match(TokenType.FUNC): return self.func_decl("function")
            if self.match(TokenType.CLASS): return self.class_decl()
            if self.match(TokenType.LET, TokenType.CONST): return self.var_decl()
            return self.statement()
        except ParseError as e:
            self.synchronize()
            print(e)
            return None

    def func_decl(self, kind: str):
        name = self.consume(TokenType.IDENTIFIER, f"Expect {kind} name.").value
        self.consume(TokenType.LPAREN, "Expect '(' after name.")
        params = []
        if not self.check(TokenType.RPAREN):
            while True:
                params.append(self.consume(TokenType.IDENTIFIER, "Expect param name.").value)
                if not self.match(TokenType.COMMA): break
        self.consume(TokenType.RPAREN, "Expect ')' after params.")
        self.consume(TokenType.LBRACE, "Expect '{' before body.")
        body = self.block()
        return ast.FuncDecl(name, params, body, line=name.line)

    def class_decl(self):
        name = self.consume(TokenType.IDENTIFIER, "Expect class name.")
        super_cls = None
        if self.match(TokenType.EXTENDS):
            super_cls = ast.Variable(self.consume(TokenType.IDENTIFIER, "Expect superclass.").value)
        self.consume(TokenType.LBRACE, "Expect '{'.")
        methods = []
        while not self.check(TokenType.RBRACE) and not self.is_at_end():
            methods.append(self.func_decl("method"))
        self.consume(TokenType.RBRACE, "Expect '}'.")
        return ast.ClassDecl(name.value, super_cls, methods, line=name.line)

    def var_decl(self):
        is_const = (self.previous().type == TokenType.CONST)
        name = self.consume(TokenType.IDENTIFIER, "Expect var name.")
        init = None
        if self.match(TokenType.ASSIGN):
            init = self.expression()
        self.consume(TokenType.SEMICOLON, "Expect ';'.")
        return ast.VarDecl(name.value, init, is_const, line=name.line)

    # --- Statements ---
    def statement(self):
        if self.match(TokenType.FOR): return self.for_stmt()
        if self.match(TokenType.IF): return self.if_stmt()
        if self.match(TokenType.WHILE): return self.while_stmt()
        if self.match(TokenType.SWITCH): return self.switch_stmt()
        if self.match(TokenType.TRY): return self.try_stmt()
        if self.match(TokenType.RETURN): return self.return_stmt()
        if self.match(TokenType.BREAK): return self.break_stmt()
        if self.match(TokenType.CONTINUE): return self.continue_stmt()
        if self.match(TokenType.LBRACE): return ast.Block(self.block(), line=self.previous().line)
        return self.expr_stmt()

    def for_stmt(self):
        self.consume(TokenType.LPAREN, "Expect '('.")
        init = None
        if self.match(TokenType.SEMICOLON): pass
        elif self.match(TokenType.LET, TokenType.CONST): init = self.var_decl()
        else: init = self.expr_stmt()
        
        cond = self.expression() if not self.check(TokenType.SEMICOLON) else None
        self.consume(TokenType.SEMICOLON, "Expect ';'.")
        
        incr = self.expression() if not self.check(TokenType.RPAREN) else None
        self.consume(TokenType.RPAREN, "Expect ')'.")
        
        body = self.statement()
        return ast.For(init, cond, incr, body, line=self.previous().line)

    def if_stmt(self):
        self.consume(TokenType.LPAREN, "Expect '('.")
        cond = self.expression()
        self.consume(TokenType.RPAREN, "Expect ')'.")
        then = self.statement()
        else_ = self.statement() if self.match(TokenType.ELSE) else None
        return ast.If(cond, then, else_, line=self.previous().line)

    def while_stmt(self):
        self.consume(TokenType.LPAREN, "Expect '('.")
        cond = self.expression()
        self.consume(TokenType.RPAREN, "Expect ')'.")
        return ast.While(cond, self.statement(), line=self.previous().line)
    
    def switch_stmt(self):
        self.consume(TokenType.LPAREN, "Expect '('.")
        val = self.expression()
        self.consume(TokenType.RPAREN, "Expect ')'.")
        self.consume(TokenType.LBRACE, "Expect '{'.")
        cases = []
        default_case = None
        while not self.check(TokenType.RBRACE) and not self.is_at_end():
            if self.match(TokenType.CASE):
                case_val = self.expression()
                self.consume(TokenType.COLON, "Expect ':'.")
                stmts = []
                while not self.check(TokenType.CASE) and not self.check(TokenType.DEFAULT) and not self.check(TokenType.RBRACE):
                    stmts.append(self.statement())
                cases.append((case_val, stmts))
            elif self.match(TokenType.DEFAULT):
                self.consume(TokenType.COLON, "Expect ':'.")
                stmts = []
                while not self.check(TokenType.CASE) and not self.check(TokenType.DEFAULT) and not self.check(TokenType.RBRACE):
                    stmts.append(self.statement())
                default_case = stmts
        self.consume(TokenType.RBRACE, "Expect '}'.")
        return ast.Switch(val, cases, default_case, line=self.previous().line)

    def try_stmt(self):
        self.consume(TokenType.LBRACE, "Expect '{'.")
        try_blk = self.block()
        catch_blk = []
        catch_var = "err"
        finally_blk = None
        
        if self.match(TokenType.CATCH):
            if self.match(TokenType.LPAREN):
                catch_var = self.consume(TokenType.IDENTIFIER, "Expect err name.").value
                self.consume(TokenType.RPAREN, "Expect ')'.")
            self.consume(TokenType.LBRACE, "Expect '{'.")
            catch_blk = self.block()
        
        if self.match(TokenType.FINALLY):
            self.consume(TokenType.LBRACE, "Expect '{'.")
            finally_blk = self.block()
            
        return ast.TryCatch(try_blk, catch_var, catch_blk, finally_blk, line=self.previous().line)

    def return_stmt(self):
        key = self.previous()
        val = self.expression() if not self.check(TokenType.SEMICOLON) else None
        self.consume(TokenType.SEMICOLON, "Expect ';'.")
        return ast.Return(val, line=key.line)

    def break_stmt(self):
        key = self.previous()
        self.consume(TokenType.SEMICOLON, "Expect ';'.")
        return ast.Break(line=key.line)

    def continue_stmt(self):
        key = self.previous()
        self.consume(TokenType.SEMICOLON, "Expect ';'.")
        return ast.Continue(line=key.line)

    def block(self):
        stmts = []
        while not self.check(TokenType.RBRACE) and not self.is_at_end():
            stmts.append(self.declaration())
        self.consume(TokenType.RBRACE, "Expect '}'.")
        return stmts

    def expr_stmt(self):
        expr = self.expression()
        self.consume(TokenType.SEMICOLON, "Expect ';'.")
        return ast.ExpressionStmt(expr, line=self.previous().line)

    # --- Expressions ---
    def expression(self):
        return self.assignment()

    def assignment(self):
        expr = self.ternary()
        if self.match(TokenType.ASSIGN, TokenType.PLUS_ASSIGN, TokenType.MINUS_ASSIGN):
            # Simplified for now: only simple ASSIGN fully supported in AST
            equals = self.previous()
            val = self.assignment()
            if isinstance(expr, ast.Variable):
                return ast.Assign(expr.name, val, line=equals.line)
            # handle Set() for objects
        return expr

    def ternary(self):
        expr = self.or_expr()
        if self.match(TokenType.QUESTION):
            true_br = self.expression()
            self.consume(TokenType.COLON, "Expect ':'.")
            false_br = self.expression()
            return ast.Ternary(expr, true_br, false_br, line=self.previous().line)
        return expr

    def or_expr(self):
        expr = self.and_expr()
        while self.match(TokenType.OR):
            op = self.previous().value
            right = self.and_expr()
            expr = ast.Logical(expr, op, right, line=self.previous().line)
        return expr

    def and_expr(self):
        expr = self.equality()
        while self.match(TokenType.AND):
            op = self.previous().value
            right = self.equality()
            expr = ast.Logical(expr, op, right, line=self.previous().line)
        return expr

    def equality(self):
        expr = self.comparison()
        while self.match(TokenType.EQ, TokenType.NEQ):
            op = self.previous().value
            right = self.comparison()
            expr = ast.Binary(expr, op, right, line=self.previous().line)
        return expr

    def comparison(self):
        expr = self.term()
        while self.match(TokenType.GT, TokenType.GTE, TokenType.LT, TokenType.LTE):
            op = self.previous().value
            right = self.term()
            expr = ast.Binary(expr, op, right, line=self.previous().line)
        return expr

    def term(self):
        expr = self.factor()
        while self.match(TokenType.PLUS, TokenType.MINUS):
            op = self.previous().value
            right = self.factor()
            expr = ast.Binary(expr, op, right, line=self.previous().line)
        return expr

    def factor(self):
        expr = self.unary()
        while self.match(TokenType.SLASH, TokenType.STAR, TokenType.PERCENT, TokenType.POWER):
            op = self.previous().value
            right = self.unary()
            expr = ast.Binary(expr, op, right, line=self.previous().line)
        return expr

    def unary(self):
        if self.match(TokenType.BANG, TokenType.MINUS):
            op = self.previous().value
            right = self.unary()
            return ast.Unary(op, right, line=self.previous().line)
        return self.call()

    def call(self):
        expr = self.primary()
        while True:
            if self.match(TokenType.LPAREN):
                args = []
                if not self.check(TokenType.RPAREN):
                    while True:
                        args.append(self.expression())
                        if not self.match(TokenType.COMMA): break
                self.consume(TokenType.RPAREN, "Expect ')'.")
                expr = ast.Call(expr, args, line=self.previous().line)
            elif self.match(TokenType.DOT):
                name = self.consume(TokenType.IDENTIFIER, "Expect prop name.").value
                expr = ast.Get(expr, name, line=self.previous().line)
            elif self.match(TokenType.LBRACKET):
                idx = self.expression()
                self.consume(TokenType.RBRACKET, "Expect ']'.")
                expr = ast.Index(expr, idx, line=self.previous().line)
            else: break
        return expr

    def primary(self):
        if self.match(TokenType.FALSE): return ast.Literal(False)
        if self.match(TokenType.TRUE): return ast.Literal(True)
        if self.match(TokenType.NULL): return ast.Literal(None)
        if self.match(TokenType.NUMBER, TokenType.STRING): return ast.Literal(self.previous().value)
        if self.match(TokenType.IDENTIFIER): return ast.Variable(self.previous().value)
        if self.match(TokenType.LPAREN):
            expr = self.expression()
            self.consume(TokenType.RPAREN, "Expect ')'.")
            return ast.Grouping(expr)
        if self.match(TokenType.LBRACKET): # List
            elems = []
            if not self.check(TokenType.RBRACKET):
                while True:
                    elems.append(self.expression())
                    if not self.match(TokenType.COMMA): break
            self.consume(TokenType.RBRACKET, "Expect ']'.")
            return ast.ListExpr(elems)
        
        if self.match(TokenType.LBRACE): # Dict
             pairs = []
             if not self.check(TokenType.RBRACE):
                 while True:
                     key = self.expression()
                     self.consume(TokenType.COLON, "Expect ':'.")
                     val = self.expression()
                     pairs.append((key, val))
                     if not self.match(TokenType.COMMA): break
             self.consume(TokenType.RBRACE, "Expect '}'.")
             return ast.DictionaryExpr(pairs)
             
        raise ParseError(self.peek(), "Expect expression.")

    # --- Helpers ---
    def match(self, *types):
        for t in types:
            if self.check(t):
                self.advance()
                return True
        return False

    def check(self, type):
        if self.is_at_end(): return False
        return self.peek().type == type

    def advance(self):
        if not self.is_at_end(): self.current += 1
        return self.previous()

    def is_at_end(self): return self.peek().type == TokenType.EOF
    def peek(self): return self.tokens[self.current]
    def previous(self): return self.tokens[self.current - 1]
    def consume(self, type, msg):
        if self.check(type): return self.advance()
        raise ParseError(self.peek(), msg)
    def synchronize(self):
        self.advance()
        while not self.is_at_end():
            if self.previous().type == TokenType.SEMICOLON: return
            if self.peek().type in (TokenType.CLASS, TokenType.FUNC, TokenType.LET, TokenType.FOR, TokenType.IF, TokenType.RETURN): return
            self.advance()
