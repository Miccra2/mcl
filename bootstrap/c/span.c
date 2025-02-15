#ifndef __SPAN_C__
#define __SPAN_C__

typedef struct {
    long int line;
    long int column;
    long int line_off;
    long int offset;
} Position;

typedef struct {
    Position start;
    Position end;
} Span;

#define new_position(l, c, l_off, off) \
    (Position){.line=l, .column=c, .line_off=l_off, .offset=off}
#define new_span(s, e) (Span){.start=s, .end=e}
#define default_position() new_position(1, 1, 0, 0)
#define default_span() new_span(default_position(), default_position())
#define span_len(s) (s.start.offset - s.end.offset)

int span_make_int(char *text, Span *span, size_t *dummy) {
    char *ptr;
    
    ptr = text + span->end.offset - 1;
    *dummy = 0;
    while (ptr >= 0 && ptr >= text + span->start.offset) {
        *dummy *= 10;
        *dummy += *ptr - '0';
        ptr--;
    }

    return 0;
}

#endif // __SPAN_C__
