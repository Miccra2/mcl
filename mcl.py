#!/usr/bin/env python3.12
"""bootstrap code for mcl (my coding language)"""
from dataclasses import dataclass, field
from copy import deepcopy
import sys

USAGE: str = "Usage: python mcl.py [file]\n    file - path to mcl source file `.mcs`"

IOTA_COUNTER: int = 0
def iota(reset: bool = False) -> int:
    global IOTA_COUNTER
    if reset:
        IOTA_COUTER = 0
    result: int = IOTA_COUNTER
    IOTA_COUNTER += 1
    return result

@dataclass
class Position:
    line:   int = 1
    column: int = 1
    offset: int = 0

@dataclass
class TokenKind:
    none: int = iota(True)

    label: int = iota()
    digits: int = iota()
    dollar: int = iota()
    colon: int = iota()
    
    l_paren: int = iota()
    r_paren: int = iota()

    plus: int = iota()

@dataclass
class Token:
    kind:  TokenKind = TokenKind.none
    start: Position  = field(default_factory=Position())
    end:   Position  = field(default_factory=Position())

@dataclass
class Lexer:
    path: str = ""
    text: str = ""
    pos: Position = field(default_factory=Position())

    index: int = 0
    curr_char: str = ''
    next_char: str = ''

def isdigit(char: str) -> bool:
    return ord(char) >= ord('0') and ord(char) <= ord('9')

def islabelstart(char: str) -> bool:
    return (char == '_' or 
            (ord(char) >= ord('A') and ord(char) <= ord('Z')) or 
            (ord(char) >= ord('a') and ord(char) <= ord('z')))

def islabel(char: str) -> bool:
    return islabelstart(char) or isdigit(char)

# next_lexer updates Lexer.index, Lexer.curr_char and Lexer.next_char only
# to hinder having an incomplete tokens 
# (mostly for first token on initialization with new_lexer)
def next_lexer(lexer: Lexer) -> None:
    lexer.curr_char = lexer.next_char
    lexer.next_char = lexer.text[lexer.index] if lexer.index < len(lexer.text) else None
    lexer.index += 1

# advance_lexer updates pos and everything next_lexer does
def advance_lexer(lexer: Lexer) -> None:
    if lexer.curr_char == '\n':
        lexer.pos.line += 1
        lexer.pos.column = 0
    
    lexer.pos.column += 1
    lexer.pos.offset += 1
    next_lexer(lexer)

def new_lexer(path: str) -> Lexer:
    text: str = ""
    with open(path, "r") as file:
        text = file.read()
    
    lexer: Lexer = Lexer(path=path, text=text, pos=Position())
    next_lexer(lexer)
    next_lexer(lexer)

    return lexer

def next_token(lexer: Lexer) -> Token:
    # skip white space characters and comments
    while True:
        if lexer.curr_char == ' ' or lexer.curr_char == '\n' or lexer.curr_char == '\r' or lexer.curr_char == '\t':
            advance_lexer(lexer)
        elif lexer.curr_char == '/' and lexer.next_char == '/':
            while lexer.curr_char != None and lexer.curr_char != '\n':
                advance_lexer(lexer)
        elif lexer.curr_char == '/' and lexer.next_char == '*':
            while lexer.next_char != None and lexer.curr_char != '*' and lexer.next_char != '/':
                advance_lexer(lexer)
            advance_lexer(lexer)
        else:
            break
    
    # check if end of text is reached
    if lexer.curr_char == None:
        return None

    # tokenise next token from Lexer.index
    pos: Position = deepcopy(lexer.pos)
    if lexer.curr_char == '+':
        advance_lexer(lexer)
        return Token(TokenKind.plus, pos, deepcopy(lexer.pos))
    elif lexer.curr_char == ';':
        advance_lexer(lexer)
        return Token(TokenKind.colon, pos, deepcopy(lexer.pos))
    elif lexer.curr_char == '(':
        advance_lexer(lexer)
        return Token(TokenKind.l_paren, pos, deepcopy(lexer.pos))
    elif lexer.curr_char == ')':
        advance_lexer(lexer)
        return Token(TokenKind.r_paren, pos, deepcopy(lexer.pos))
    elif lexer.curr_char == '$':
        advance_lexer(lexer)
        return Token(TokenKind.dollar, pos, deepcopy(lexer.pos))
    elif lexer.curr_char != None and isdigit(lexer.curr_char):
        while lexer.curr_char != None and isdigit(lexer.curr_char):
            advance_lexer(lexer)

        return Token(TokenKind.digits, pos, deepcopy(lexer.pos))
    elif lexer.curr_char != None and islabelstart(lexer.curr_char):
        while lexer.curr_char != None and islabel(lexer.curr_char):
            advance_lexer(lexer)

        return Token(TokenKind.label, pos, deepcopy(lexer.pos))
    else:
        assert False, f"[ERROR] Invalid token '{lexer.curr_char}'!"

def lex_file(lexer) -> list[Token]:
    tokens: list = []
    token: Token = next_token(lexer)
    
    while token != None:
        tokens.append(token)
        token = next_token(lexer)

    return tokens

if __name__ == "__main__":
    if len(sys.argv) < 2:
        assert False, "[ERROR] To few input arguments given, please provide more input arguments!\n" + USAGE

    path: str = sys.argv[1]
    lexer: Lexer = new_lexer(path)
    tokens: list = lex_file(deepcopy(lexer))
   
    print(TokenKind())
    for token in tokens:
        print(f"`{lexer.text[token.start.offset:token.end.offset]}` {token}", token.start.offset, token.end.offset)
