#ifndef __LEXER_C__
#define __LEXER_C__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "token.c"
#include "dynamic_array.c"

#define ISDIGIT(x) ((x)>='0' && (x)<='9')
#define ISNUMBER(x) ((x)=='.' || ISDIGIT(x))
#define ISLABELSTART(x) ((x)=='_' || ((x)>='A' && (x)<='Z') || ((x)>='a' && (x)<='z'))
#define ISLABEL(x) (ISLABELSTART(x) || ISDIGIT(x))

Lexer *new_lexer(char *path);
void del_lexer(Lexer **lexer);
void lexer_next(Lexer *lexer);
void lexer_davance(Lexer *lexer);
Token *lexer_next_token(Lexer *lexer);
void del_token(Token **token);
DynamicArray *lex_file(char *path);

typedef struct {
    uint64_t path_length;
    char *path;
    uint64_t text_length;
    char *text;
    Position pos;

    uint64_t char_index;
    char curr_char;
    char next_char;
} Lexer;

Lexer *new_lexer(char *path) {
    FILE *pFile = fopen(path, "r");
    if (!pFile) {
        fprintf(stderr, "ERROR: Could not open file `%s`!\n", path);
        return NULL;
    }
    fseed(pFile, 0, SEEK_END);
    uint64_t size = ftell(pFile);
    char *buffer = malloc(size);
    rewind(pFile);
    fread(buffer, 1, size, pFile);
    fclose(pFile);

    Lexer *lexer = malloc(sizeof(Lexer));
    lexer->path_length = strlen(path);
    lexer->path = path;
    lexer->text_length = size;
    lexer->text = buffer;
    lexer->pos = new_pos(1, 1, 0, 0);           // line=1, column=1, line_off=0, offset=0
    lexer->char_index = 0;
    lexer->curr_char = 0;
    lexer->next_char = 0;
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

void lexer_advance(Lexer *lexer) {
    if (lexer->curr_char == '\n') {
        lexer->pos.line++;
        lexer->pos.column = 0;
        lexer->pos.line_off = lexer->pos.offset+1;
    }
    lexer->pos.column++;
    lexer->pos.offset++;
    lexer_next(lexer);
}

DynamicArray *lex_file(char *path) {
    Lexer *lexer = new_lexer(path);
    DynamicArray *tokens = new_dynamic_array();
    while (lexer->char_index < lexer->text_length) {
        Token token = lexer_next_token(lexer);
        dynamic_array_append(&tokens, &token, sizeof(Token));
    }
    return tokens;
}

Token *lexer_next_token(Lexer *lexer) {
    while (lexer->char_index < lexer->text_length) {
        if (lexer->curr_char == ' ' || 
            lexer->curr_char == '\n' || 
            lexer->curr_char == '\t') {
            lexer_advance(lexer);
        } else if (lexer->curr_char == '/' && lexer->next_char == '/') {
            while (lexer->char_index < lexer->text_length &&
                   lexer->curr_char != '\n') {
                lexer_advance(lexer);
            }
        } else if (lexer->curr_char == '/' && lexer->next_char == '*') {
            while (lexer->char_index < lexer->text_length && 
                   lexer->curr_char != '*' && 
                   lexer->next_char != '/') {
                lexer_advance(lexer);
            }
            lexer_advance(lexer);
            lexer_advance(lexer);
        } else {
            break;
        }
    }
    if (lexer->char_index >= lexer->text_length) {
        return NULL;
    }
    Position pos = lexer->pos;
    Token *token = malloc(sizeof(Token));
    if (lexer->curr_char == '+') {
        lexer_advance(lexer);
        token->kind = TokenKind.plus;
        token->start = pos;
        token->end = lexer->pos;
        return token;
    } else if (ISDIGIT(lexer->curr_char)) {
        bool isnumber = false;
        while (lexer->char_index < lexer->text_length && 
               ISNUMBER(lexer->curr_char)) {
            if (lexer->curr_char == '.') {
                isnumber = true;
            }
            lexer_advance(lexer);
        }
        if (isnumber) {
            token->kind = TokenKind.number;
        } else {
            token->kind = TokenKind.digit;
        }
        token->start = pos;
        token->end = lexer->pos;
        return token;
    } else if (ISLABELSTART(lexer->curr_char)) {
        while (lexer->char_index < lexer->text_length && 
               ISLABEL(lexer->curr_char)) {
            lexer_advance(lexer);
        }
        token->kind = TokenKind.label;
        token->start = pos;
        token->end = lexer->pos;
        return token;
    } else {
        fprintf(stderr, "ERROR:l%d:c%d: Encountered an invalid token `%c` while parsing!\n", lexer->curr_char);
        return NULL;
    }
}

#endif // __LEXER_C__
