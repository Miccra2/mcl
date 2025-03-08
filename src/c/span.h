#ifndef __SPAN_H__
#define __SPAN_H__

#include <stddef.h>

typedef struct {
    size_t line;
    size_t column;
    size_t line_off;
    size_t offset;
} position_t;

typedef struct {
    position_t start;
    position_t end;
} span_t;

#define new_position(l, c, lo, o) \
    ((position_t){.line=l, .column=c, .line_off=lo, .offset=o})
#define new_span(s, e) ((span_t){.start=s, .end=e})

#endif // __SPAN_H__
