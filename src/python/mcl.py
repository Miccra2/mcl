#!/usr/bin/env python3
from dataclasses import dataclass, field
from copy import deepcopy
import sys

__all__: list[str] = [
    # token
    "Position", "Span", "Token", "TokenKind",
    # ast
    "Expression", "ExprKind", "InfixExpr", "InfixOp", "ValueExpr", "Type",
    # classes
    "Lexer",
    "Parser",
    "Interpreter",
    "Compiler",
]

###############################################################################
# functions for dataclass definitions

IOTA_COUNTER: int = 0
def iota(reset: int | None = None) -> int:
    global IOTA_COUNTER
    if isinstance(reset, int):
        IOTA_COUNTER = reset
    result: int = IOTA_COUNTER
    IOTA_COUNTER += 1
    return result

def default(cls: any) -> any:
    return field(default_factory=cls)

def usage() -> None:
    print("Usage: python3 mcl.py [file]")
    print("    file - path to source file")

###############################################################################
# dataclasses

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
class TokenKind:
    undefined: int = iota(0)
    integer: int = iota()
    plus: int = iota()
    semicolon: int = iota()
    count: int = iota()

@dataclass
class Token:
    kind: int = TokenKind.undefined
    span: Span = default(Span)

@dataclass
class ExprKind:
    undefined: int = iota(0)
    value: int = iota()
    infix: int = iota()
    count: int = iota()

@dataclass
class Expression:
    kind: int = ExprKind.undefined
    span: Span = default(Span)
    expr: None = None

@dataclass
class Type:
    undefined: int = iota(0)

    # integer types
    i8: int = iota()
    u8: int = iota()
    i16: int = iota()
    u16: int = iota()
    i32: int = iota()
    u32: int = iota()
    i64: int = iota()
    u64: int = iota()
    i128: int = iota()
    u128: int = iota()
    isize: int = iota()
    usize: int = iota()
    
    count: int = iota()

@dataclass
class ValueExpr:
    type: int = Type.undefined
    value: None = None

@dataclass
class InfixOp:
    undefined: int = iota(0)
    plus: int = iota()
    count: int = iota()

@dataclass
class InfixExpr:
    op: int = InfixOp.undefined
    lvalue: Expression = default(Expression)
    rvalue: Expression = default(Expression)

###############################################################################
# classes

class Lexer:
    def __init__(self, path: str) -> None:
        self.path: str = path
        self.text: str = ""
        self.position: Position = Position()
        self.curr_char: str | None = None
        self.next_char: str | None = None
        self.char_index: int = 0

        try:
            with open(self.path, "r") as file:
                self.text = file.read()
        except FileNotFoundError:
            print("ERROR: while lexing: Could not find file " +
                  f"`{self.path}`, please provide a valid file path!")
            sys.exit(1)

        self.next()
        self.next()

    def next(self) -> None:
        self.curr_char = self.next_char
        if self.char_index < len(self.text):
            self.next_char = self.text[self.char_index]
        else:
            self.next_char = None
        self.char_index += 1

        print(f"curr={self.curr_char}, next={self.next_char}")

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
        token: Token | None = self.next_token()
        
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

        position: Position = deepcopy(self.position)
        kind: int = TokenKind.undefined
        match (self.curr_char, self.next_char):
            case ('+', _):
                kind = TokenKind.plus
            case (';', _):
                kind = TokenKind.semicolon
            case _:
                while self.curr_char and self.curr_char.isdigit():
                    self.advance()
                if position != self.position:
                    return Token(TokenKind.integer, 
                                 Span(position, deepcopy(self.position)))
                print(f"ERROR:{position.line}:{position.column}: " +
                      "while lexing: Invalid token " +
                      f"(curr=`{self.curr_char}`, next=`{self.next_char}`)!")

        self.advance()
        return Token(kind, Span(position, deepcopy(self.position)))

class Parser:
    def __init__(self, lexer: Lexer) -> None:
        self.lexer: Lexer = lexer
        self.last_token: Token | None = None
        self.curr_token: Token | None = None

        self.next()

    def next(self) -> None:
        self.last_token = self.curr_token
        self.curr_token = self.lexer.next_token()

    def expect(self, kind: int) -> None:
        if self.curr_token.kind == kind:
            self.next()

    def get_priority(self, kind: int) -> (int, int):
        match kind:
            case TokenKind.plus:
                return (2, 1)
            case _:
                return (0, 0)

    def value_expr(self, start_position: Position) -> Expression:
        kind: int = Type.undefined
        value: None = None
        self.next()

        match self.last_token.kind:
            case TokenKind.integer:
                kind = Type.i64
                value = int(self.lexer.text[start_position.offset: \
                                            self.last_token.span.end.offset], 
                            10)

        return Expression(
            ExprKind.value, 
            Span(start_position, self.last_token.span.end),
            ValueExpr(kind, value)
        )

    def infix_expr(self, 
                   start_position: Position, 
                   op: int, 
                   left: Expression | None) -> Expression:
        right_priority: int = self.get_priority(self.curr_token.kind)[1]
        self.next()
        right: Expression | None = self.expression(right_priority)
        
        return Expression(
            ExprKind.infix, 
            Span(start_position, self.last_token.span.end), 
            InfixExpr(op, left, right)
        )

    def expression(self, priority: int = 0) -> Expression:
        start_position: Position = deepcopy(self.curr_token.span.start)
        expr: Expression | None = None

        match self.curr_token.kind:
            case TokenKind.integer:
                expr = self.value_expr(start_position)

        while self.curr_token and \
                self.get_priority(self.curr_token.kind)[0] > priority:
            match self.curr_token.kind:
                case TokenKind.plus:
                    expr = self.infix_expr(start_position, InfixOp.plus, expr)

        return expr

class Interpreter:
    def __init__(self, expr: Expression) -> None:
        self.expr = expr

    def is_integer_type(self, _type: int) -> bool:
        match _type:
            case (Type.i8 | Type.u8 | Type.i16 | Type.u16 | 
                    Type.i32 | Type.u32 | Type.i64 | Type.u64 | 
                    Type.i128 | Type.u128 | Type.isize | Type.usize):
                return True
            case _:
                return False

    def eval_infix_expr(self, expr: InfixExpr) -> ValueExpr:
        left: self.eval_expression(expr.lvalue)
        right: self.eval_expression(expr.rvalue)

        match expr.op:
            case InfixOp.plus:
                if (self.is_integer_type(left.type) and 
                    self.is_integer_type(right.type)):
                    return ValueExpr(left.type, left.value + right.value)

    def eval_expression(self, expr: Expression) -> ValueExpr:
        match expr.kind:
            case ExprKind.value:
                return expr.expr
            case ExprKind.infix:
                return self.eval_infix_expr(expr.expr)

class Compiler:
    def __init__(self) -> None:
        pass

def main() -> None:
    if len(sys.argv) < 2:
        print("ERROR: To few input arguments given!")
        usage()

    lexer: Lexer = Lexer(sys.argv[1])
    parser: Parser = Parser(lexer)

    print(ExprKind())
    print(InfixOp())
    print(Type())

    print()
    print(parser.expression())

    print()
    print(f"'{lexer.text}'")
    print(len(lexer.text))
    
if __name__ == "__main__":
    main()
