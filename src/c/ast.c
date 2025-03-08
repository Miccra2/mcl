#include "ast.h"

const char *EXPR_KIND[] = {
    "EXPR_UNDEFINED",
    "EXPR_VALUE",
    "EXPR_INFIX",
    "EXPR_COUNT",
};

void del_expression(expression_t **expr) {
    if ((*expr)->expr) {
        switch ((*expr)->kind) {
        case EXPR_VALUE:
            del_value_expr((value_expr_t **)(&(*expr)->expr));
        case EXPR_INFIX:
            del_infix_expr((infix_expr_t **)(&(*expr)->expr));
        }
    }
    free(*expr);
    *expr = NULL;
}

const char *TYPE[] = {
    "TYPE_UNDEFINED",
    
    // integer types
    "TYPE_I8", "TYPE_I16", "TYPE_I32", "TYPE_I64", "TYPE_I128", "TYPE_ISIZE",
    "TYPE_U8", "TYPE_U16", "TYPE_U32", "TYPE_U64", "TYPE_U128", "TYPE_USIZE",
    
    "TYPE_COUNT",
};

void del_value_expr(value_expr_t **expr) {
    free((*expr)->value);
    free(*expr);
    *expr = NULL;
}

const char *INFIX_OP[] = {
    "INFIX_UNDEFINED",
    "INFIX_PLUS",
    "INFIX_MINUS",
    "INFIX_COUNT",
};

void del_infix_expr(infix_expr_t **expr) {
    if ((*expr)->lvalue) del_expression(&(*expr)->lvalue);
    if ((*expr)->rvalue) del_expression(&(*expr)->rvalue);
    free(*expr);
    *expr = NULL;
}
