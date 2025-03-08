#ifndef __AST_H__
#define __AST_H__

#include <stdlib.h>

#include "lexer.h"

static char *MCL_EXPR_KIND[] = {
    "MCL_EXPR_UNDEFINED",
    "MCL_EXPR_VALUE",
    "MCL_EXPR_INFIX",
    "MCL_EXPR_COUNT",
};

typedef enum {
    MCL_EXPR_UNDEFINED,
    MCL_EXPR_VALUE,
    MCL_EXPR_INFIX,
    MCL_EXPR_COUNT,
} mcl_expr_kind_t;

typedef struct {
    mcl_expr_kind_t kind;
    mcl_span_t      span;
    void            *expr;
} mcl_expression_t;

static char *MCL_TYPE[] = {
    "MCL_TYPE_UNDEFINED",
    "MCL_TYPE_I8",
    "MCL_TYPE_I16",
    "MCL_TYPE_I32",
    "MCL_TYPE_I64",
    "MCL_TYPE_I128",
    "MCL_TYPE_ISIZE",
    "MCL_TYPE_U8",
    "MCL_TYPE_U16",
    "MCL_TYPE_U32",
    "MCL_TYPE_U64",
    "MCL_TYPE_U128",
    "MCL_TYPE_USIZE",
    "MCL_TYPE_COUNT",
};


typedef enum {
    MCL_TYPE_UNDEFINED,

    // integer types
    MCL_TYPE_I8,
    MCL_TYPE_I16,
    MCL_TYPE_I32,
    MCL_TYPE_I64,
    MCL_TYPE_I128,
    MCL_TYPE_ISIZE,
    MCL_TYPE_U8,
    MCL_TYPE_U16,
    MCL_TYPE_U32,
    MCL_TYPE_U64,
    MCL_TYPE_U128,
    MCL_TYPE_USIZE,

    MCL_TYPE_COUNT,
} mcl_type_t;

typedef struct {
    mcl_type_t type;
    void       *value;
} mcl_value_expr_t;

static char *MCL_INFIX_OP[] = {
    "MCL_INFIX_UNDEFINED",
    "MCL_INFIX_PLUS",
    "MCL_INFIX_COUNT",
};

typedef enum {
    MCL_INFIX_UNDEFINED,
    MCL_INFIX_PLUS,
    MCL_INFIX_COUNT,
} mcl_infix_op_t;

typedef struct {
    mcl_infix_op_t   op;
    mcl_expression_t *l_value;
    mcl_expression_t *r_value;
} mcl_infix_expr_t;

#define mcl_new_expression(k, s, e) \
    ((mcl_expression_t){.kind=(k), .span=(s), .expr=(e)})
#define mcl_new_value_expr(t, v) \
    ((mcl_value_expr_t){.type=(t), .value=(v)})
#define mcl_new_infix_expr(o, l, r) \
    ((mcl_infix_expr_t){.op=(o), .l_value=(l), .r_value=(r)})

void mcl_del_expression(mcl_expression_t **expr);
void mcl_del_value_expr(mcl_value_expr_t **expr);
void mcl_del_infix_expr(mcl_infix_expr_t **expr);
size_t mcl_get_type_size(mcl_type_t type);
void mcl_cast_type(mcl_value_expr_t *value, mcl_type_t type);

#endif // __AST_H__
