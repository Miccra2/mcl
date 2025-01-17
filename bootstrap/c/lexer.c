#ifndef __LEXER_C__
#define __LEXER_C__

#include "utils.c"
#include "token.c"

#define error(msg, pos, ...) if (pos.line == 0 && pos.column == 0) { \
        fprintf(stderr, "ERROR: "msg"\n", ##__VA_ARGS__); } else { \
        fprintf(stderr, "ERROR:l%u:c%u: "msg"\n", \
                pos.line, pos.column, ##__VA_ARGS__); } \
        exit(1);
#define waring(msg, pos, ...) if (pos.line == 0 && pos.column == 0) { \
        fprintf(stderr, "WARING: "msg"\n", ##__VA_ARGS__); } else { \
        fprintf(stderr, "WARING:l%u:c%u: "msg, \
                pos.line, pos.column, ##__VA_ARGS__); }
#define info(msg, pos, ...) if (pos.line == 0 && pos.column == 0) { \
        fprintf(stderr, "INFO: "msg"\n", ##__VA_ARGS__); } else { \
        fprintf(stderr, "INFO:l%u:c%u: "msg"\n", \
                pos.line, pos.column, ##__VA_ARGS__); }

typedef struct {
    usize    path_length;
    cString  path;
    usize    text_length;
    cString  text;
    Position pos;

    usize    char_index;
    uchar    curr_char;
    uchar    next_char;
} Lexer, *pLexer;

#define inrange(x) ((x)->char_index < (x)->text_length)
#define iswhitespace(x) ((x)->curr_char==' ' || (x)->curr_char=='\n' \
        || (x)->curr_char=='\r' || (x)->curr_char=='\t')
#define islinecomment(x) ((x)->curr_char=='/' && (x)->next_char=='/')
#define isblockcomment(x) ((x)->curr_char=='/' && (x)->next_char=='*')

#define isdigit(x) ((x)->curr_char >= '0' && (x)->curr_char <= '9')
#define isnumber(x) ((x)->curr_char == '.' || isdigit(x))
#define isalpha(x) (((x)->curr_char >= 'A' && (x)->curr_char <= 'Z') \
        || ((x)->curr_char >= 'a' && (x)->curr_char <= 'z'))
#define islabelstart(x) ((x)->curr_char == '_' || isalpha(x))
#define islabel(x) (islabelstart(x) || isdigit(x))

pLexer new_lexer(uchar *path);
void del_lexer(pLexer *lexer);
void lexer_next(pLexer lexer);
void lexer_advance(pLexer lexer);
Token lexer_next_token(pLexer lexer);
Tokens *tokenise(pLexer lexer);

pLexer new_lexer(cString path) {
    FILE *pFile;
    pLexer lexer;
    
    pFile = fopen(path, "r");
    if (!pFile) {
        error("Could not open file `%s`, please provide a valid file path!", 
                empty_pos, path);
        return NULL;
    }
    
    lexer = malloc(sizeof(Lexer));
    
    lexer->path_length = cstr_len(path);
    lexer->path = path;
    lexer->text_length = flen(pFile);
    printf("lexer->text_length: %i\n", lexer->text_length);
    lexer->text = malloc(sizeof(lexer->text_length));
    printf("lexer->text: 0x%.16X\n", lexer->text);
    lexer->pos = new_pos(1, 1, 0, 0);
    lexer->char_index = 0;
    lexer->curr_char = 0;
    lexer->next_char = 0;
    
    fread(&lexer->text, 1, lexer->text_length, pFile);

    lexer_next(lexer);
    lexer_next(lexer);

    fclose(pFile);
    return lexer;
}

void del_lexer(pLexer *lexer) {
    free((*lexer)->text);
    free(*lexer);
    *lexer = NULL;
}

void lexer_next(pLexer lexer) {
    lexer->curr_char = lexer->next_char;
    lexer->next_char = lexer->char_index < lexer->text_length ? 
        lexer->text[lexer->char_index] : 0;
    lexer->char_index++;
}

void lexer_advance(pLexer lexer) {
    if (lexer->curr_char == '\n') {
        lexer->pos.line++;
        lexer->pos.column = 0;
        lexer->pos.line_off = lexer->pos.offset + 1;
    }
    lexer->pos.column++;
    lexer->pos.offset++;
    lexer_next(lexer);
}

Token lexer_next_token(pLexer lexer) {
    bool isfloat;
    Position pos;

    // skip whitespaces, line comments and block commentes
    while (inrange(lexer)) {
        if (iswhitespace(lexer)) {
            // skip whitespaces aka: ` `, `\n`, `\r` and `\t`
            lexer_advance(lexer);
        }else if (islinecomment(lexer)) {
            // skip line comment aka: `//`
            while (inrange(lexer) && lexer->curr_char != '\n') {
                lexer_advance(lexer);
            }
        } else if (isblockcomment(lexer)) {
            // skip block comment aka: `/*` and `*/`
            while (inrange(lexer) && 
                    lexer->curr_char != '*' && 
                    lexer->next_char != '/') {
                lexer_advance(lexer);
            }

            // skip end of block comment `*/` to not get them as tokens
            lexer_advance(lexer);
            lexer_advance(lexer);
        } else {
            // no whitespace or comment found (break out of loop)
            break;
        }
    }

    // check if end for lexer->char_index is reached and return empty Token
    if (!inrange(lexer)) {
        return new_token(undefined, lexer->pos, lexer->pos);
    }
    
    // tokenise
    pos = lexer->pos;
    if (lexer->curr_char == '+') {
        lexer_advance(lexer);
        return new_token(plus, pos, lexer->pos);
    } else if (isdigit(lexer)) {
        while (inrange(lexer) && isnumber(lexer)) {
            if (lexer->curr_char == '.') {
                isfloat = true;
            }
            lexer_advance(lexer);
        }
        if (isfloat) {
            return new_token(number, pos, lexer->pos);
        }
        return new_token(digits, pos, lexer->pos);
    } else if (islabelstart(lexer)) {
        while (inrange(lexer) && islabel(lexer)) {
            lexer_advance(lexer);
        }
        return new_token(label, pos, lexer->pos);
    } else {
        error("Encountered an invalid token (%i) `%c` while parsing!",
                pos, lexer->curr_char, lexer->curr_char);
    }
}

pTokens tokenise(pLexer lexer) {
    pTokens tokens;
    Token token;

    tokens = new_tokens();
    token = lexer_next_token(lexer);
    while (inrange(lexer)) {
        printf("token (tokenise): %i", token.kind);
        tokens_append(&tokens, token);
        token = lexer_next_token(lexer);
    }

    return tokens;
}

#endif // __LEXER_C__
