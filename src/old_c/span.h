#ifndef __SPAN_H__
#define __SPAN_H__

#include <stddef.h>

typedef struct {
    size_t line;
    size_t column;
    size_t line_off;
    size_t offset;
} mcl_position_t;

typedef struct {
    mcl_position_t start;
    mcl_position_t end;
} mcl_span_t;

#define mcl_new_position(l, c, lo, o) \
    ((mcl_position_t){.line=l, .column=c, .line_off=lo, .offset=o})
#define mcl_new_span(s, e) \
    ((mcl_span_t){.start=s, .end=e})

#endif // __SPAN_H__
