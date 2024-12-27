#!/usr/bin/env python3.12
"""mcl (my coding language) will be a selfhosted language bootstrap with python"""
from dataclasses import dataclass, field, asdict, astuple
from copy import deepcopy
import subprocess
import sys

__all__: list[str] = [
    "Position",
    "TokenKind",
    "Token",
    "Lexer",
    #"Parser",          # not implemented
    #"Interpreter",     # not implemented
    #"Compiler",        # not implemented
    #"Transpiler"       # not implemented
]

IOTA_COUNTER: int = 0
def iota(reset: bool = False) -> int:
    """enumerate objects not the pythonic way but more go like (i never used go)"""
    global IOTA_COUNTER
    if reset:
        IOTA_COUNTER = 0
    result: int = IOTA_COUNTER
    IOTA_COUNTER += 1
    return result

def usage() -> None:
    print("Usage: python mcl.py [file]")
    print("    file - path to source file, with `.mcs` extension!")

def default(cls: any) -> any:
    return field(default_factory=cls)

@dataclass
class TokenKind:
    none: int = iota(True)

    plus: int = iota()

    at: int = iota()
    colon: int = iota()
    semicolon: int = iota()
    
    l_paren: int = iota()
    r_paren: int = iota()
    l_brace: int = iota()
    r_brace: int = iota()

    integer: int = iota()
    label: int = iota()

@dataclass
class Position:
    line:     int = 1
    column:   int = 1
    line_off: int = 0
    offset:   int = 0

@dataclass
class Token:
    kind:  TokenKind = TokenKind.none
    start: Position  = default(Position())
    end:   Position  = default(Position())

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
    
        # populate self.text
        try:
            with open(path, "r") as file:
                self.text = file.read()
        except FileNotFoundError:
            usage()
            assert False, f"ERROR: Could not find file `{path}`!"
        except IOError:
            usage()
            assert False, f"ERROR: Could not read file `{path}`!"

        # populate self.curr_char and self.next_char with data
        self.next()
        self.next()

    def __next__(self) -> Token:
        return self.next_token()

    def next(self) -> None:
        """update self.curr_char, self.next_char and self.char_index"""
        self.curr_char = self.next_char
        
        if self.char_index < len(self.text):
            self.next_char = self.text[self.char_index]
        else:
            self.next_char = None
        
        self.char_index += 1

    def advance(self) -> None:
        """update self.pos and do self.next()"""
        
        # check if curr_char is newline and update pos respectifly
        if self.curr_char == '\n':
            self.pos.line += 1
            self.pos.column = 0
            self.pos.line_off = self.pos.offset + 1
        
        self.pos.column += 1
        self.pos.offset += 1
        
        self.next()

    def lex_file(self) -> list[Token]:
        tokens: list[Token] = []
        token: Token | None = self.next_token()
        while token:
            tokens.append(token)
            token = self.next_token()
        return tokens

    def isdigit(self) -> bool:
        """digits only contain [0-9]"""
        char: int = ord(self.curr_char)
        return char >= ord('0') and char <= ord('9')

    def islabelstart(self) -> bool:
        """labels can only start with [_A-Za-z] but can contain [_0-9A-Za-z]"""
        char: int = ord(self.curr_char)
        return (char == ord('_') or 
                (char >= ord('A') and char <= ord('Z')) or 
                (char >= ord('a') and char <= ord('z')))

    def islabel(self) -> bool:
        """labels can only start with [_A-Za-z] but can contain [_0-9A-Za-z]"""
        return self.islabelstart() or self.isdigit()

    def next_token(self) -> Token:
        """generate next token in sequence"""
        # skip whitespaces and comments
        while True:
            # whitespaces ` `, `\n`, `\r`, `\t`
            if (self.curr_char == ' ' or self.curr_char == '\n' or 
                self.curr_char == '\r' or self.curr_char == '\t'):
                self.advance()
            
            # line comment `//`
            elif self.curr_char == '/' and self.next_char == '/':
                while self.curr_char and self.curr_char != '\n':
                    self.advance()
            
            # block comment `/*` and `*/`
            elif self.curr_char == '/' and self.next_char == '*':
                while (self.curr_char and 
                       self.curr_char != '*' and self.next_char != '/'):
                    self.advance()

                # skip `*/` characters of block comment 
                # to not get them as Tokens
                self.advance()
                self.advance()
            
            # break loop no whitespace or comment found
            else:
                break
        
        if not self.curr_char:
            return None

        # deepcopy self.pos for Token.start,
        # to not have a reference to self.pos that canges
        pos: Position = deepcopy(self.pos)
        if self.curr_char == '+':
            self.advance()
            return Token(TokenKind.plus, pos, deepcopy(self.pos))
        elif self.curr_char == '@':
            self.advance()
            return Token(TokenKind.at, pos, deepcopy(self.pos))
        elif self.curr_char == ':':
            self.advance()
            return Token(TokenKind.colon, pos, deepcopy(self.pos))
        elif self.curr_char == ';':
            self.advance()
            return Token(TokenKind.semicolon, pos, deepcopy(self.pos))
        elif self.curr_char == '(':
            self.advance()
            return Token(TokenKind.l_paren, pos, deepcopy(self.pos))
        elif self.curr_char == ')':
            self.advance()
            return Token(TokenKind.r_paren, pos, deepcopy(self.pos))
        elif self.curr_char == '{':
            self.advance()
            return Token(TokenKind.l_brace, pos, deepcopy(self.pos))
        elif self.curr_char == '}':
            self.advance()
            return Token(TokenKind.r_brace, pos, deepcopy(self.pos))
        elif self.isdigit():
            while self.curr_char and self.isdigit():
                self.advance()
            return Token(TokenKind.integer, pos, deepcopy(self.pos))
        elif self.islabelstart():
            while self.islabel():
                self.advance()
            return Token(TokenKind.label, pos, deepcopy(self.pos))
        else:
            assert False, f"ERROR:{self.pos.line}:{self.pos.column}: Encountered an invalid token `{self.curr_char}` while lexing!"

class Parser:
    def __init__(self, lexer: Lexer) -> None:
        self.lexer: Lexer = lexer
        self.last_token: Token | None = None
        self.curr_token: Token | None = None
        self.next_token: Token | None = None

        # populate self.curr_token and self.next_token with data
        self.next()
        self.next()

    def next(self) -> None:
        self.last_token = self.curr_token
        self.curr_token = self.next_token
        self.next_token = self.lexer.next_token()

    def parse(self) -> None:
        stack: list[tuple[int]] = []
        program: list[tuple[int]] = []
        while isinstance(self.curr_token, Token):
            if self.curr_token.kind == TokenKind.dollar:
                stack.append()
            else:
                assert False, f"ERROR:{self.curr_token.start.line}:{self.curr_token.start.column}: while parsing!"

if __name__ == "__main__":
    if len(sys.argv) < 3:
        usage()
        assert False, "ERROR: To few input arguments, please provide more input arguments!"

    lexer: Lexer = Lexer(sys.argv[2])
    parser: Parser = Parser(lexer)
    
    if sys.argv[1] == "tokenise":
        print(TokenKind())
        for token in lexer.lex_file():
            print(token)
    elif sys.argv[1] == "parse":
        print(parser.parse())
    elif sys.argv[1] == "interpret":
        assert False, "Interpreter not yet implemented!"
    elif sys.argv[1] == "compile":
        assert False, "Compiler not yet implemented!"
    else:
        assert False, f"ERROR: Invalid subcommand given `{self.argv[1]}`, please provide a valid subcommand!"
