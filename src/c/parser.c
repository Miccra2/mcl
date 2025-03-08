#include "parser.h"

parser_t *new_parser(lexer_t *lexer) {
    parser_t *parser = malloc(sizeof(parser_t));
    if (!parser) return NULL;

    parser->lexer = lexer;
    parser->curr_token = new_token (TOKEN_UNDEFINED, 
            new_span (new_token (1, 1, 0, 0), new_token (1, 1, 0, 0)));
    parser->next_token = new_token (TOKEN_UNDEFINED,
            new_span (new_token (1, 1, 0, 0), new_token (1, 1, 0, 0)));

    parser_next(parser);
    return parser;
}

void del_paresr(parser_t **parser) {
    // parser->lexer has to be manually freed
    free(*parser);
    *parser = NULL;
}

void parser_next(parser_t *parser) {
    parser->last_token = parser->curr_token;
    parser->curr_token = lexer_next_token(parser->lexer);
}

expression_t parser_expression(parser_t *parser, unsigned char priority) {
    position_t start_position = parser->curr_token.span.start;
    expression_t left;
    
    switch (parser->curr_token.kind) {
    case TOKEN_INTEGER:
        left = parser_value_expr(parser);
    }

    while (get_priority(parser->curr_token.kind).right > priority) {
        switch (parser->curr_token.kind) {
        case TOKEN_PLUS:
            left = parser_infix_expr(
                    parser, start_position, INFIX_PLUS, &left);
        }
    }

    return left;
}

int64_t parser_i64(parser_t *parser) {
    parser_next(parser);

    if (parser->last_token.kind != TOKEN_INTEGER) {
        fprintf(
            stderr, 
            "ERROR:Parser:%d:%d:%s: Encountered an invalid token "
            "while parsing i64!\n", 
            parser->last_token.span.line, 
            parser->last_token.span.column, 
            parser->lexer->path
        );
        return 0;
    }

    int64_t sign = 1;
    if (parser->last_token.kind == TOKEN_MINUS) {
        num *= -1;
    }
    
    int64_t num = 0;
    for (size_t offset = parser->last_token.span.end.offset;
            offset >= parser->last_token.span.start.offset; offset++) {
        num *= 10;
        num += parser->lexer->text[offset] - 0x30;
    }

    return num * sign;
}

expression_t parser_value_expr(parser_t *parser, position_t *start_position) {
    value_expr_t value = new_value_expr (
        TYPE_UNDEFINED,
        NULL,
    );

    switch (parser->last_token.kind) {
    case TOKEN_INTEGER:
        value.value = malloc(sizeof(int64_t));
        *value.value = parser_i64(parser);
    }

    expression_t expr = new_expression (
        EXPR_VALUE,
        new_span (start_position, parser->last_token.span.end),
        malloc(sizeof(value_expr_t))
    );

    *expr->expr = value;
    return expr;
}

expression_t parser_infix_expr(parser_t *parser, position_t start_position, 
                               infix_op_t op, expression_t *left) {
    unsigned char right_priority = get_priority(parser->curr_token.kind).right;
    parser_next(parser);
    expression_t right = parser_expression(parser, right_priority);
    
    infix_expr_t infix = new_infix_expr (
        op, 
        malloc(sizeof(expression_t)),
        malloc(sizeof(expression_t))
    );

    *infix->lvalue = *left; 
    *infix->rvalue = right;

    expression_t expr = new_expression (
        EXPR_INFIX, 
        new_span (start_position, parser->last_token.span.end),
        malloc(sizeof(infix_expr_t));
    );

    *expr->expr = infix;
    return expr;
}
