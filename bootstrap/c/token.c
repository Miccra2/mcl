#ifndef __TOKEN_C__
#define __TOKEN_C__

#include <stdint.h>

typedef enum {
    undefined,

    digits,
    label,

    plus,

    count,
} TokeKind;

typedef struct {
    uint64_t line;
    uint64_t column;
    uint64_t line_off;
    uint64_t offset;
} Position;

typedef struct {
    TokenKind kind;
    Position  start;
    Position  end;
} Token;

Position new_pos(uint64_t line, uint64_t column, uint64_t line_off, uint64_t offset) {
    return (Position){.line=line, .column=column, .line_off=line_off, .offset=offset};
}

Token new_token(TokeKind kind, Position start, Position end) {
    return (Token){.kind=kind, .start=start, .end=end};
}

#endif // __TOKEN_C__
