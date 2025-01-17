#include <stdio.h>

#include "utils.c"
#include "token.c"
#include "lexer.c"

int main(int argc, char **argv) {
    pLexer lexer;
    pTokens tokens;

    if (argc < 2) {
        fprintf(stderr, "ERROR: To few input arguments given, "
                "please provide more input arguments!\n");
        return 1;
    }

    lexer = new_lexer(argv[1]);
    tokens = tokenise(lexer);

    printf("token count: %i\n", tokens->count);

    Token token;
    uchar *offset;
    for (usize i = 0; i < tokens->count; i++) {
        token = tokens->tokens[i];
        printf("token: (%i) `", token.kind);
        if (token.start.offset < token.end.offset) {
            offset = token.start.offset;
            while (offset < token.end.offset) {
                putc(*offset, stdout);
                offset++;
            }
        }
        printf("\n");
    }

    del_lexer(&lexer);
    del_tokens(&tokens);
    return 0;
}
