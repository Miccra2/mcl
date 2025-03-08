#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "span.h"

extern char *TOKEN_KIND[];

typedef enum {
    TOKEN_UNDEFINED,
    TOKEN_INTEGER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_SEMICOLON,
    TOKEN_COUNT,
} token_kind_t;

typedef struct {
    token_kind_t kind;
    span_t       span;
} token_t;

typedef struct {
    unsigned char left;
    unsigned char right;
} priority_t;

#define new_token(k, s) ((token_t){.kind=k, .span=s})
#define new_priority (l, r) ((priority_t){.left=l, .right=r})

priority_t get_priority(token_kind_t kind);

#endif // __TOKEN_H__
