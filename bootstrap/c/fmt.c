#ifndef __FMT_C__
#define __FMT_C__

#include <stdio.h>
//#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#include "span.c"
#include "token.c"
#include "lexer.c"
#include "parser.c"
#include "ast.c"

typedef struct {
    size_t level_length;
    size_t curr_level;
    bool newline;
} FMT;

#define fmt_print(fmt, str, ...) { \
    for (size_t i = 0; i < (fmt)->level_length*(fmt)->curr_level; i++) \
        putc(' ', stdout); \
    printf(str __VA_OPT__(,) __VA_ARGS__); \
    if ((fmt)->newline) printf("\n"); \
}

FMT *new_fmt() {
    FMT *fmt;
    
    fmt = (FMT *)malloc(sizeof(FMT));
    fmt->level_length = 4;
    fmt->curr_level = 0;
    fmt->newline = false;
    
    return fmt;
}

void del_fmt(FMT **fmt) {
    free(*fmt);
    *fmt = NULL;
}

void fmt_position(FMT *fmt, Position pos) {
    fmt_print(fmt, "Position(%d, %d, %d, %d)", 
              pos.line, pos.column, pos.line_off, pos.offset); 
}

void fmt_span(FMT *fmt, Span span) { 
    fmt_print(fmt, "Span("); 
    fmt_position(fmt, span.start);  
    fmt_print(fmt, ", "); 
    fmt_position(fmt, span.end); 
    fmt_print(fmt, ")");
}

void fmt_token(FMT *fmt, Token token) {
    fmt_print(fmt, "Token(%d, ", token.kind);
    fmt_span(fmt, token.span);
    fmt_print(fmt, ")");
}

#endif // __FMT_C__
