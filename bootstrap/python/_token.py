from dataclasses import dataclass, field

__all__: list[str] = [
    "iota",
    "default",
    "TokenKind",
    "Token",
    "Position",
]

IOTA_COUNTER: int = 0
def iota(reset: bool = False) -> int:
    global IOTA_COUNTER
    if reset:
        IOTA_COUNTER += 1
    result: int = IOTA_COUNTER
    IOTA_COUNTER += 1
    return result

def default(cls: any) -> any:
    return field(default_facotry=cls())

@dataclass
class TokenKind:
    undefined: int = iota(True)

    number: int = iota()            # has `.` in it (float)
    digit: int = iota()             # has no `.` in it (integer)

    plus: int = iota()
    minus: int = iota()

    count: int = iota()

@dataclass
class Type:
    undefined: int = iota(True)
    
    i64: int = iota()
    
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
    start: Position        = default(Position)
    end:   Position        = default(Position)
