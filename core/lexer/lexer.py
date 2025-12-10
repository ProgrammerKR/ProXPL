import re
from typing import Optional, Any
from core.tokens import TokenType, Token, KEYWORDS

class LexerError(Exception):
    def __init__(self, message, line, column):
        super().__init__(f"LexerError: {message} at {line}:{column}")

class Lexer:
    def __init__(self, source):
        self.source = source
        self.length = len(source)
        self.pos = 0
        self.line = 1
        self.column = 1
        self.current_char = self.source[0] if self.length > 0 else None

    def advance(self):
        if self.current_char == '\n':
            self.line += 1
            self.column = 0
        self.pos += 1
        self.current_char = self.source[self.pos] if self.pos < self.length else None
        self.column += 1

    def peek(self, n=1):
        pos = self.pos + n
        return self.source[pos] if pos < self.length else None

    def skip_whitespace(self):
        while self.current_char is not None and self.current_char.isspace():
            self.advance()

    def skip_comment(self):
        if self.current_char == '/' and self.peek() == '/':
            while self.current_char is not None and self.current_char != '\n':
                self.advance()
            return True
        elif self.current_char == '/' and self.peek() == '*':
            self.advance(); self.advance()
            while self.current_char is not None:
                if self.current_char == '*' and self.peek() == '/':
                    self.advance(); self.advance()
                    return True
                self.advance()
            return True
        return False

    def number(self):
        start_col = self.column
        num_str = ''
        is_float = False
        
        # Hex
        if self.current_char == '0' and self.peek() == 'x':
            self.advance(); self.advance() # 0x
            val = ''
            while self.current_char and (self.current_char.isdigit() or self.current_char.lower() in 'abcdef'):
                val += self.current_char
                self.advance()
            return Token(TokenType.NUMBER, int(val, 16), self.line, start_col)

        while self.current_char is not None and (self.current_char.isdigit() or self.current_char == '.'):
            if self.current_char == '.':
                if is_float or self.peek() == '.': break # Stop at range ..
                is_float = True
            num_str += self.current_char
            self.advance()

        val = float(num_str) if is_float else int(num_str)
        return Token(TokenType.NUMBER, val, self.line, start_col)

    def string(self):
        start_col = self.column
        quote = self.current_char
        self.advance()
        val = ''
        while self.current_char is not None and self.current_char != quote:
            if self.current_char == '\\':
                self.advance()
                if self.current_char == 'n': val += '\n'
                elif self.current_char == 't': val += '\t'
                elif self.current_char == 'r': val += '\r'
                elif self.current_char == '\\': val += '\\'
                elif self.current_char in ('"', "'"): val += self.current_char
                else: val += self.current_char
            else:
                val += self.current_char
            self.advance()
        
        if self.current_char == quote:
            self.advance()
            return Token(TokenType.STRING, val, self.line, start_col)
        raise LexerError("Unterminated string literal", self.line, start_col)

    def identifier(self):
        start_col = self.column
        val = ''
        while self.current_char is not None and (self.current_char.isalnum() or self.current_char == '_'):
            val += self.current_char
            self.advance()
        return Token(KEYWORDS.get(val, TokenType.IDENTIFIER), val, self.line, start_col)

    def next_token(self):
        while self.current_char is not None:
            if self.current_char.isspace():
                self.skip_whitespace()
                continue
            
            if self.current_char == '/' and (self.peek() == '/' or self.peek() == '*'):
                self.skip_comment()
                continue
            
            start_col = self.column

            if self.current_char.isalpha() or self.current_char == '_':
                return self.identifier()
            if self.current_char.isdigit():
                return self.number()
            if self.current_char in ('"', "'"):
                return self.string()

            # Operators and Punctuation
            char = self.current_char
            nxt = self.peek()
            
            # 3-char ops (e.g. <<=, >>=)
            if self.current_char == '<' and nxt == '<' and self.peek(2) == '=':
                 self.advance(); self.advance(); self.advance()
                 return Token(TokenType.LSHIFT_ASSIGN, '<<=', self.line, start_col)
            if self.current_char == '>' and nxt == '>' and self.peek(2) == '=':
                 self.advance(); self.advance(); self.advance()
                 return Token(TokenType.RSHIFT_ASSIGN, '>>=', self.line, start_col)
            
            # 2-char ops
            if char == '.' and nxt == '.':
                self.advance(); self.advance(); return Token(TokenType.RANGE, '..', self.line, start_col)
            if char == '?' and nxt == '.':
                self.advance(); self.advance(); return Token(TokenType.Q_DOT, '?.', self.line, start_col)
            if char == '?' and nxt == '?':
                self.advance(); self.advance(); return Token(TokenType.Q_Q, '??', self.line, start_col)
            
            if char == '=' and nxt == '>': self.advance(); self.advance(); return Token(TokenType.ARROW, '=>', self.line, start_col)
            if char == '=' and nxt == '=': self.advance(); self.advance(); return Token(TokenType.EQ, '==', self.line, start_col)
            if char == '!' and nxt == '=': self.advance(); self.advance(); return Token(TokenType.NEQ, '!=', self.line, start_col)
            if char == '<' and nxt == '=': self.advance(); self.advance(); return Token(TokenType.LTE, '<=', self.line, start_col)
            if char == '>' and nxt == '=': self.advance(); self.advance(); return Token(TokenType.GTE, '>=', self.line, start_col)
            
            if char == '+' and nxt == '+': self.advance(); self.advance(); return Token(TokenType.INC, '++', self.line, start_col)
            if char == '-' and nxt == '-': self.advance(); self.advance(); return Token(TokenType.DEC, '--', self.line, start_col)
            
            if char == '&' and nxt == '&': self.advance(); self.advance(); return Token(TokenType.AND, '&&', self.line, start_col)
            if char == '|' and nxt == '|': self.advance(); self.advance(); return Token(TokenType.OR, '||', self.line, start_col)
            
            if char == '<' and nxt == '<': self.advance(); self.advance(); return Token(TokenType.LSHIFT, '<<', self.line, start_col)
            if char == '>' and nxt == '>': self.advance(); self.advance(); return Token(TokenType.RSHIFT, '>>', self.line, start_col)
            if char == '*' and nxt == '*': 
                self.advance(); self.advance()
                if self.peek() == '=': # **=
                    self.advance()
                    return Token(TokenType.POW_ASSIGN, '**=', self.line, start_col)
                return Token(TokenType.POWER, '**', self.line, start_col)
            
            # Assignments
            if nxt == '=':
                self.advance(); self.advance()
                if char == '+': return Token(TokenType.PLUS_ASSIGN, '+=', self.line, start_col)
                if char == '-': return Token(TokenType.MINUS_ASSIGN, '-=', self.line, start_col)
                if char == '*': return Token(TokenType.MUL_ASSIGN, '*=', self.line, start_col)
                if char == '/': return Token(TokenType.DIV_ASSIGN, '/=', self.line, start_col)
                if char == '%': return Token(TokenType.MOD_ASSIGN, '%=', self.line, start_col)
                if char == '&': return Token(TokenType.AND_ASSIGN, '&=', self.line, start_col)
                if char == '|': return Token(TokenType.OR_ASSIGN, '|=', self.line, start_col)
                if char == '^': return Token(TokenType.XOR_ASSIGN, '^=', self.line, start_col)
                
            # Single char
            self.advance()
            if char == '+': return Token(TokenType.PLUS, '+', self.line, start_col)
            if char == '-': return Token(TokenType.MINUS, '-', self.line, start_col)
            if char == '*': return Token(TokenType.STAR, '*', self.line, start_col)
            if char == '/': return Token(TokenType.SLASH, '/', self.line, start_col)
            if char == '%': return Token(TokenType.PERCENT, '%', self.line, start_col)
            if char == '=': return Token(TokenType.ASSIGN, '=', self.line, start_col)
            if char == '<': return Token(TokenType.LT, '<', self.line, start_col)
            if char == '>': return Token(TokenType.GT, '>', self.line, start_col)
            if char == '!': return Token(TokenType.BANG, '!', self.line, start_col)
            if char == '&': return Token(TokenType.BIT_AND, '&', self.line, start_col)
            if char == '|': return Token(TokenType.BIT_OR, '|', self.line, start_col)
            if char == '^': return Token(TokenType.BIT_XOR, '^', self.line, start_col)
            if char == '~': return Token(TokenType.BIT_NOT, '~', self.line, start_col)
            if char == '.': return Token(TokenType.DOT, '.', self.line, start_col)
            if char == '?': return Token(TokenType.QUESTION, '?', self.line, start_col)
            if char == ':': return Token(TokenType.COLON, ':', self.line, start_col)
            
            if char == '(': return Token(TokenType.LPAREN, '(', self.line, start_col)
            if char == ')': return Token(TokenType.RPAREN, ')', self.line, start_col)
            if char == '{': return Token(TokenType.LBRACE, '{', self.line, start_col)
            if char == '}': return Token(TokenType.RBRACE, '}', self.line, start_col)
            if char == '[': return Token(TokenType.LBRACKET, '[', self.line, start_col)
            if char == ']': return Token(TokenType.RBRACKET, ']', self.line, start_col)
            if char == ',': return Token(TokenType.COMMA, ',', self.line, start_col)
            if char == ';': return Token(TokenType.SEMICOLON, ';', self.line, start_col)

            raise LexerError(f"Unexpected character '{char}'", self.line, start_col)

        return Token(TokenType.EOF, None, self.line, self.column)

    def tokenize(self):
        tokens = []
        while True:
            t = self.next_token()
            tokens.append(t)
            if t.type == TokenType.EOF: break
        return tokens

