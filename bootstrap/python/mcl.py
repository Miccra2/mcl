#!/usr/bin/env python3

__all__: list[str] = [
    "Token",
]

from dataclasses import dataclass, field
from copy import deepcopy
from typing import Any
import sys

###############################################################################
# HELPER FUNCTIONS

def default(cls: Any) -> Any:
    return field(default_factory=cls)

IOTA_COUNTER: int = 0
def iota(value: int | None = None) -> int:
    global IOTA_COUNTER
    if isinstance(value, int):
        IOTA_COUNTER = value
    result: int = IOTA_COUNTER
    IOTA_COUNTER += 1
    return result

###############################################################################
# DATACLASSES

@dataclass
class TokenKind:
    UNDEFINED: int = iota(0)
    DIGITS:    int = iota()
    PLUS:      int = iota()
    SEMICOLON: int = iota()
    COUNT:     int = iota()

@dataclass
class Position:
    line:     int = 1
    column:   int = 1
    line_off: int = 0
    offset:   int = 0

@dataclass
class Span:
    start: Position = default(Position)
    end:   Position = default(Position)

@dataclass
class Token:
    kind: TokenKind | int = TokenKind.UNDEFINED
    span: Span            = default(Span)

@dataclass
class ExprKind:
    UNDEFINED:  int = iota(0)
    VALUE_EXPR: int = iota()
    INFIX_EXPR: int = iota()
    COUNT:      int = iota()

@dataclass
class Expression:
    kind: int        = ExprKind.UNDEFINED
    expr: Any | None = None

@dataclass
class Type:
    UNDEFINED: int = iota(0)
    I64:       int = iota()
    COUNT:     int = iota()

@dataclass
class ValueExpr:
    type:  int        = Type.UNDEFINED
    span:  Span       = default(Span)
    value: Any | None = None

@dataclass
class InfixOp:
    UNDEFINED: int = iota(0)
    PLUS:      int = iota()
    COUNT:     int = iota()

@dataclass
class InfixExpr:
    op:    InfixOp    = InfixOp.UNDEFINED
    span:  Span       = default(Span)
    left:  Expression = default(Expression)
    right: Expression = default(Expression)

###############################################################################
# FUNCTIONS

def get_pos(pos: Position) -> str:
    return f"{pos.line}:{pos.column}:"

def get_priority(token_kind: int) -> tuple[int, int]:
    match token_kind:
        case TokenKind.PLUS:
            return (iota(1), iota())
        case _:
            return (0, 0)

def isdigit(char: str) -> bool:
    return ord(char) >= ord('0') and ord(char) <=  ord('9')

###############################################################################
# LEXER

class Lexer:
    def __init__(self, path: str) -> None:
        self.path: str = path
        self.text: str = ""
        self.position: Position = Position()
        self.char_index: int = 0
        self.curr_char: str | None = None
        self.next_char: str | None = None

        try:
            with open(self.path, "r") as file:
                self.text = file.read()
        except:
            print("ERROR: While lexing, could not open file path " +
                 f"`{self.path}`, please provide a valid file path!")

        # populate self.curr_char and self.next_char with data
        self.next()
        self.next()

    def __next__(self) -> Token | None:
        return self.next_token()

    def next(self) -> None:
        self.curr_char = self.next_char
        self.next_char = self.text[self.char_index] \
                if self.char_index < len(self.text) else None
        self.char_index += 1

    def advance(self, newline: bool = False) -> None:
        if newline:
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
                    self.advance(self.curr_char == '\n')
                case ('/', '/'):
                    while self.curr_char and self.curr_char != '\n':
                        self.advance(self.curr_char == '\n')
                case ('/', '*'):
                    while (self.curr_char and self.curr_char != '*' and 
                           self.next_char and self.next_char != '/'):
                        self.advance()
                    self.advance()
                    self.advance()
                case _:
                    break
        
        if not self.curr_char:
            return None
        
        position: Position = deepcopy(self.position)
        kind: int = TokenKind.UNDEFINED
        match (self.curr_char, self.next_char):
            case ('+', _):
                kind = TokenKind.PLUS 
            case (';', _):
                kind = TokenKind.SEMICOLON
            case _:
                while self.curr_char and isdigit(self.next_char):
                    self.advance()
                if position != self.position:
                    kind = TokenKind.DIGITS 
        
        self.advance()
        print(Token(kind, Span(Position, self.position)))
        return Token(kind, Span(position, deepcopy(self.position)))

        if not isinstance(token, Token):
            print(f"{get_pos(position)}ERROR: While lexing, encountered an " +
                  f"invalid token ({ord(self.curr_char)}) `{self.curr_char}`!")
        
        self.advance()
        return token

###############################################################################
# PARSER

class Parser:
    def __init__(self, lexer: Lexer) -> None:
        self.lexer:      Lexer        = lexer
        self.last_token: Token | None = None
        self.curr_token: Token | None = None
        self.next_token: Token | None = None

        self.next()

    def next(self) -> None:
        self.last_token = self.curr_token
        self.curr_token = self.lexer.next_token()

    def span_text(self, span: Span) -> str:
        return self.lexer.text[span.start.offset:span.end.offset]

    def value_expr(self) -> ValueExpr | None:
        start_position: Position = self.curr_token.span.start
        expr: Any | None = None
        self.next()
        match (self.last_token.kind, self.curr_token.kind \
                if isinstance(self.curr_token, Token) else None):
            case (TokenKind.DIGITS, _):
                expr = ValueExpr(Type.I64, self.last_token.span, 
                                 int(self.span_text(self.last_token.span), 10))
            case _:
                print(f"{get_pos(self.curr_token.span.start)}ERROR: " +
                      f"While parsing, encountered an invalid token " +
                      f"({self.curr_token.kind}) " +
                      f"`{self.span_text(self.curr_token.span)}`!")
        
        return Expression(
            ExprKind.VALUE_EXPR,
            expr
        )

    def infix_expr(self, 
                   start_pos: Position, 
                   expr: Any | None, 
                   op: InfixOp) -> Expression | None:
        r_priority: int = get_priority(self.curr_token.kind)[1]
        self.next()
        right: Any | None = self.expression(r_priority)
        
        return Expression(
            ExprKind.INFIX_EXPR,
            InfixExpr(
                op,
                Span(start_pos, self.last_token.span.end),
                expr,
                right
            )
        )

    def expression(self, priority: int) -> Expression | None:
        start_pos: Position = self.curr_token.span.start

        expr: Any | None = None
        print(self.curr_token.kind)
        match self.curr_token.kind:
            case TokenKind.DIGITS:
                expr = self.value_expr()
            case _:
                print(f"{get_pos(start_pos)}ERROR: While parsing, " +
                      f"encountered an invalid token " +
                      f"({self.curr_token.kind}) " +
                      f"`{self.span_text(self.curr_token.span)}`!")
                return None

        while (self.curr_token and 
               get_priority(self.curr_token.kind)[0] > priority):
            match self.curr_token.kind:
                case TokenKind.PLUS:
                    expr = self.infix_expr(start_pos, expr, InfixOp.PLUS)
                case _:
                    return expr

        return expr

    def parse(self) -> None:
        while self.curr_token:
            print(self.curr_token)
            print(self.next_token)
            if not self.expression(0):
                print("exit parsing")
                break
    
def main() -> None:
    lexer: Lexer = Lexer("../test.mcs")
    parser: Parser = Parser(lexer)
    parser.parse()

if __name__ == "__main__":
    main()
