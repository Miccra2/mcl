#ifndef __PARSER_C__
#define __PARSER_C__

#include "span.c"
#include "token.c"
#include "lexer.c"
#include "ast.c"

typedef struct {
    Lexer *lexer;
    Token last_token;
    Token curr_token;
} Parser;

Parser *new_parser(Lexer *lexer);
void del_parser(Parser **parser);
void parser_next(Parser *parser);
void parser_parse(Parser *parser);
Expression *parser_value_expr(Parser *parser);
Expression *parser_infix_expr(Parser *parser, Position *start_position, 
                              Expression *left, InfixOp op);
Expression *parser_expression(Parser *parser, size_t priority);

Parser *new_parser(Lexer *lexer) {
    Parser *parser;

    parser = (Parser *)malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->last_token = default_token();
    parser->curr_token = default_token();

    parser_next(parser);

    return parser;
}

void del_parser(Parser **parser) {
    // parser->lexer has to be manualy freed
    free(*parser);
    *parser = NULL;
}

void parser_next(Parser *parser) {
    parser->last_token = parser->curr_token;
    parser->curr_token = lexer_next_token(parser->lexer);
}

Expression *parser_value_expr(Parser *parser) {
    Position start_position;
    ExprValue *expr;
    Type type;
    size_t dummy_int;
    void *value;

    start_position = parser->curr_token.span.start;
    
    expr = NULL;
    type = TYPE_UNDEFINED;
    value = NULL;

    parser_next(parser);
    switch (parser->last_token.kind) {
    case TOKEN_DIGITS:
        if (token_len(&parser->last_token) > 20) return NULL;
        dummy_int = 0;
        int e = token_make_int(parser, &parser->last_token, &dummy_int);
        if(e) return NULL;
        value = malloc(sizeof(size_t));
        *(size_t *)value = dummy_int;
        break;
    };

    expr = malloc(sizeof(ExprValue) + ast_type_size(type));
    expr->type = type;
    return new_expression(
        EXPR_VALUE, 
        new_span(start_position, parser->last_token.span.end), 
        expr
    );
}

Expression *parser_infix_expr(Parser *parser, Position *start_position,
                              Expression *left, InfixOp op) {
    size_t priority_right;
    Expression *right;
    Expression *expr;

    priority_right = token_get_priority(parser->curr_token.kind).right;
    parser_next(parser);
    right = parser_expression(parser, priority_right);
    
    expr = malloc(sizeof(Expression));
    expr->kind = EXPR_INFIX;
    expr->span = new_span(*start_position, parser->last_token.span.end);
    expr->expr = malloc(sizeof(ExprInfix));
    ((ExprInfix *)expr->expr)->op = op;
    ((ExprInfix *)expr->expr)->l_value = left;
    ((ExprInfix *)expr->expr)->r_value = right;
    
    return expr;
}

Expression *parser_expression(Parser *parser, size_t priority) {
    Position start_position;
    Expression *expr;
    
    switch (parser->curr_token.kind) {
    case TOKEN_DIGITS:
        expr = parser_value_expr(parser);
        break;
    default:
        expr = NULL;
        break;
    }

    while (token_get_priority(parser->curr_token.kind).left > priority) {
        switch (parser->curr_token.kind) {
        case TOKEN_PLUS:
            expr = parser_infix_expr(parser, 
                                     &start_position, 
                                     expr, 
                                     INFIX_PLUS);
        default:
            return NULL;
        }
    }

    return expr;
}

#endif // __PARSER_C__
