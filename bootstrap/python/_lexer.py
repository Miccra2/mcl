from copy import deepcopy
from _token import *

__all__: list[str] = ["Lexer",]

class Lexer:
    def __init__(self, path: str) -> None:
        # public
        self.path: str = path
        self.text: str = ""
        self.pos: Position = default(Position)

        # private
        self.char_index: int = 0
        self.curr_char: str | None = None
        self.next_char: str | None = None

        try:
            with open(self.path, "r") as file:
                self.text = file.read()
        except:
            usage()
            assert False, f"ERROR: Could not open file `{self.path}`!"

        # populate self.curr_char and self.next_char
        self.next()
        self.next()

    def __next__(self) -> Token:
        return self.next_token()
    
    def next(self) -> None:
        self.curr_char = self.next_char
        
        if self.char_index < len(self.text):
            self.next_char = self.text[self.char_index]
        else:
            self.next_char = None

        self.char_index += 1

    def advance(self) -> None:
        if self.curr_char == '\n':
            self.pos.line += 1
            self.pos.column = 0
            self.pos.line_off = self.pos.offset + 1
        self.pos.column += 1
        self.pos.offset += 1
        self.next()

    def lex_file(self) -> list[Token]:
        tokens: list[Toke] = []
        while isinstance(self.curr_char, str):
            tokens.append(self.next_token())
        return tokens
    
    def skip_line_comment(self) -> None:
        while isinstance(self.curr_char, str) and self.curr_char != '\n':
            self.advance()

    def skip_block_comment(self) -> None:
        while isinstance(self.curr_char, str) and self.curr_char != '*' and self.next_char != '/':
            self.advance()

        # skip end of block comment `*/` to avoid getting them as tokens
        self.advance()
        self.advance()

    def isdigit(self) -> bool:
        char: int = ord(self.curr_char)
        return char >= ord('0') and char <= ord('9')

    def isnumber(self) -> bool:
        return self.curr_char == '.' or self.isdigt()

    def islabelstart(self) -> bool:
        char: int = ord(self.curr_char)
        return (char == ord('_') or 
                (char >= ord('A') and char <= ord('Z')) or 
                (char >= ord('a') and char <= ord('z')))

    def islabel(self) -> bool:
        return self.islabelstart() or self.isdigit()

    def next_token(self) -> Token | None:
        # skip whitespaces (` `, `\n` and `\t`), line comments (`//`)
        # and block comments (`/*` and `*/`)
        while isinstance(self.curr_char, str):
            if self.curr_char == ' ' or self.curr_char == '\n' or self.curr_char == '\t':
                self.advance()
            elif self.curr_char == '/' and self.next_char == '/':
                self.skip_line_comment()
            elif self.curr_char == '/' and self.next_char == '*':
                self.skip_block_comment()
            else:
                break

        if not isinstance(self.curr_char, str):
            return None
        
        pos: Position = deepcopy(self.pos)
        if self.curr_char == '+':
            self.advance()
            return Token(TokenKind.plus, pos, deepcopy(self.pos))
        elif self.isdigit():
            isfloat: bool = False
            while isinstance(self.curr_char, str) and self.isnumber():
                if self.curr_char == '.':
                    if islfloat:
                        assert False, f"ERROR:{pos.line}:{pos.column}: Number can only have one `.`, but found more than one!"
                    isflaot = True
                self.advance()
            if isfloat:
                return Token(TokenKind.number, pos, deepcopy(self.pos))
            return Token(TokenKind.digit, pos, deepcopy(self.pos))
        elif self.islabelstart():
            while isinstance(self.curr_char, str) and self.islabel():
                self.advance()
            return Token(TokenKind.lable, pos, deepcopy(self.pos))
        else:
            assert False, f"ERROR:{pos.line}:{pos.column}: Encountered an invalid token `{self.curr_char}` while lexing!"
