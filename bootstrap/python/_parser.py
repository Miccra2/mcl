from _lexer import *

__all__: list[str] = ["Parser",]

class Parser:
    def __init__(self, lexer: Lexer) -> None:
        self.lexer: Lexer = lexer
        self.last_token: Token | None = None
        self.curr_token: Token | None = None
        self.next_token: Token | None = None
        
        self.program: list[tuple[int]] = []

        # populate self.curr_token and self.next_token
        self.next()
        self.next()

    def next(self) -> None:
        self.last_token = self.curr_token
        self.curr_token = self.next_token
        self.next_token = self.lexer.next_token()

    def try_integer(self, token: Token) -> tuple[int] | None:
        # (type, value)
        if token.kind != TokenKind.digit:
            assert False, f"ERROR:{token.start.line}:{token.start.column}: try_integer() expected token of kind `digit` ({TokenKind.digit})!"
        if token.end.offset-token.start.offset > 20:
            assert False, f"ERROR:{token.start.line}:{token.start.column}: try_integer() length of token ({token.end.offset-token.start.offset}) seams to be over 20!"
        text: str = self.lexer.text[token.start.offset:token.end.offset]
        value: int = 0
        for i in len(text):
            value = value * 10 + int(text[i], 10)
        return (Type.i64, value)

    def try_add(self) -> tuple[int] | None:
        # (instruction, value0, value1)
        if (self.last_char.kind == TokenKind.digit and 
            self.next_char.kind == TokenKind.digit):
            return (Ins.add, self.try_integer(), self.try_integer())

    def parse(self) -> list[tuple[int]]:
        while isinstance(self.curr_token, str):
            if self.curr_token.kind == TokenKind.plus:
                self.try_add()
            else:
                print(TokenKind())
                assert False, f"ERROR:{self.curr_token.start.line}:{self.curr_token.start.column}: Encountered an invalid token ({self.curr_token.kind}) while parsing!"
