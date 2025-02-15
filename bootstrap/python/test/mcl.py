#!/usr/bin/env python3
from dataclasses import dataclass, field
from copy import deepcopy
import sys

__all__: list[str] = [
    "TokenKind",
    "Token",
    "Position",
    "Lexer",
    "Parser",
    "Compiler",
]

###############################################################################
# constructor functions

IOTA_COUNTER: int = 0
def iota(reset: bool = False) -> int:
    global IOTA_COUNTER
    if reset:
        IOTA_COUNTER = 0
    result: int = IOTA_COUNTER
    IOTA_COUNTER += 1
    return result

def default(cls: any) -> any:
    return field(default_factory=cls())

###############################################################################
# data structures

@dataclass
class Position:
    line:     int = 1
    column:   int = 1
    line_off: int = 0
    offset:   int = 0

@dataclass
class TokenKind:
    undefined:   int = iota(True)
    
    identifier:  int = iota()
    digits:      int = iota()
    number:      int = iota()

    plus:        int = iota()

    count:       int = iota()

@dataclass
class Token:
    kind:  TokenKind | int = TokenKind.undefined
    start: Position        = default(Position)
    end:   Position        = default(Position)

@dataclass
class Type:
    undefined: int = iota(True)
    
    # integer types
    i8:    int = iota()
    i16:   int = iota()
    i32:   int = iota()
    i64:   int = iota()
    #i128:  int = iota()
    #isize: int = iota()
    u8:    int = iota()
    u16:   int = iota()
    u32:   int = iota()
    u64:   int = iota()
    #u128:  int = iota()
    #usize: int = iota()

    count: int = iota()

@dataclass
class Op:
    undefined: int = iota(True)

    # arithmetic instructions
    add: int = iota()
    sub: int = iota()
    mul: int = iota()
    div: int = iota()
    mod: int = iota()

    count: int = iota()

@dataclass
class Value:
    type:    Type | int = Type.undefined
    content: any | None = None

@dataclass
class Expr:
    op:     Op        = Op.undefined
    values: list[any] = []

###############################################################################
# LOGGER

class Logger:
    def usage(printmsg: bool = True) -> str | None:
        result: str = "Usage: python3 mcl.py [file]\n"
        result     += "    file - path to source file, with `.mcs` extension\n"
        if not printmsg:
            return result
        print(result, end="")

    def error(msg:       str, 
              token:     Token | None = None,
              showsuage: bool = False,
              printmsg:  bool = True,
              errorcode: int  = 1) -> str | None:
        result: str = "ERROR:"
        if isinstance(token, Token):
            result += "{token.start.line}:{token.start.column}:"
        result += f" {msg}"
        if showusage:
            result += self.usage(False)
        if not printmsg:
            return result
        print(result, end="")
        sys.exit(errorcode)

    def waring(msg:       str,
               token:     Token | None = None,
               showusage: bool = False,
               printmsg:  bool = True,
               fatal:     bool = False) -> str | None:
        result: str = "WARNING:"
        if isinstance(token, Token):
            result += "{token.start.line}:{token.end.column}:"
        result += f" {msg}"
        if showusage:
            result += self.usage(False)
        if not printmsg:
            return result
        print(result, end="")
        if fatal:
            sys.exit(1)

    def info(msg:       str,
             token:     Token | None = None,
             showusage: bool = False,
             printmsg:  bool = True,
             fatal:     bool = False) -> str | None:
        result: str = "INFO:"
        if isinstance(token, Token):
            result += "{token.start.line}:{token.start.column}:"
        result += f" {msg}"
        if showusage:
            result += self.usage(False)
        if not printmsg:
            return result
        print(result, end="")
        if fatal:
            sys.exit(1)

log: Logger = Logger()

###############################################################################
# LEXER

class Lexer:
    def __init__(self, path: str) -> None:
        # public
        self.path: str = path
        self.text: str = ""
        self.pos: Position = Position()

        # private
        self.char_index: int = 0
        self.curr_char: str | None = None
        self.next_char: str | None = None

        try:
            with open(self.path, "r") as file:
                self.text = file.read()
        except:
            log.error(f"Could not open file `{self.path}`, " + 
                      "please provide a valid file path!")

        # initialize self.curr_char and self.next_char
        self.next()
        self.next()

    def __next__(self) -> Token | None:
        return self.next_token()

    def tokenise(self) -> list[Token]:
        tokens: list[Token] = []
        token: Token = self.next_token()

        while token:
            tokens.append(token)
            token = self.next_token()

        return tokens

    def next(self) -> None:
        self.curr_char = self.next_char
        self.next_char = self.text[self.char_index] \
                if self.char_index < len(self.text) else None
        self.char_index += 1

    def advance(self) -> None:
        # update self.pos if on newline character
        if self.curr_char == '\n':
            self.pos.line += 1
            self.pos.column = 0
            self.pos.line_off = self.pos.offset + 1

        self.pos.column += 1
        self.pos.offset += 1

        self.next()

    def isalpha(self) -> bool:
        char: int = ord(self.curr_char)
        return ((char >= ord('A') and char <= ord('Z')) or 
                (char >= ord('a') and char <= ord('z')))

    def isdigit(self) -> bool:
        char: int = ord(self.curr_char)
        return char >= ord('0') and char <= ord('9')

    def isnumber(self) -> bool:
        return self.curr_char == '.' or self.isdigit()

    def isidentifierstart(self) -> bool:
        return self.curr_char == '_' or self.isalpha()

    def isidentifier(self) -> bool:
        return self.isidentifierstart() or self.isdigit()

    def next_token(self) -> Token | None:
        # skip whitespaces and comments
        while self.curr_char:
            if (self.curr_char == ' ' or self.curr_char == '\n' or 
                self.curr_char == '\r' or self.curr_char == '\t'):
                # skip whitespaces aka: ` `, `\n`, `\r` and `\t`
                self.advance()
            elif self.curr_char == '/' and self.next_char == '/':
                # skip line comment aka: `//`
                while self.curr_char and self.curr_char != '\n':
                    self.advance()
            elif self.curr_char == '/' and self.next_char == '*':
                # skip block comment aka: `/*` and `*/`
                while (self.curr_char and self.next_char and 
                       self.curr_char != '*' and self.next_char != '/'):
                    self.advance()
                # skip end of block comment `*/`
                self.advance()
                self.advance()
            else:
                # no whitespace or comment found (break out of loop)
                break
        # return None if end of self.text is reached
        if not self.curr_char:
            return None
        # tokenise
        pos: Position = deepcopy(self.pos)
        if self.curr_char == '+':
            self.advance()
            return Token(TokenKind.plus, pos, deepcopy(self.pos))
        elif self.isdigit():
            isfloat: bool = False
            while self.curr_char and self.isnumber():
                if self.curr_char == '.':
                    isfloat = True
                self.advance()
            if isfloat:
                return Token(TokenKind.number, pos, deepcopy(self.pos))
            return Token(TokenKind.digits, pos, deepcopy(self.pos))
        log.error("Encountered an invalid token " +
                  f"`{self.curr_char}` ({ord(self.curr_char)}) while lexing!")

###############################################################################
# PARSER

assert Op.count == 5, "Ops not implemented!"
BASE_OPS: list[list[list[int] | None] | None] = [
    None,               # undefined
    [
        [Type.i8,  Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.i16, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.i32, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.i64, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u8,  Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u16, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u32, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u64, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
    ],
    [
        [Type.i8,  Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.i16, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.i32, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.i64, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u8,  Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u16, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u32, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u64, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
    ],
    [
        [Type.i8,  Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.i16, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.i32, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.i64, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u8,  Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u16, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u32, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u64, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
    ],
    [
        [Type.i8,  Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.i16, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.i32, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.i64, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u8,  Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u16, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u32, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u64, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
    ],
    [
        [Type.i8,  Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.i16, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.i32, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.i64, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u8,  Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u16, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u32, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
        [Type.u64, Type.i8, Type.i16, Type.i32, Type.i64, 
                   Type.u8, Type.u16, Type.u32, Type.u64,],
    ],

    None,               # count
]

class Parser:
    def __init__(self, lexer: Lexer) -> None:
        # public
        self.lexer: Lexer = lexer
        self.last_token: Token | None = None
        self.curr_token: Token | None = None

        # initialize self.token
        self.next()

    def next(self) -> Token | None:
        self.last_token = self.curr_token
        self.curr_token = self.lexer.next_token()

    def handle_plus(self) -> Expr:
        pass

    def parse(self, operator_value_combinations: list[tuple[int] | None]) -> None:
        expr: Expr = Expr()
        while self.curr_token:
            if self.curr_token.kind == TokenKind.plus:
                pass
            self.next()

###############################################################################
# INTERPRETER

class Interpreter:
    pass

###############################################################################
# COMPILER

class Compiler:
    pass

###############################################################################
# TRANSPILER

class Transpiler:
    pass

def main() -> None:
    if len(sys.argv) < 2:
        log.error("To few input arguments given, " +
                  "please provide more input arguments", 
                  showusage=True)
    lexer: Lexer = Lexer(sys.argv[1])
    tokens: list[Token] = lexer.tokenise()
    for token in tokens:
        print(token)

if __name__ == "__main__":
    import os
    print(os.getpid())
    input()
    main()
