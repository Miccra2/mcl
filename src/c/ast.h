#ifndef __AST_H__
#define __AST_H__

#include "span.h"
#include "token.h"
#include "lexer.h"

extern const char *EXPR_KIND[];

typedef enum {
    EXPR_UNDEFINED,
    EXPR_VALUE,
    EXPR_INFIX,
    EXPR_COUNT,
} expr_kind_t;

typedef struct {
    expr_kind_t kind;
    span_t      span;
    void        *expr;
} expression_t;

extern const char *TYPE[];

typedef enum {
    TYPE_UNDEFINED,
    
    // integer types
    TYPE_I8, TYPE_I16, TYPE_I32, TYPE_I64, TYPE_I128, TYPE_ISIZE,
    TYPE_U8, TYPE_U16, TYPE_U32, TYPE_U64, TYPE_U128, TYPE_USIZE,

    TYPE_COUNT,
} type_t;

typedef struct {
    type_t type;
    char *value;
} value_expr_t;

extern const char *INFIX_OP[];

typedef enum {
    INFIX_UNDEFINED,
    INFIX_PLUS,
    INFIX_MINUS,
    INFIX_COUNT,
} infix_op_t;

typedef struct {
    infix_op_t op;
    expression_t *lvalue;
    expression_t *rvalue;
} infix_expr_t;

#define new_expression(k, s, e) ((expression_t){.kind=k, .span=s, .expr=e})
#define new_value_expr(t, v) ((value_expr_t){.type=t, .value=v})
#define new_infix_expr(o, l, r) ((infix_expr_t){.op=o, .lvalue=l, .rvalue=r})

void del_expression(expression_t **expr);
void del_value_expr(value_expr_t **expr);
void del_infix_expr(infix_expr_t **expr);

#endif // __AST_H__
