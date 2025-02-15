#ifndef __TOKEN_C__
#define __TOKEN_C__

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "span.c"
#include "fmt.c"

typedef enum {
    TOKEN_UNDEFINED,
    TOKEN_DIGITS,
    TOKEN_PLUS,
    TOKEN_SEMICOLON,
    TOKEN_COUNT,
} TokenKind;

char *TOKEN_KIND[] = {
    "TOKEN_UNDEFINED",
    "TOKEN_DIGITS",
    "TOKEN_PLUS",
    "TOKEN_SEMICOLON",
    "TOKEN_COUNT",
};

typedef struct {
    TokenKind kind;
    Span      span;
} Token;

typedef struct {
    size_t capacity;
    size_t count;
    Token  token[];
} Tokens;

typedef struct {
    size_t left;
    size_t right;
} Priority;

#ifndef TOKENS_DEFAULT_CAPACITY
#define TOKENS_DEFAULT_CAPACITY 8 * 1024
#endif // TOKENS_DEFAULT_CAPACITY

#define new_token(k, s) (Token){.kind=k, .span=s}
#define new_priority(r, l) (Priority){.right=r, .left=l}
#define default_token() new_token(TOKEN_UNDEFINED, default_span())
#define token_len(t) span_len((t)->span)

Tokens *new_tokens() {
    Tokens *tokens;
    size_t size;

    size = sizeof(Tokens) + sizeof(Token)*TOKENS_DEFAULT_CAPACITY;
    tokens = (Tokens *)malloc(size);
    tokens->capacity = TOKENS_DEFAULT_CAPACITY;
    tokens->count = 0;

    return tokens;
}

void del_tokens(Tokens **tokens) {
    free(*tokens);
    *tokens = NULL;
}

void tokens_append(Tokens *tokens, Token *token) {
    if (tokens->count > tokens->capacity) {
        tokens->capacity *= 2;
        tokens = realloc(tokens, sizeof(Tokens)+tokens->capacity);
    }
    tokens->token[tokens->count] = *token;
    tokens->count++;
}

#define token_make_int(p, t, d) span_make_int((p)->lexer->text, &(t)->span, d)

Priority token_get_priority(TokenKind kind) {
    switch (kind) {
    case TOKEN_PLUS:
        return new_priority(1, 2);
    default:
        return new_priority(0, 0);
    }
}

#endif // __TOKEN_C__
