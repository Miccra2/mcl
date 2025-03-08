#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "dynamic_array.h"
#include "span.h"
#include "token.h"

typedef struct {
    size_t path_length;
    char *path;
    size_t text_length;
    char *text;
    position_t position;
    char curr_char;
    char next_char;
    size_t char_index;
} lexer_t;

#define is_digit(c) ((c)>='0' && (c)<='9')

lexer_t *new_lexer(char *path);
void del_lexer(lexer_t **lexer);
void lexer_next(lexer_t *lexer);
void lexer_advance(lexer_t *lexer, bool newline);
da_t lexer_tokenise(lexer_t *lexer);
token_t lexer_next_token(lexer_t *lexer);

#endif // __LEXER_H__
