#include "parser.h"

mcl_parser_t *mcl_new_parser(mcl_lexer_t *lexer) {
    mcl_parser_t *parser = malloc(sizeof(mcl_parser_t));

    parser->lexer = lexer;
    parser->last_token = mcl_new_token (
            MCL_TOKEN_UNDEFINED, 
            mcl_new_span (mcl_new_position (1, 1, 0, 0), 
                          mcl_new_position (1, 1, 0, 0)));
    parser->curr_token = mcl_new_token (
            MCL_TOKEN_UNDEFINED,
            mcl_new_span (mcl_new_position (1, 1, 0, 0),
                          mcl_new_position (1, 1, 0, 0)));

    mcl_parser_next(parser);

    return parser;
}

void mcl_del_parser(mcl_parser_t **parser) {
    // parser->lexer has to be manually freed
    free(*parser);
    *parser = NULL;
}

void mcl_parser_next(mcl_parser_t *parser) {
    parser->last_token = parser->curr_token;
    parser->curr_token = mcl_lexer_next_token(parser->lexer);
}

uint64_t mcl_parser_max_integer(mcl_type_t type) {
    switch (type) {
    case MCL_TYPE_I8:    return 0x7Full;
    case MCL_TYPE_I16:   return 0x7FFFull;
    case MCL_TYPE_I32:   return 0x7FFFFFFFull;
    case MCL_TYPE_I64:   return 0x7FFFFFFFFFFFFFFFull;
    //case MCL_TYPE_I128:  return 0;
    //case MCL_TYPE_ISIZE: return 0;
    case MCL_TYPE_U8:    return 0xFFull;
    case MCL_TYPE_U16:   return 0xFFFFull;
    case MCL_TYPE_U32:   return 0xFFFFFFFFull;
    case MCL_TYPE_U64:   return 0xFFFFFFFFFFFFFFFFull;
    //case MCL_TYPE_U128:  return 0;
    //case MCL_TYPE_USIZE: return 0;
    default:
        return 0;
    }
}

uint64_t mcl_parser_integer(mcl_parser_t *parser, 
                            mcl_type_t type) {
    uint64_t value = 0;
    size_t ptr = parser->curr_token.span.end.offset - 1;
    uint64_t max_value = mcl_parser_max_integer(type);
    
    mcl_parser_next(parser);

    while (ptr > parser->last_token.span.start.offset - 1) {
        value *= 10;
        value += parser->lexer->text[ptr] - 0x30;
        ptr--;
    }
    
    return value;
}

mcl_expression_t mcl_parser_value_expr(mcl_parser_t *parser, 
                                       mcl_position_t start_position, 
                                       mcl_type_t type) {
    mcl_expression_t expr = mcl_new_expression (MCL_EXPR_VALUE, 
            mcl_new_span(start_position, parser->curr_token.span.end), 
            NULL);
    
    expr.expr = malloc(sizeof(mcl_value_expr_t));
    *(mcl_value_expr_t *)expr.expr = mcl_new_value_expr (type, NULL);

    switch (type) {
    case MCL_TYPE_I8:    case MCL_TYPE_U8:
        mcl_alloc_type (expr.expr, int8_t);
        mcl_value_assign (expr.expr, 
                          int8_t, 
                          mcl_parser_integer(parser, type));
        break;
    case MCL_TYPE_I16:   case MCL_TYPE_U16:
        mcl_alloc_type (expr.expr, int16_t);
        mcl_value_assign (expr.expr, 
                          int16_t, 
                          mcl_parser_integer(parser, type));
        break;
    case MCL_TYPE_I32:   case MCL_TYPE_U32:
        mcl_alloc_type (expr.expr, int32_t);
        mcl_value_assign (expr.expr,
                          int32_t,
                          mcl_parser_integer(parser, type));
        break;
    case MCL_TYPE_I64:   case MCL_TYPE_U64:
        mcl_alloc_type (expr.expr, int64_t);
        mcl_value_assign (expr.expr,
                          int64_t,
                          mcl_parser_integer(parser, type));
        
        break;
    //case MCL_TYPE_I128:  case MCL_TYPE_U128:
    //case MCL_TYPE_ISIZE: case MCL_TYPE_USIZE:
    }

    return expr;
}

mcl_expression_t mcl_parser_infix_expr(mcl_parser_t *parser, 
                                       mcl_position_t start_position, 
                                       mcl_infix_op_t op, 
                                       mcl_expression_t *left) {
    uint8_t right_priority = 
        mcl_token_get_priority(parser->curr_token.kind).right;
    
    mcl_parser_next(parser);
    
    mcl_expression_t *right = mcl_parser_expression(parser, right_priority);
    mcl_expression_t *tmp_left = malloc(sizeof(mcl_expression_t));
    mcl_infix_expr_t *expr = malloc(sizeof(mcl_infix_expr_t));
    
    tmp_left->kind = left->kind;
    tmp_left->span = left->span;
    tmp_left->expr = left->expr;
    
    *expr = mcl_new_infix_expr (op, tmp_left, right);

    return mcl_new_expression (MCL_EXPR_INFIX, 
                               mcl_new_span (start_position, 
                                             parser->last_token.span.end),
                               expr);
}

mcl_expression_t *mcl_parser_expression(mcl_parser_t *parser, 
                                        uint8_t priority) {
    mcl_position_t start_position = parser->curr_token.span.start; 
    mcl_expression_t *expr = malloc(sizeof(mcl_expression_t));
    mcl_expr_kind_t kind = MCL_EXPR_UNDEFINED;
    
    switch (parser->curr_token.kind) {
    case MCL_TOKEN_INTEGER:
        // try to convert integer into giggest posible type (u64)
        // type check has to solve problems later if value is to big
        *expr = mcl_parser_value_expr(parser, start_position, MCL_TYPE_U64);
        break;
    }

    bool run = true;
    while (run && (
           mcl_token_get_priority(parser->curr_token.kind).left > priority)) {
        switch (parser->curr_token.kind) {
        case MCL_TOKEN_PLUS:
            *expr = mcl_parser_infix_expr(
                    parser, start_position, MCL_INFIX_PLUS, expr);
            break;
        case MCL_TOKEN_SEMICOLON:
            mcl_lexer_next(parser->lexer);
            run = false;
            break;
        }
    }
    
    return expr;
}
