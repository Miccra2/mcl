#ifndef __AST_C__
#define __AST_C__

#include <stddef.h>
#include <assert.h>
#include "span.c"

typedef struct {
    size_t length;
    char *data;
} String;

typedef enum {
    EXPR_UNDEFINED,
    EXPR_VALUE,
    EXPR_INFIX,
    EXPR_COUNT,
} ExprKind;

typedef enum {
    TYPE_UNDEFINED,

    // integer types
    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64,
    TYPE_I128,
    TYPE_ISIZE,
    TYPE_U8,
    TYPE_U16,
    TYPE_U32,
    TYPE_U64,
    TYPE_U128,
    TYPE_USIZE,
    TYPE_COUNT,
} Type;

typedef enum {
    INFIX_UNDEFINED,
    INFIX_PLUS,
    INFIX_COUNT,
} InfixOp;

typedef struct {
    Type type;
    void *value;
} ExprValue;
    
typedef struct {
    InfixOp op;
    void *l_value;
    void *r_value;
} ExprInfix;

typedef struct {
    ExprKind kind;
    Span span;
    void *expr;
} Expression;

ExprValue *new_value_expr(Type type, void *value);
ExprInfix *new_infix_expr(InfixOp op, void *l_value, void *r_value);
Expression *new_expression(ExprKind kind, Span span, void *expr);
void del_value_expr(ExprValue **expr);
void del_infix_expr(ExprInfix **expr);
void del_expression(Expression **expr);

ExprValue *new_value_expr(Type type, void *value) {
    ExprValue *expr;
    expr = malloc(sizeof(ExprValue));
    expr->type = type;
    expr->value = value;
    return expr;
}

void del_value_expr(ExprValue **expr) {
    if ((*expr)->value) free((*expr)->value);
    free(*expr);
    *expr = NULL;
}

ExprInfix *new_infix_expr(InfixOp op, void *l_value, void *r_value) {
    ExprInfix *expr;
    expr = malloc(sizeof(ExprInfix));
    expr->op = op;
    expr->l_value = l_value;
    expr->r_value = r_value;
    return expr;
}

void del_infix_expr(ExprInfix **expr) {
    assert(INFIX_COUNT==2);
    if ((*expr)->l_value) del_expression((Expression **)&(*expr)->l_value);
    if ((*expr)->r_value) del_expression((Expression **)&(*expr)->r_value);
    free(*expr);
    *expr = NULL;
}

Expression *new_expression(ExprKind kind, Span span, void *inner) {
    Expression *expr;

    expr = malloc(sizeof(Expression));
    expr->kind = kind;
    expr->span = span;
    expr->expr = inner;

    return expr;
}

void del_expression(Expression **expr) {
    assert(EXPR_COUNT==3);
    if ((*expr)->expr) {
        switch ((*expr)->kind) {
        case EXPR_VALUE:
            del_value_expr((ExprValue **)&(*expr)->expr);
            break;
        case EXPR_INFIX:
            del_infix_expr((ExprInfix **)&(*expr)->expr);
            break;
        }
    }
    free(*expr);
    *expr = NULL;
}

size_t ast_type_size(Type type) {
    switch (type) {
    case TYPE_I8: case TYPE_U8:
        return 1;
    case TYPE_I16: case TYPE_U16:
        return 2;
    case TYPE_I32: case TYPE_U32:
        return 4;
    case TYPE_I64: case TYPE_U64:
        return 8;
    case TYPE_I128: case TYPE_U128:
        return 16;
    }
    return 0;
}

#endif // __AST_C__
