import re
from enum import Enum, auto
from dataclasses import dataclass
from typing import Optional, Any

class TokenType(Enum):
    # Keywords
    LET = auto()
    CONST = auto()
    FUNC = auto()
    CLASS = auto()
    RETURN = auto()
    IF = auto()
    ELSE = auto()
    WHILE = auto()
    FOR = auto()
    IN = auto()
    IMPORT = auto()
    FROM = auto()
    TRUE = auto()
    FALSE = auto()
    NULL = auto()
    AND = auto()
    OR = auto()
    NOT = auto()

    # Literals
    IDENTIFIER = auto()
    NUMBER = auto()
    STRING = auto()

    # Operators
    PLUS = auto()
    MINUS = auto()
    STAR = auto()
    SLASH = auto()
    PERCENT = auto()
    EQ = auto()          # ==
    NEQ = auto()         # !=
    LT = auto()          # <
    GT = auto()          # >
    LTE = auto()         # <=
    GTE = auto()         # >=
    ASSIGN = auto()      # =
    PLUS_ASSIGN = auto() # +=
    MINUS_ASSIGN = auto() # -=

    # Delimiters
    LPAREN = auto()
    RPAREN = auto()
    LBRACE = auto()
    RBRACE = auto()
    LBRACKET = auto()
    RBRACKET = auto()
    COMMA = auto()
    DOT = auto()
    COLON = auto()
    SEMICOLON = auto()
    ARROW = auto() # ->

    EOF = auto()

@dataclass
class Token:
    type: TokenType
    value: Any
    line: int
    column: int

    def __repr__(self):
        return f"Token({self.type.name}, {repr(self.value)}, line={self.line}, col={self.column})"

KEYWORDS = {
    'let': TokenType.LET,
    'const': TokenType.CONST,
    'func': TokenType.FUNC,
    'class': TokenType.CLASS,
    'return': TokenType.RETURN,
    'if': TokenType.IF,
    'else': TokenType.ELSE,
    'while': TokenType.WHILE,
    'for': TokenType.FOR,
    'in': TokenType.IN,
    'import': TokenType.IMPORT,
    'from': TokenType.FROM,
    'true': TokenType.TRUE,
    'false': TokenType.FALSE,
    'null': TokenType.NULL,
    'and': TokenType.AND,
    'or': TokenType.OR,
    'not': TokenType.NOT
}

class LexerError(Exception):
    def __init__(self, message, line, column):
        super().__init__(f"LexerError: {message} at {line}:{column}")
        self.line = line
        self.column = column

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
        if self.pos < self.length:
            self.current_char = self.source[self.pos]
        else:
            self.current_char = None
        self.column += 1

    def peek(self):
        peek_pos = self.pos + 1
        if peek_pos < self.length:
            return self.source[peek_pos]
        return None

    def skip_whitespace(self):
        while self.current_char is not None and self.current_char.isspace():
            self.advance()

    def skip_comment(self):
        # Single line comment //
        if self.current_char == '/' and self.peek() == '/':
            while self.current_char is not None and self.current_char != '\n':
                self.advance()
            return True
        # Multi line comment /* */
        elif self.current_char == '/' and self.peek() == '*':
            self.advance() # /
            self.advance() # *
            while self.current_char is not None:
                if self.current_char == '*' and self.peek() == '/':
                    self.advance()
                    self.advance()
                    return True
                self.advance()
            return True # End of file inside comment
        return False

    def number(self):
        start_col = self.column
        num_str = ''
        is_float = False

        while self.current_char is not None and (self.current_char.isdigit() or self.current_char == '.'):
            if self.current_char == '.':
                if is_float:
                    break # Second dot
                is_float = True
            num_str += self.current_char
            self.advance()

        if is_float:
            return Token(TokenType.NUMBER, float(num_str), self.line, start_col)
        return Token(TokenType.NUMBER, int(num_str), self.line, start_col)

    def string(self):
        start_col = self.column
        quote = self.current_char # ' or "
        self.advance()
        val = ''
        
        while self.current_char is not None and self.current_char != quote:
            if self.current_char == '\\':
                self.advance()
                if self.current_char == 'n': val += '\n'
                elif self.current_char == 't': val += '\t'
                elif self.current_char == 'r': val += '\r'
                elif self.current_char == '\\': val += '\\'
                elif self.current_char == '"': val += '"'
                elif self.current_char == "'": val += "'"
                else: val += self.current_char
            else:
                val += self.current_char
            self.advance()
        
        if self.current_char == quote:
            self.advance()
            return Token(TokenType.STRING, val, self.line, start_col)
        else:
            raise LexerError("Unterminated string literal", self.line, start_col)

    def identifier(self):
        start_col = self.column
        val = ''
        while self.current_char is not None and (self.current_char.isalnum() or self.current_char == '_'):
            val += self.current_char
            self.advance()
        
        token_type = KEYWORDS.get(val, TokenType.IDENTIFIER)
        return Token(token_type, val, self.line, start_col)

    def next_token(self):
        while self.current_char is not None:
            if self.current_char.isspace():
                self.skip_whitespace()
                continue
            
            if self.current_char == '/':
                if self.peek() == '/' or self.peek() == '*':
                    self.skip_comment()
                    continue
            
            start_col = self.column

            if self.current_char.isdigit():
                return self.number()
            
            if self.current_char.isalpha() or self.current_char == '_':
                return self.identifier()
            
            if self.current_char in ('"', "'"):
                return self.string()
            
            # Operators and parsing
            if self.current_char == '+':
                if self.peek() == '=':
                    self.advance()
                    self.advance()
                    return Token(TokenType.PLUS_ASSIGN, '+=', self.line, start_col)
                self.advance()
                return Token(TokenType.PLUS, '+', self.line, start_col)
            
            if self.current_char == '-':
                if self.peek() == '>':
                    self.advance()
                    self.advance()
                    return Token(TokenType.ARROW, '->', self.line, start_col)
                if self.peek() == '=':
                    self.advance()
                    self.advance()
                    return Token(TokenType.MINUS_ASSIGN, '-=', self.line, start_col)
                self.advance()
                return Token(TokenType.MINUS, '-', self.line, start_col)
            
            if self.current_char == '*':
                self.advance()
                return Token(TokenType.STAR, '*', self.line, start_col)
            
            if self.current_char == '/':
                self.advance()
                return Token(TokenType.SLASH, '/', self.line, start_col)
            
            if self.current_char == '%':
                self.advance()
                return Token(TokenType.PERCENT, '%', self.line, start_col)

            if self.current_char == '=':
                if self.peek() == '=':
                    self.advance()
                    self.advance()
                    return Token(TokenType.EQ, '==', self.line, start_col)
                self.advance()
                return Token(TokenType.ASSIGN, '=', self.line, start_col)
            
            if self.current_char == '!':
                if self.peek() == '=':
                    self.advance()
                    self.advance()
                    return Token(TokenType.NEQ, '!=', self.line, start_col)
                # Note: '!' as NOT is handled mostly by keyword 'not', but we can support '!' if desired.
                # For now let's just error or treat as single char if we add '!' unary op.
                # Assuming 'not' is the keyword. 
                raise LexerError(f"Unexpected character '!'", self.line, start_col)

            if self.current_char == '<':
                if self.peek() == '=':
                    self.advance()
                    self.advance()
                    return Token(TokenType.LTE, '<=', self.line, start_col)
                self.advance()
                return Token(TokenType.LT, '<', self.line, start_col)
            
            if self.current_char == '>':
                if self.peek() == '=':
                    self.advance()
                    self.advance()
                    return Token(TokenType.GTE, '>=', self.line, start_col)
                self.advance()
                return Token(TokenType.GT, '>', self.line, start_col)

            if self.current_char == '(':
                self.advance()
                return Token(TokenType.LPAREN, '(', self.line, start_col)
            
            if self.current_char == ')':
                self.advance()
                return Token(TokenType.RPAREN, ')', self.line, start_col)
            
            if self.current_char == '{':
                self.advance()
                return Token(TokenType.LBRACE, '{', self.line, start_col)
            
            if self.current_char == '}':
                self.advance()
                return Token(TokenType.RBRACE, '}', self.line, start_col)

            if self.current_char == '[':
                self.advance()
                return Token(TokenType.LBRACKET, '[', self.line, start_col)
            
            if self.current_char == ']':
                self.advance()
                return Token(TokenType.RBRACKET, ']', self.line, start_col)
            
            if self.current_char == ',':
                self.advance()
                return Token(TokenType.COMMA, ',', self.line, start_col)
            
            if self.current_char == '.':
                self.advance()
                return Token(TokenType.DOT, '.', self.line, start_col)
            
            if self.current_char == ':':
                self.advance()
                return Token(TokenType.COLON, ':', self.line, start_col)
            
            if self.current_char == ';':
                self.advance()
                return Token(TokenType.SEMICOLON, ';', self.line, start_col)

            raise LexerError(f"Unexpected character '{self.current_char}'", self.line, start_col)

        return Token(TokenType.EOF, None, self.line, self.column)

    def tokenize(self):
        tokens = []
        while True:
            token = self.next_token()
            tokens.append(token)
            if token.type == TokenType.EOF:
                break
        return tokens

if __name__ == '__main__':
    # Simple test
    code = """
    func add(a, b) {
        return a + b;
    }
    let x = 10;
    // This is a comment
    /* Multi-line
       comment */
    let y = "hello";
    """
    lexer = Lexer(code)
    try:
        tokens = lexer.tokenize()
        for t in tokens:
            print(t)
    except LexerError as e:
        print(e)
