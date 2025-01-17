#!/usr/bin/env python3
import sys, subprocess
from dataclasses import dataclass, field
from copy import deepcopy

__all__: list[str] = [
    "TokenKind",
    "Token",
    "Position",
    "Lexer",
    "Parser",
    "Interpreter",
    "Compiler",
]

IOTA_COUNTER: int = 0
def iota(reset: bool = False) -> int:
    global IOTA_COUNTER
    if reset:
        IOTA_COUNTER = 0
    result: int = IOTA_COUNTER
    IOTA_COUNTER += 1
    return result

def defualt(cls: any) -> any:
    return field(default_factory=cls())

@dataclass
class TokenKind:
    undefined: int = iota(True)
    
    digits: int = iota()
    number: int = iota()
    label: int = iota()

    plus: int = iota()

    count: int = iota()

@dataclass
class Ops:
    udefined: int = iota(True)
    
    add: int = iota()

    count: int = iota()

@dataclass
class Type:
    udefined: int = iota(True)

    i8: int = iota()
    i16: int = iota()
    i32: int = iota()
    i64: int = iota()
    i128: int = iota()
    isize: int = iota()
    u8: int = iota()
    u16: int = iota()
    u32: int = iota()
    u64: int = iota()
    u128: int = iota()
    usize: int = iota()

    f16: int = iota()
    f32: int = iota()
    f64: int = iota()

    boolean: int = iota()
    pointer: int = iota()
    array: int = iota()

    char: int = iota()
    string: int = iota()

    alias: int = iota()

    count: int = iota()

@dataclass
class Position:
    line:     int = 1
    column:   int = 1
    line_off: int = 0
    offset:   int = 0

@dataclass
class Token:
    kind:  TokenKind | int = TokenKind.undefined
    start: Position        = defualt(Position)
    end:   Position        = defualt(Position)

@dataclass
class Ast:
    def add(expr0: any, expr1: any) -> tuple[int]:
        return (Ops.add, expr0, expr1)

class Logger:
    def __init__(self) -> None:
        pass

    def print(self, *args: list[any]) -> None:
        print(*args, end="")

    def usage(self, printmsg: bool = True) -> str | None:
        result: str = "Usage: python3 mcs.py [file]\n"
        result +=     "    file - path to source file, with `.mcs` extension\n"
        if not printmsg:
            return result
        self.print(result)

    def error(self, 
              msg: str, 
              pos: Position | None = None, 
              showusage: bool = False, 
              printmsg: bool = True, 
              errorcode: int = 1) -> str | None:
        result: str = ""
        if showusage:
            result += self.usage(False)
        result += "ERROR:"
        if isinstance(pos, Position):
            result += f"l{pos.line}:c{pos.column}:"
        result += f" {msg}"
        if not printmsg:
            return result
        self.print(result)
        sys.exit(errorcode)

    def waring(self, 
               msg: str, 
               pos: Position | None = None, 
               showusage: bool = False, 
               printmsg: bool = True, 
               fatal: bool = True) -> str | None:
        result: str = ""
        if showusage:
            result += self.usage(False)
        result += "ERROR:"
        if isinstance(pos, Position):
            result += f"l{pos.line}:c{pos.column}:"
        result += f" {msg}"
        if not printmsg:
            return result
        self.print(result)
        if fatal:
            sys.exit(1)

    def info(self, 
             msg: str, 
             pos: Position | None = None, 
             showusage: bool = False, 
             printmsg: bool = True, 
             fatal: bool = False) -> str | None:
        result: str = ""
        if showusage:
            result += self.usage(False)
        result += "INFO:"
        if isinstance(pos, Position):
            result += f"l{pos.line}:c{pos.column}:"
        result += f" {msg}"
        if not printmsg:
            return result
        self.print(result)
        if fatal:
            sys.exit(1)

log: Logger = Logger()

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
                         "please provide a valid file path!\n")
        log.info(f"Opened file `{self.path}`!\n")

        # populate self.curr_char and self.next_char
        self.next()
        self.next()

    def __next__(self) -> Token | None:
        return self.next_token()

    def next_token(self) -> Token | None:
        # skip whitespaces and comments
        while self.isreachabletext():
            # skip whitespaces aka: ` `, `\n`, `\r` and `\t`
            if self.curr_char == ' ' or self.curr_char == '\n' or \
                    self.curr_char == '\r' or self.curr_char == '\t':
                self.advance()
            # skip line comment aka: `//`
            elif self.curr_char == '/' and self.next_char == '/':
                while self.isreachabletext() and \
                        self.curr_char != '\n':
                    self.advance()
            # skip block comment aka: `/*` and `*/`
            elif self.curr_char == '/' and self.enxt_char == '*':
                while self.isreachabletext() and \
                        self.curr_char != '*' and self.next_char != '/':
                    self.advance()
                # skip end of block comment `*/` to not get them as tokens
                self.advance()
                self.advance()
            # break out of loop (no whitespaces or comment found)
            else:
                break
        # return None if end of self.text reached
        if not isinstance(self.curr_char, str):
            return None
        # tokenise
        pos: Position = deepcopy(self.pos)
        if self.curr_char == '+':
            self.advance()
            return Token(TokenKind.plus, pos, deepcopy(self.pos))
        elif self.isdigit():
            isfloat: bool = False
            while self.isreachabletext() and self.isnumber():
                if self.curr_char == '.':
                    isfloat = True
                self.advance()
            if isfloat:
                return Token(TokenKind.number, pos, deepcopy(self.pos))
            return Token(TokenKind.digits, pos, deepcopy(self.pos))
        elif self.islabelstart():
            while self.isreachabletext() and self.islabel():
                self.advance()
            return Token(TokenKind.label, pos, deepcopy(self.pos))
        log.error("Encountered an invalid token " +
                  f"`{self.curr_char}` ({ord(self.curr_char)}) " +
                  "while lexing!\n", pos)

    def isreachabletext(self) -> bool:
        return self.char_index < len(self.text) - 1

    def isdigit(self) -> bool:
        char: int = ord(self.curr_char)
        return char >= ord('0') and char <= ord('9')
    
    def isnumber(self) -> bool:
        return self.curr_char == '.' or self.isdigit()
    
    def isalpha(self) -> bool:
        char: int = ord(self.curr_char)
        return (char >= ord('A') and char <= ord('Z')) or \
                (char >= ord('a') and char <= ord('z'))
    
    def islabelstart(self) -> bool:
        return self.curr_char == '_' or self.isalpha()

    def islabel(self) -> bool:
        return self.islabelstart() or self.isdigit()

    def next(self) -> None:
        self.curr_char = self.next_char
        self.next_char = self.text[self.char_index] \
                if self.isreachabletext() \
                else None
        self.char_index += 1

    def advance(self) -> None:
        if self.curr_char == '\n':
            self.pos.line += 1
            self.pos.column = 0
            self.pos.line_off = self.pos.offset + 1
        self.pos.column += 1
        self.pos.offset += 1
        self.next()

    def tokenise(self) -> list[Token]:
        log.info(f"Lexing file `{self.path}`!\n")
        tokens: list[Token] = []
        token: Token | None = self.next_token()
        while self.isreachabletext():
            log.info(f"token: {token}\n")
            tokens.append(token)
            token = self.next_token()
        return tokens

class Parser:
    def __init__(self) -> None:
        assert False, "Not implemented!"

class Interpreter:
    def __init__(self) -> None:
        assert False, "Not implemented!"

class Compiler:
    def __init__(self) -> None:
        assert False, "Not implemented!"

def main() -> None:
    if len(sys.argv) < 2:
        log.error("To few input arguments given, " + 
                     "please provide more input arguments!\n")
    lexer: Lexer = Lexer(sys.argv[1])
    tokens = lexer.tokenise()
    print()
    print(TokenKind())
    for token in lexer.tokenise():
        print(token)
    print(lexer.char_index)

if __name__ == "__main__":
    main()
