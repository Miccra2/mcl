#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "dynamic_array.h"
#include "token.h"

typedef struct {
    size_t path_length;
    char   *path;
    size_t text_length;
    char   *text;

    mcl_position_t position;

    size_t char_index;
    char   curr_char;
    char   next_char;
} mcl_lexer_t;

#define mcl_inrange(l) (lexer->curr_char)
#define mcl_iswhitespace(c) ((c)==' ' || (c)=='\n' || (c)=='\r' || (c)=='\t')
#define mcl_islinecomment(l) ((l)->curr_char == '/' && (l)->next_char == '/')
#define mcl_isblockcomment(l) ((l)->curr_char == '/' && (l)->next_char == '*')
#define mcl_isdigit(c) ((c)>='0' && (c)<='9')

mcl_lexer_t *mcl_new_lexer(char *path);
void mcl_del_lexer(mcl_lexer_t **lexer);
void mcl_lexer_next(mcl_lexer_t *lexer);
void mcl_lexer_advance(mcl_lexer_t *lexer, bool newline);
mcl_token_t mcl_lexer_next_token(mcl_lexer_t *lexer);
da_t *mcl_lexer_tokenise(mcl_lexer_t *lexer);

#endif // __LEXER_H__
