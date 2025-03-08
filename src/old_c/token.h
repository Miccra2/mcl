#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stdint.h>

#include "span.h"

static char *MCL_TOKEN_KIND[] = {
    "MCL_TOKEN_UNDEFINED",
    "MCL_TOKEN_INTEGER",
    "MCL_TOKEN_PLUS",
    "MCL_TOKEN_SEMICOLON",
    "MCL_TOKEN_COUNT",
};

typedef enum {
    MCL_TOKEN_UNDEFINED,
    MCL_TOKEN_INTEGER,
    MCL_TOKEN_PLUS,
    MCL_TOKEN_SEMICOLON,
    MCL_TOKEN_COUNT,
} mcl_token_kind_t;

typedef struct {
    mcl_token_kind_t kind;
    mcl_span_t       span;
} mcl_token_t;

typedef struct {
    uint8_t left;
    uint8_t right;
} mcl_priority_t;

#define mcl_new_token(k, s) \
    ((mcl_token_t){.kind=k, .span=s})

mcl_priority_t mcl_token_get_priority(mcl_token_kind_t kind);

#endif // __TOKEN_H__
