# ProXPL Grammar (EBNF-ish)

program     ::= declaration* EOF ;

declaration ::= classDecl
              | funcDecl
              | varDecl
              | statement ;

classDecl   ::= "class" IDENTIFIER ( "<" IDENTIFIER )? "{" function* "}" ;
funcDecl    ::= "func" IDENTIFIER "(" parameters? ")" block ;
varDecl     ::= ( "let" | "const" ) IDENTIFIER ( "=" expression )? ";" ;

statement   ::= exprStmt
              | forStmt
              | ifStmt
              | returnStmt
              | whileStmt
              | block ;

exprStmt    ::= expression ";" ;
forStmt     ::= "for" "(" ( varDecl | exprStmt | ";" ) expression? ";" expression? ")" statement ;
ifStmt      ::= "if" "(" expression ")" statement ( "else" statement )? ;
returnStmt  ::= "return" expression? ";" ;
whileStmt   ::= "while" "(" expression ")" statement ;
block       ::= "{" declaration* "}" ;

expression  ::= assignment ;
assignment  ::= IDENTIFIER "=" assignment | logic_or ;
logic_or    ::= logic_and ( "or" logic_and )* ;
logic_and   ::= equality ( "and" equality )* ;
equality    ::= comparison ( ( "!=" | "==" ) comparison )* ;
comparison  ::= term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term        ::= factor ( ( "-" | "+" ) factor )* ;
factor      ::= unary ( ( "/" | "*" ) unary )* ;
unary       ::= ( "!" | "-" ) unary | call ;
call        ::= primary ( "(" arguments? ")" | "." IDENTIFIER )* ;
primary     ::= NUMBER | STRING | "true" | "false" | "null" | "(" expression ")" | IDENTIFIER ;
