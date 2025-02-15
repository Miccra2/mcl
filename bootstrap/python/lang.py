#!/usr/bin/env python3
from copy import deepcopy
from typing import Any

IOTA_COUNTER: int = 0
def iota(value: int | None = None) -> int:
    global IOTA_COUNTER
    if isinstance(value, int):
        IOTA_COUNTER = value
    result: int = IOTA_COUNTER
    IOTA_COUNTER += 1
    return result

class TokenKind:
    UNDEFINED: int = iota(0)
    DIGITS:    int = iota()
    PLUS:      int = iota()
    SEMICOLON: int = iota()
    COUNT:     int = iota()

    def __repr__(self) -> str:
        result: str =  "TokenKind(\n"
        result     += f"    {self.UNDEFINED=},\n"
        result     += f"    {self.DIGITS=},\n"
        result     += f"    {self.PLUS=},\n"
        result     += f"    {self.SEMICOLON=},\n"
        result     +=  ")"
        return result

class Position:
    line:     int
    column:   int
    line_off: int
    offset:   int

    def __init__(self, 
                 line: int = 1, 
                 column: int = 1, 
                 line_off: int = 0, 
                 offset: int = 0) -> None:
        self.line     = line
        self.column   = column
        self.line_off = line_off
        self.offset   = offset

    def __repr__(self) -> str:
        result: str =  "Position(\n"
        result     += f"    {self.line=},\n"
        result     += f"    {self.column=},\n"
        result     += f"    {self.line_off=},\n"
        result     += f"    {self.offset=},\n"
        result     +=  ")"
        return result

class Span:
    start: Position
    end:   Position

    def __init__(self, 
                 start: Position = Position(), 
                 end: Position = Position()) -> None:
        self.start = start
        self.end   = end

    def __repr__(self) -> str:
        result: str =  "Span(\n"
        result     += f"    {f'{self.start=!r}'.replace('\n','\n    ')},\n"
        result     += f"    {f'{self.end=!r}'.replace('\n','\n    ')},\n"
        result     +=  ")"
        return result

class Token:
    kind: int
    span: Span

    def __init__(self, 
                 kind: int = TokenKind.UNDEFINED, 
                 span: Span = Span()) -> None:
        self.kind = kind
        self.span = span

    def __repr__(self) -> str:
        result: str =  "Token(\n"
        result     += f"    {self.kind=},\n"
        result     += f"    {f'{self.span=!r}'.replace('\n','\n    ')},\n"
        result     +=  ")"
        return result

class ExprKind:
    UNDEFINED: int = iota(0)
    VALUE:     int = iota()
    INFIX:     int = iota()
    COUNT:     int = iota()

    def __repr__(self) -> str:
        result: str =  "Expr(\n"
        result     += f"    {self.VALUE=},\n"
        result     += f"    {self.INFIX=},\n"
        result     +=  ")"
        return result

class InfixOp:
    UNDEFINED: int = iota(0)
    ADDITION:  int = iota()
    COUNT:     int = iota()

    def __repr__(self) -> str:
        result: str =  "InfixOp(\n"
        result     += f"    {self.ADDITION=},\n"
        result     +=  ")"
        return result

class Type:
    UNDEFINED: int = iota(0)
    I64:       int = iota()
    COUNT:     int = iota()

class TypeExpr:
    kind:    int
    alias:   str
    context: Any | None

    def __init__(self, 
                 kind: int = Type.UNDEFINED, 
                 alias: str = "", 
                 context: Any | None = None) -> None:
        self.kind = kind
        self.alias = alias
        self.context = context

    def __repr__(self) -> str:
        result: str =  "TypeExpr(\n"
        result     += f"    {f'{self.kind=!r}'.replace('\n','\n    ')},\n"
        result     += f"    {self.alias=},\n"
        result     += f"    {self.context=},\n"
        result     +=  ")"
        return result

class ValueExpr:
    type:  TypeExpr | None
    span:  Span
    value: Any | None

    def __init__(self, 
                 type: TypeExpr | None = None, 
                 span: Span = Span(), 
                 value: Any | None = None) -> None:
        self.type  = type
        self.span  = span
        self.value = value

    def __repr__(self) -> str:
        result: str =  "ValueExpr(\n"
        result     += f"    {f'{self.span=!r}'.replace('\n','\n    ')},\n"
        result     += f"    {f'{self.type=!r}'.replace('\n','\n    ')},\n"
        result     +=  ")"
        return result

class Expression:
    kind: int
    span: Span
    expr: Any | None

    def __init__(self, 
                 kind: int = ExprKind.UNDEFINED, 
                 span: Span = Span(), 
                 expr: Any | None = None) -> None:
        self.kind = kind
        self.span = span
        self.expr = expr

    def __repr__(self) -> str:
        result: str =  "Expression(\n"
        result     += f"    {self.kind=},\n"
        result     += f"    {f'{self.span=!r}'.replace('\n','\n    ')},\n"
        result     += f"    {f'{self.expr=!r}'.replace('\n','\n    ')},\n"
        result     +=  ")"
        return result

def isdigit(char: str) -> bool:
    return (ord(char) >= ord('0') and ord(char) <= ord('9'))

class Lexer:
    def __init__(self, path: str) -> None:
        self.path:       str        = path
        self.text:       str        = ""
        self.position:   tuple[int] = Position()
        self.curr_char:  str | None = None
        self.next_char:  str | None = None
        self.char_index: int        = 0

        with open(self.path, "r") as file:
            self.text = file.read()

        self.next()
        self.next()
        
    def next(self) -> None:
        self.curr_char = self.next_char
        self.next_char = self.text[self.char_index] \
            if self.char_index < len(self.text) else None
        self.char_index += 1

    def advance(self) -> None:
        if self.curr_char == '\n':
            self.position.line += 1
            self.position.column = 0
            self.position.line_off = self.position.offset + 1
        self.position.column += 1
        self.position.offset += 1
        self.next()

    def tokenise(self) -> list[Token]:
        tokens: list[Token] = []
        token: Token = self.next_token()
        while token:
            tokens.append(token)
            token = self.next_token()
        return tokens

    def next_token(self) -> Token | None:
        while self.curr_char:
            match (self.curr_char, self.next_char):
                case (' ' | '\n' | '\r' | '\t', _):
                    self.advance()
                case ('/', '/'):
                    while self.curr_char and self.curr_char != '\n':
                        self.advance()
                case ('/', '*'):
                    while (self.curr_char and self.next_char and
                           self.curr_char != '*' and self.next_char != '/'):
                        self.advance()
                    self.advance()
                    self.advance()
                case _:
                    break
        
        if not self.curr_char:
            return None
        
        position: tuple[int] = deepcopy(self.position)
        match (self.curr_char, self.next_char):
            case ('+', _):
                self.advance()
                return Token(TokenKind.PLUS, 
                             Span(position, deepcopy(self.position)))
            case (';', _):
                self.advance()
                return Token(TokenKind.SEMICOLON, 
                             Span(position, deepcopy(self.position)))
            case _:
                while isdigit(self.curr_char):
                    self.advance()
                if position != self.position:
                    return Token(TokenKind.DIGITS, 
                                 Span(position, deepcopy(self.position)))
        print(f"{self.position.line}:{self.position.column}:ERROR: " +
              f"Encountered an invalid token ({ord(self.curr_char)}) " +
              f"`{self.curr_char}` while lexing!")

class Parser:
    def __init__(self, lexer: Lexer) -> None:
        self.lexer: Lexer = lexer
        self.last_token: Token | None = None
        self.curr_token: Token | None = None
        self.next_token: Token | None = None
        self.context: Any | None = None

        self.next()
        self.next()

    def next(self) -> None:
        self.last_token = self.curr_token
        self.curr_token = self.next_token
        self.next_token = self.lexer.next_token()

    def span_text(self, span: Span) -> str:
        return self.lexer.text[span.start.offset:span.end.offset]

    def type(self) -> TypeExpr | None:
        pass

    def value(self) -> ValueExpr | None:
        lexer: Lexer = deepcopy(self.lexer)
        self.next()
        match (self.last_token.kind, self.next_token.kind):
            case (TokenKind.Digits, _):
                return ValueExpr()

    def expression(self) -> Expression | None:
        start_position: Position = self.curr_token.span.start

        expr: Any | None = None
        match self.curr_token.kind:
            case TokenKind.Plus:
                pass
            case TokenKind.Digits:
                expr = self.value()
        print(f"{self.curr_token.span.start.line}:" +
              f"{self.curr_token.span.start.column}:" +
              f"ERROR: Encountered an invalid token " +
              f"({self.curr_token.kind}) " +
              f"`{self.span_text(self.curr_token.span)}` " +
              "while parsing!")

    def parse(self) -> None:
        expressions: list[Expression] = []
        while self.curr_token:
            expr: Expression | None = self.expression()
            if isinstance(expr, Expression):
                expressions.append(self.expression())
            self.next()

def main() -> None:
    print(TokenKind())
    #print(ExprKind())
    #print(InfixOp())

    l = Lexer("../test.mcs")
    p = Parser(deepcopy(l))
    t = l.tokenise()
    print([_.kind for _ in t])
    print(f"'{l.text}'")

if __name__ == "__main__":
    main()
