#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lexer.h"
#include "ast.h"

typedef struct {
    mcl_lexer_t *lexer;
    mcl_token_t last_token;
    mcl_token_t curr_token;
} mcl_parser_t;

#define mcl_alloc_type(e, t) \
    (((mcl_value_expr_t *)(e))->value = malloc(sizeof(t)))
#define mcl_value_assign(e, t, v) \
    (*(t *)((mcl_value_expr_t *)(e))->value = (v))

mcl_parser_t *mcl_new_parser(mcl_lexer_t *lexer);
void mcl_del_parser(mcl_parser_t **parser);
void mcl_parser_next(mcl_parser_t *parser);
uint64_t mcl_parser_max_integer(mcl_type_t type);
uint64_t mcl_parser_integer(mcl_parser_t *parser, mcl_type_t type);
mcl_expression_t mcl_parser_value_expr(mcl_parser_t *parser, 
        mcl_position_t start_position, mcl_type_t type);
mcl_expression_t mcl_parser_infix_expr(mcl_parser_t *parser, 
        mcl_position_t start_position, mcl_infix_op_t op, 
        mcl_expression_t *left);
mcl_expression_t *mcl_parser_expression(
        mcl_parser_t *parser, uint8_t priority);

#endif // __PARSER_H__
