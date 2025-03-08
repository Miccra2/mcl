#include "main.h"

void expr_info(mcl_expression_t *expr, size_t level);
void infix_info(mcl_infix_expr_t *expr, size_t level);
void value_info(mcl_value_expr_t *expr);

void token_info(mcl_lexer_t *lexer) {
    da_t *tokens = mcl_lexer_tokenise(lexer);
    printf("tokens: %d\n", (int)tokens->count);
    
    for (size_t i = 0; i < tokens->count; i++) {
        mcl_token_t token = ((mcl_token_t *)tokens->data)[i];
        printf("token_kind: %d, ", (int)token.kind);
        mcl_fmt_span (token.span);
        printf("\n");
    }
    
    del_da(&tokens);
}

void infix_info(mcl_infix_expr_t *expr, size_t level) {
    printf(" (op: %s=%d)\n", MCL_INFIX_OP[expr->op], expr->op);
    expr_info(expr->l_value, level);
    expr_info(expr->r_value, level);
}

void value_info(mcl_value_expr_t *expr) {
    printf(" (type:%s=%d, value=%.16X)\n", 
            MCL_TYPE[expr->type], expr->type, *(size_t *)expr->value);
}

void expr_info(mcl_expression_t *expr, size_t level) {
    printf("%*s%s=%d", level*4, "", MCL_EXPR_KIND[expr->kind], expr->kind);
    switch (expr->kind) {
    case MCL_EXPR_INFIX:
        infix_info(expr->expr, level+1);
    case MCL_EXPR_VALUE:
        value_info(expr->expr);
    }
    printf("\n");
}

int main() {
    mcl_lexer_t *lexer = mcl_new_lexer("test.mcs");
    if (!lexer) return 1;
    
    mcl_parser_t *parser = mcl_new_parser(lexer);
    if (!parser) return 1;

    mcl_expression_t *expr = mcl_parser_expression(parser, 0);
    expr_info(expr, 0);
    
    mcl_del_expression(&expr);
    mcl_del_parser(&parser);
    mcl_del_lexer(&lexer);
    return 0;
}
