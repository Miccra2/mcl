#include "lexer.h"

mcl_lexer_t *mcl_new_lexer(char *path) {
    mcl_lexer_t *lexer = malloc (sizeof (mcl_lexer_t));

    lexer->path_length = strlen(path);
    lexer->path = path;
    lexer->position = mcl_new_position(1, 1, 0, 0);
    lexer->char_index = 0;
    lexer->curr_char = 0;
    lexer->next_char = 0;

    FILE *pf = fopen(lexer->path, "r");
    if (!pf) {
        fprintf(stderr, "ERROR: %s: Could not open file, "
                "please provide a valid file path!\n", lexer->path);
        return NULL;
    }

    fseek(pf, 0, SEEK_END);
    lexer->text_length = ftell(pf);
    lexer->text = malloc (lexer->text_length);
    
    rewind(pf);
    fread(lexer->text, 1, lexer->text_length, pf);
    
    fclose(pf);

    mcl_lexer_next(lexer);
    mcl_lexer_next(lexer);

    return lexer;
}

void mcl_del_lexer(mcl_lexer_t **lexer) {
    free((*lexer)->text);
    free(*lexer);
    *lexer = NULL;
}

void mcl_lexer_next(mcl_lexer_t *lexer) {
    lexer->curr_char = lexer->next_char;
    lexer->next_char = lexer->char_index < lexer->text_length
                       ? lexer->text[lexer->char_index] : 0;
    lexer->char_index++;
}

void mcl_lexer_advance(mcl_lexer_t *lexer, bool newline) {
    if (newline) {
        lexer->position.line++;
        lexer->position.column = 0;
        lexer->position.line_off = lexer->position.offset + 1;
    }

    lexer->position.column++;
    lexer->position.offset++;

    mcl_lexer_next(lexer);
}

da_t *mcl_lexer_tokenise(mcl_lexer_t *lexer) {
    da_t *tokens = new_da();
    mcl_token_t token = mcl_lexer_next_token(lexer);
    
    while (mcl_inrange (lexer)) {
        da_append (tokens, token);
        token = mcl_lexer_next_token(lexer);
    }
    
    return tokens;
}

mcl_token_t mcl_lexer_next_token(mcl_lexer_t *lexer) {
    while (mcl_inrange (lexer)) {
        if (mcl_iswhitespace (lexer->curr_char)) {
            mcl_lexer_advance(lexer, lexer->curr_char == '\n');
        } else if (mcl_islinecomment (lexer)) {
            while (mcl_inrange (lexer) && lexer->next_char != '\n') {
                mcl_lexer_advance(lexer, false);
            }
        } else if (mcl_isblockcomment (lexer)) {
            while (mcl_inrange (lexer) && 
                   lexer->curr_char != '*' && lexer->next_char != '/') {
                mcl_lexer_advance(lexer, lexer->curr_char == '\n');
            }

            mcl_lexer_advance(lexer, false);
            mcl_lexer_advance(lexer, false);
        } else {
            break;
        }
    }

    mcl_position_t position = lexer->position;
    mcl_token_kind_t kind = MCL_TOKEN_UNDEFINED;
    if (lexer->curr_char == '+') {
        kind = MCL_TOKEN_PLUS;
    } else if (lexer->curr_char == ';') {
        kind = MCL_TOKEN_SEMICOLON;
    } else if (mcl_isdigit (lexer->curr_char))  {
        while (mcl_inrange (lexer) && mcl_isdigit (lexer->next_char)) {
            mcl_lexer_advance(lexer, false);
        }
        kind = MCL_TOKEN_INTEGER;
    }

    if (kind != MCL_TOKEN_UNDEFINED) {
        mcl_lexer_advance(lexer, false);
    } else if (mcl_inrange(lexer)) {
        fprintf(stderr, "WARING: %s:%d:%d: While lexing encountred "
                "an invalid token (curr=%d, next=%d)!\n", 
                lexer->path, position.line, position.column, 
                lexer->curr_char, lexer->next_char);
    }

    return mcl_new_token (kind, mcl_new_span (position, lexer->position));
}
