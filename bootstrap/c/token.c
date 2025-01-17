#ifndef __TOKEN_C__
#define __TOKEN_C__

#include "utils.c"

typedef enum {
    undefined,
    
    label,
    digits,
    number,
    string,

    plus,

    count,
} TokenKind;

TokenKind token_kind;

typedef struct {
    usize line;
    usize column;
    uchar *line_off;
    uchar *offset;
} Position, *pPosition;

typedef struct {
    TokenKind kind;
    Position  start;
    Position  end;
} Token, *pToken;

typedef struct {
    usize capacity;     // in tokens
    usize count;        // in tokens
    Token tokens[];
} Tokens, *pTokens;

#ifndef TOKENS_CAPACITY
#define TOKENS_CAPACITY 8 * 1024    // 8K Tokens
#endif // TOKENS_CAPACITY

#define new_pos(l, c, lo, o) ((Position){.line=l, .column=c, \
        .line_off=lo, .offset=o})
#define new_token(k, s, e) ((Token){.kind=k, .start=s, .end=e})
#define empty_pos new_pos(0, 0, 0, 0)

pTokens new_tokens() {
    pTokens tokens;

    tokens = malloc(sizeof(Tokens) + TOKENS_CAPACITY);
    tokens->capacity = TOKENS_CAPACITY;
    tokens->count = 0;
    
    return tokens;
}

void del_tokens(pTokens *tokens) {
    free(*tokens);
    *tokens = NULL;
}

void tokens_append(pTokens *tokens, Token token) {
    pTokens ntokens;

    if ((*tokens)->count + 1 > (*tokens)->capacity) {
        ntokens = malloc(sizeof(Tokens) + (*tokens)->capacity*2);
        for (usize i = 0; i < (*tokens)->count; i++) {
            ntokens->tokens[i] = (*tokens)->tokens[i];
        }
        free(*tokens);
        *tokens = ntokens;
    }

    (*tokens)->count++;
    (*tokens)->tokens[(*tokens)->count] = token;
}

#endif // __TOKEN_C__
