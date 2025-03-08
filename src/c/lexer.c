#include "lexer.h"

lexer_t *new_lexer(char *path) {
    lexer_t *lexer = malloc(sizeof(lexer_t));
    if (!lexer) return NULL;

    lexer->path_length = strlen(path);
    lexer->path = path;
    lexer->position = new_position(1, 1, 0, 0);
    lexer->curr_char = 0;
    lexer->next_char = 0;
    lexer->char_index = 0;

    FILE *pf = fopen(lexer->path, "r");
    if (!pf) {
        fprintf(stderr, "ERROR:Lexer: Could not open file `%s`, "
                "pealse provide a valid file path!\n", lexer->path);
        fclose(pf);
        return NULL;
    }

    fseek(pf, 0, SEEK_END);
    lexer->text_length = ftell(pf);
    lexer->text = malloc(lexer->text_length);
    rewind(pf);
    fread(lexer->text, lexer->text_length, 1, pf);

    fclose(pf);
    return lexer;
}

void del_lexer(lexer_t **lexer) {
    free((*lexer)->text);
    free(*lexer);
    *lexer = NULL;
}

void lexer_next(lexer_t *lexer) {
    lexer->curr_char = lexer->next_char;
    if (lexer->char_index < lexer->text_length) {
        lexer->next_char = lexer->text[lexer->char_index];
    } else {
        lexer->next_char = 0;
    }
    lexer->char_index++;
}

void lexer_advance(lexer_t *lexer, bool newline) {
    if (newline) {
        lexer->position.line++;
        lexer->position.column = 0;
        lexer->position.line_off = lexer->position.offset + 1;
    }

    lexer->position.column++;
    lexer->position.offset++;

    lexer_next(lexer);
}

da_t lexer_tokenise(lexer_t *lexer) {
    da_t tokens = new_da();
    token_t token = lexer_next_token(lexer);
    
    while (token.kind) {
        da_append(tokens, token);
        token = lexer_next_token(lexer);
    }

    return tokens;
}

token_t lexer_next_token(lexer_t *lexer) {
    while (lexer->curr_char) {
        if (lexer->curr_char == ' ' || lexer->curr_char == '\n' || 
                lexer->curr_char == '\r' || lexer->curr_char == '\t') {
            lexer_advance(lexer, lexer->curr_char=='\n');
        } else if (lexer->curr_char == '/' && lexer->next_char == '/') {
            while (lexer->curr_char && lexer->curr_char != '\n') {
                lexer_advance(lexer, lexer->curr_char=='\n');
            }
        } else if (lexer->curr_char == '/' && lexer->next_char == '*') {
            while (lexer->curr_char && lexer->next_char && 
                    lexer->curr_char != '*' && lexer->next_char != '/') {
                lexer_advance(lexer, lexer->curr_char=='\n');
            }
            lexer_advance(lexer, false);
            lexer_advance(lexer, false);
        } else {
            break;
        }
    }

    if (!lexer->curr_char) {
        fprintf(stderr, "ERROR:Lexer:%d:%d:%s: End of file reached!\n", 
                lexer->position.line, lexer->position.column, lexer->path);
        return new_token (TOKEN_UNDEFINED, 
                          new_span (lexer->position, lexer->position));
    }

    position_t position = lexer->position;
    token_kind_t kind = TOKEN_UNDEFINED;
    if (lexer->curr_char == '+') {
        kind = TOKEN_PLUS;
    } else if (lexer->curr_char == ';') {
        kind = TOKEN_SEMICOLON;
    } else if (is_digit(lexer->curr_char)) {
        while (lexer->curr_char && is_digit(lexer->curr_char)) {
            lexer_advance(lexer, false);
        }
        return new_token (TOKEN_INTEGER, new_span(position, lexer->position));
    } else {
        fprintf(
            stderr, 
            "ERROR:Lexer:%d:%d:%s: Encountred an invalid token "
            "(curr=0x%.2X, next=0x%.2X)!\n", 
            position.line, 
            position.column, 
            lexer->path, 
            lexer->curr_char, 
            lexer->next_char
        );
    }

    lexer_advance(lexer, false);
    return new_token (kind, new_span (position, lexer->position));
}
