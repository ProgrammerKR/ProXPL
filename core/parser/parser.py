from typing import List, Optional, Any
from core.lexer.lexer import TokenType, Token
from core.ast import ast_nodes as ast

class ParseError(Exception):
    def __init__(self, token, message):
        self.token = token
        self.message = message
        super().__init__(f"ParseError at line {token.line}, col {token.column}: {message}")

class Parser:
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.current = 0

    def parse(self) -> List[ast.Stmt]:
        statements = []
        while not self.is_at_end():
            decl = self.declaration()
            if decl:
                statements.append(decl)
        return statements

    # --- Declarations ---

    def declaration(self) -> Optional[ast.Stmt]:
        try:
            if self.match(TokenType.FUNC):
                return self.function_declaration("function")
            if self.match(TokenType.CLASS):
                return self.class_declaration()
            if self.match(TokenType.LET):
                return self.var_declaration(is_const=False)
            if self.match(TokenType.CONST):
                return self.var_declaration(is_const=True)
            return self.statement()
        except ParseError as e:
            self.synchronize()
            print(e) # In a real compiler, we'd collect errors
            return None

    def function_declaration(self, kind: str) -> ast.FuncDecl:
        name = self.consume(TokenType.IDENTIFIER, f"Expect {kind} name.").value
        self.consume(TokenType.LPAREN, f"Expect '(' after {kind} name.")
        
        parameters = []
        if not self.check(TokenType.RPAREN):
            while True:
                if len(parameters) >= 255:
                    self.error(self.peek(), "Can't have more than 255 parameters.")
                param_name = self.consume(TokenType.IDENTIFIER, "Expect parameter name.").value
                parameters.append(param_name)
                if not self.match(TokenType.COMMA):
                    break
        
        self.consume(TokenType.RPAREN, "Expect ')' after parameters.")
        self.consume(TokenType.LBRACE, f"Expect '{{' before {kind} body.")
        body = self.block()
        return ast.FuncDecl(name=name, params=parameters, body=body, line=name.line)

    def class_declaration(self) -> ast.ClassDecl:
        name = self.consume(TokenType.IDENTIFIER, "Expect class name.")
        
        superclass = None
        if self.match(TokenType.LT):
            self.consume(TokenType.IDENTIFIER, "Expect superclass name.")
            superclass = ast.Variable(self.previous().value, line=self.previous().line)

        self.consume(TokenType.LBRACE, "Expect '{' before class body.")

        methods = []
        while not self.check(TokenType.RBRACE) and not self.is_at_end():
            methods.append(self.function_declaration("method"))
        
        self.consume(TokenType.RBRACE, "Expect '}' after class body.")
        return ast.ClassDecl(name=name.value, methods=methods, superclass=superclass, line=name.line)

    def var_declaration(self, is_const: bool) -> ast.VarDecl:
        name = self.consume(TokenType.IDENTIFIER, "Expect variable name.")
        
        initializer = None
        if self.match(TokenType.ASSIGN):
            initializer = self.expression()
        
        self.consume(TokenType.SEMICOLON, "Expect ';' after variable declaration.")
        return ast.VarDecl(name=name.value, initializer=initializer, is_const=is_const, line=name.line)

    # --- Statements ---

    def statement(self) -> ast.Stmt:
        if self.match(TokenType.FOR):
            return self.for_statement()
        if self.match(TokenType.IF):
            return self.if_statement()
        if self.match(TokenType.RETURN):
            return self.return_statement()
        if self.match(TokenType.WHILE):
            return self.while_statement()
        if self.match(TokenType.LBRACE):
            return ast.Block(self.block(), line=self.previous().line)
        
        return self.expression_statement()

    def for_statement(self) -> ast.Stmt:
        self.consume(TokenType.LPAREN, "Expect '(' after 'for'.")
        
        initializer = None
        if self.match(TokenType.SEMICOLON):
            initializer = None
        elif self.match(TokenType.LET):
            initializer = self.var_declaration(is_const=False)
        else:
            initializer = self.expression_statement()
        
        condition = None
        if not self.check(TokenType.SEMICOLON):
            condition = self.expression()
        self.consume(TokenType.SEMICOLON, "Expect ';' after loop condition.")
        
        increment = None
        if not self.check(TokenType.RPAREN):
            increment = self.expression()
        self.consume(TokenType.RPAREN, "Expect ')' after for clauses.")
        
        body = self.statement()
        
        # Desugar for to while
        # { initializer; while(cond) { body; increment; } }
        return ast.For(initializer=initializer, condition=condition, increment=increment, body=body, line=self.previous().line)

    def if_statement(self) -> ast.Stmt:
        self.consume(TokenType.LPAREN, "Expect '(' after 'if'.")
        condition = self.expression()
        self.consume(TokenType.RPAREN, "Expect ')' after if condition.")
        
        then_branch = self.statement()
        else_branch = None
        if self.match(TokenType.ELSE):
            else_branch = self.statement()
            
        return ast.If(condition=condition, then_branch=then_branch, else_branch=else_branch, line=self.previous().line)

    def return_statement(self) -> ast.Stmt:
        keyword = self.previous()
        value = None
        if not self.check(TokenType.SEMICOLON):
            value = self.expression()
        
        self.consume(TokenType.SEMICOLON, "Expect ';' after return value.")
        return ast.Return(keyword=keyword, value=value, line=keyword.line)

    def while_statement(self) -> ast.Stmt:
        self.consume(TokenType.LPAREN, "Expect '(' after 'while'.")
        condition = self.expression()
        self.consume(TokenType.RPAREN, "Expect ')' after while condition.")
        body = self.statement()
        
        return ast.While(condition=condition, body=body, line=self.previous().line)

    def block(self) -> List[ast.Stmt]:
        statements = []
        while not self.check(TokenType.RBRACE) and not self.is_at_end():
            statements.append(self.declaration())
        
        self.consume(TokenType.RBRACE, "Expect '}' after block.")
        return statements

    def expression_statement(self) -> ast.Stmt:
        expr = self.expression()
        self.consume(TokenType.SEMICOLON, "Expect ';' after expression.")
        return ast.ExpressionStmt(expression=expr, line=self.previous().line)

    # --- Expressions ---

    def expression(self) -> ast.Expr:
        return self.assignment()

    def assignment(self) -> ast.Expr:
        expr = self.or_expr()
        
        if self.match(TokenType.ASSIGN):
            equals = self.previous()
            value = self.assignment()
            
            if isinstance(expr, ast.Variable):
                return ast.Assign(name=expr.name, value=value, line=equals.line)
            elif isinstance(expr, ast.Get):
                return ast.Set(object=expr.object, name=expr.name, value=value, line=equals.line)
            
            self.error(equals, "Invalid assignment target.")
            
        if self.match(TokenType.PLUS_ASSIGN):
            # Desugar a += b to a = a + b
            # Implementation needs careful handling of ast nodes
            pass # Simplified for now

        return expr

    def or_expr(self) -> ast.Expr:
        expr = self.and_expr()
        
        while self.match(TokenType.OR):
            operator = self.previous()
            right = self.and_expr()
            expr = ast.Logical(left=expr, operator=operator.value, right=right, line=operator.line)
            
        return expr

    def and_expr(self) -> ast.Expr:
        expr = self.equality()
        
        while self.match(TokenType.AND):
            operator = self.previous()
            right = self.equality()
            expr = ast.Logical(left=expr, operator=operator.value, right=right, line=operator.line)
            
        return expr

    def equality(self) -> ast.Expr:
        expr = self.comparison()
        
        while self.match(TokenType.EQ, TokenType.NEQ):
            operator = self.previous()
            right = self.comparison()
            expr = ast.Binary(left=expr, operator=operator.value, right=right, line=operator.line)
            
        return expr

    def comparison(self) -> ast.Expr:
        expr = self.term()
        
        while self.match(TokenType.GT, TokenType.GTE, TokenType.LT, TokenType.LTE):
            operator = self.previous()
            right = self.term()
            expr = ast.Binary(left=expr, operator=operator.value, right=right, line=operator.line)
            
        return expr

    def term(self) -> ast.Expr:
        expr = self.factor()
        
        while self.match(TokenType.MINUS, TokenType.PLUS):
            operator = self.previous()
            right = self.factor()
            expr = ast.Binary(left=expr, operator=operator.value, right=right, line=operator.line)
        
        return expr

    def factor(self) -> ast.Expr:
        expr = self.unary()
        
        while self.match(TokenType.SLASH, TokenType.STAR, TokenType.PERCENT):
            operator = self.previous()
            right = self.unary()
            expr = ast.Binary(left=expr, operator=operator.value, right=right, line=operator.line)
        
        return expr

    def unary(self) -> ast.Expr:
        if self.match(TokenType.NOT, TokenType.MINUS):
            operator = self.previous()
            right = self.unary()
            return ast.Unary(operator=operator.value, right=right, line=operator.line)
        
        return self.call()

    def call(self) -> ast.Expr:
        expr = self.primary()
        
        while True:
            if self.match(TokenType.LPAREN):
                expr = self.finish_call(expr)
            elif self.match(TokenType.DOT):
                name = self.consume(TokenType.IDENTIFIER, "Expect property name after '.'.")
                expr = ast.Get(object=expr, name=name.value, line=name.line)
            elif self.match(TokenType.LBRACKET):
                index = self.expression()
                self.consume(TokenType.RBRACKET, "Expect ']' after index.")
                expr = ast.Index(target=expr, index=index, line=self.previous().line)
            else:
                break
        
        return expr

    def finish_call(self, callee: ast.Expr) -> ast.Expr:
        arguments = []
        if not self.check(TokenType.RPAREN):
            while True:
                if len(arguments) >= 255:
                    self.error(self.peek(), "Can't have more than 255 arguments.")
                arguments.append(self.expression())
                if not self.match(TokenType.COMMA):
                    break
        
        paren = self.consume(TokenType.RPAREN, "Expect ')' after arguments.")
        return ast.Call(callee=callee, arguments=arguments, line=paren.line)

    def primary(self) -> ast.Expr:
        if self.match(TokenType.FALSE): return ast.Literal(False, line=self.previous().line)
        if self.match(TokenType.TRUE): return ast.Literal(True, line=self.previous().line)
        if self.match(TokenType.NULL): return ast.Literal(None, line=self.previous().line)
        
        if self.match(TokenType.NUMBER, TokenType.STRING):
            return ast.Literal(self.previous().value, line=self.previous().line)
        
        if self.match(TokenType.IDENTIFIER):
            return ast.Variable(self.previous().value, line=self.previous().line)
        
        if self.match(TokenType.LPAREN):
            expr = self.expression()
            self.consume(TokenType.RPAREN, "Expect ')' after expression.")
            return ast.Grouping(expression=expr, line=self.previous().line)
            
        if self.match(TokenType.LBRACKET):
            elements = []
            if not self.check(TokenType.RBRACKET):
                while True:
                    elements.append(self.expression())
                    if not self.match(TokenType.COMMA):
                        break
            self.consume(TokenType.RBRACKET, "Expect ']' after list elements.")
            return ast.ListExpr(elements=elements, line=self.previous().line)
            
        raise self.error(self.peek(), "Expect expression.")


    # --- Helpers ---

    def match(self, *types: TokenType) -> bool:
        for t in types:
            if self.check(t):
                self.advance()
                return True
        return False

    def check(self, type: TokenType) -> bool:
        if self.is_at_end(): return False
        return self.peek().type == type

    def advance(self) -> Token:
        if not self.is_at_end():
            self.current += 1
        return self.previous()

    def is_at_end(self) -> bool:
        return self.peek().type == TokenType.EOF

    def peek(self) -> Token:
        return self.tokens[self.current]

    def previous(self) -> Token:
        return self.tokens[self.current - 1]

    def consume(self, type: TokenType, message: str) -> Token:
        if self.check(type):
            return self.advance()
        raise self.error(self.peek(), message)

    def error(self, token: Token, message: str):
        return ParseError(token, message)

    def synchronize(self):
        self.advance()
        while not self.is_at_end():
            if self.previous().type == TokenType.SEMICOLON:
                return
            
            if self.peek().type in (TokenType.CLASS, TokenType.FUNC, TokenType.LET, TokenType.CONST, 
                                    TokenType.FOR, TokenType.IF, TokenType.WHILE, TokenType.RETURN):
                return
            
            self.advance()
