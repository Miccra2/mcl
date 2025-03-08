#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>
#include <stdlib.h>

#include "span.h"
#include "token.h"
#include "lexer.h"
#include "ast.h"

typedef struct {
    lexer_t *lexer;
    token_t last_token;
    token_t curr_token;
} parser_t;

typedef struct {
    unsigned char left;
    unsigned char right;
} priority_t;

#define new_pririty (l, r) ((priority_t){.left=l, .right=r})

parser_t *new_parser(lexer_t *lexer);
void del_parser(parser_t **parser);
void parser_next(parser_t *parser);
expression_t parser_expression(parser_t *parser, unsigned char priority);
expression_t parser_value_expr(parser_t *parser);
expression_t parser_infix_expr(parser_t *parser, position_t start_position, 
                               infix_op_t op, expression_t *left);

#endif // __PARSER_H__
