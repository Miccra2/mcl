#ifndef __FMT_H__
#define __FMT_H__

#include <stdio.h>

#include "span.h"

#define mcl_fmt_position(p) \
    printf("position(%d, %d, %d, %d)", \
            (size_t)(p).line, (size_t)(p).column, \
            (size_t)(p).line_off, (size_t)(p).offset)
#define mcl_fmt_span(s) ({ \
    printf("span("); \
    mcl_fmt_position ((s).start); \
    printf(", "); \
    mcl_fmt_position ((s).end); \
    printf(")"); \
})

#endif // __FMT_H__
