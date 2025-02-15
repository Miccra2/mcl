#ifndef __LEXER_C__
#define __LEXER_C__

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "span.c"
#include "token.c"

typedef struct {
    size_t path_length;
    char *path;
    size_t text_length;
    char *text;
    Position position;
    char curr_char;
    char next_char;
    size_t char_index;
} Lexer;

Lexer *new_lexer(char *path);
void del_lexer(Lexer **lexer);
void lexer_next(Lexer *lexer);
void lexer_advance(Lexer *lexer, bool newline);
Tokens *lexer_tokenise(Lexer *lexer);
Token lexer_next_token(Lexer *lexer);

Lexer *new_lexer(char *path) {
    size_t size;
    FILE *pFile;
    
    Lexer *lexer = malloc(sizeof(Lexer));
    if (!lexer) return NULL;
    lexer->path_length = (size_t)strlen(path);
    lexer->path        = path;
    lexer->position    = new_position(1, 1, 0, 0);
    lexer->curr_char   = 0;
    lexer->next_char   = 0;
    lexer->char_index  = 0;
    
    pFile = fopen(path, "r");
    fseek(pFile, 0, SEEK_END);
    lexer->text_length = sizeof(char)*ftell(pFile);
    rewind(pFile);
    lexer->text = (char *)malloc(lexer->text_length);
    if (!lexer->text) return NULL;
    fread(lexer->text, 1, lexer->text_length, pFile);
    fclose(pFile);

    lexer_next(lexer);
    lexer_next(lexer);

    return lexer;
}

void del_lexer(Lexer **lexer) {
    free((*lexer)->text);
    free(*lexer);
    *lexer = NULL;
}

void lexer_next(Lexer *lexer) {
    lexer->curr_char = lexer->next_char;
    if (lexer->char_index < lexer->text_length) {
        lexer->next_char = lexer->text[lexer->char_index];
    } else {
        lexer->next_char = 0;
    }
    lexer->char_index++;
}

void lexer_advance(Lexer *lexer, bool newline) {
    if (newline) {
        lexer->position.line++;
        lexer->position.column = 0;
        lexer->position.line_off = lexer->position.offset + 1;
    }

    lexer->position.column++;
    lexer->position.offset++;
    
    lexer_next(lexer);
}

Tokens *lexer_tokenise(Lexer *lexer) {
    Tokens *tokens;
    Token token;

    tokens = new_tokens();
    token = lexer_next_token(lexer);
    while (lexer->curr_char) {
        tokens_append(tokens, &token);
        token = lexer_next_token(lexer);
    }

    return tokens;
}

#define isdigit(c) (c>='0' && c<='9')

Token lexer_next_token(Lexer *lexer) {
    Position position;
    TokenKind kind;

    while (lexer->curr_char) {
        if (lexer->curr_char == ' ' || lexer->curr_char == '\n'
                || lexer->curr_char == '\r' || lexer->curr_char == '\t') {
            lexer_advance(lexer, lexer->curr_char=='\n');
        } else if (lexer->curr_char == '/' && lexer->next_char == '/') {
            while (lexer->curr_char && lexer->curr_char != '\n') {
                lexer_advance(lexer, false);
            }
        } else if (lexer->curr_char == '/' && lexer->next_char == '*') {
            while (lexer->curr_char && lexer->curr_char != '*' 
                                    && lexer->next_char != '/') {
                lexer_advance(lexer, false);
            }
            lexer_advance(lexer, false);
            lexer_advance(lexer, false);
        } else {
            break;
        }
    }

    if (!lexer->curr_char) {
        return new_token(TOKEN_UNDEFINED, default_span());
    }

    position = lexer->position;
    kind = TOKEN_UNDEFINED;
    if (lexer->curr_char == '+') {
        kind = TOKEN_PLUS;
    } else if (lexer->curr_char == ';') {
        kind = TOKEN_SEMICOLON;
    } else if (isdigit(lexer->curr_char)) {
        while (lexer->curr_char && isdigit(lexer->next_char)) {
            lexer_advance(lexer, false);
        }
        kind = TOKEN_DIGITS;
    }

    lexer_advance(lexer, false);
    return new_token(kind, new_span(position, lexer->position));
}

#endif // __LEXER_C__
