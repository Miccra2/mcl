#include <stdio.h>
#include <stddef.h>
#include "fmt.c"
#include "token.c"
#include "lexer.c"
#include "parser.c"

void print_tokens(Tokens *tokens, Lexer *lexer) {
    FMT *fmt;

    fmt = new_fmt();
    for (size_t i = 0; i < tokens->count; i++) {
        //printf("%s=%d,\n", TOKEN_KIND[tokens->token[i].kind], 
        //        tokens->token[i].kind);
        Token token = tokens->token[i];
        Position start = token.span.start;
        Position end = token.span.end;

        printf("'");
        for (size_t i = start.offset; i < end.offset; i++) {
            putc(lexer->text[i], stdout);
        }
        printf("' ");
        
        fmt_token(fmt, token);
        printf("\n");
    }

    del_fmt(&fmt);
}

int main(int argc, char **argv) {
    Lexer *lexer;
    Expression *expr;
    Parser *parser;
    Tokens *tokens;
    FMT *fmt;

    fmt = new_fmt();

    lexer = new_lexer("/home/miccra/projects/mcl/bootstrap/test.mcs");
    parser = new_parser(lexer);
    //tokens = lexer_tokenise(lexer);

    //print_tokens(tokens, lexer);

    expr = parser_expression(parser, 0);
    printf("kind: %d\n", expr->kind);
    printf("span: ");
    fmt_span(fmt, expr->span);
    printf("\nexpr: 0x%.16X\n", expr->expr);

    del_fmt(&fmt);
    del_lexer(&lexer);
    //del_tokens(&tokens);
    del_expression(&expr);
    del_parser(&parser);

    return 0;
}
