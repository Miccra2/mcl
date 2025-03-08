#include "ast.h"

void mcl_del_expression(mcl_expression_t **expr) {
    switch ((*expr)->kind) {
    case MCL_EXPR_VALUE:
        mcl_del_value_expr((mcl_value_expr_t **)&(*expr)->expr);
        break;
    case MCL_EXPR_INFIX:
        mcl_del_infix_expr((mcl_infix_expr_t **)&(*expr)->expr);
        break;
    default:
        if (!(*expr)->expr)
            free((*expr)->expr);
        break;
    }
    free(*expr);
    *expr = NULL;
}

void mcl_del_value_expr(mcl_value_expr_t **expr) {
    if ((*expr)->value)
        free((*expr)->value);
    free(*expr);
    *expr = NULL;
}

void mcl_del_infix_expr(mcl_infix_expr_t **expr) {
    if ((*expr)->l_value)
        mcl_del_expression((mcl_expression_t **)&(*expr)->l_value);
    if ((*expr)->r_value)
        mcl_del_expression((mcl_expression_t **)&(*expr)->r_value);
    free(*expr);
    *expr = NULL;
}

size_t mcl_get_type_size(mcl_type_t type) {
    switch (type) {
    case MCL_TYPE_I8:  case MCL_TYPE_U8:  return 1;
    case MCL_TYPE_I16: case MCL_TYPE_U16: return 2;
    case MCL_TYPE_I32: case MCL_TYPE_U32: return 4;
    case MCL_TYPE_I64: case MCL_TYPE_U64: return 8;
    default: return 0;
    }
}

void mcl_cast_type(mcl_value_expr_t *expr, mcl_type_t type) {
    void *value = expr->value;
    size_t old_size = mcl_get_type_size(expr->type);
    size_t new_size = mcl_get_type_size(type);
    
    expr->type = type;
    expr->value = malloc(new_size);
    
    size_t i;
    size_t old_off;
    size_t new_off;
    if (new_size > old_size) {
        i = new_size - 1;
        old_off = old_size - new_size;
        new_off = 0;
    } else {
        i = old_size - 1;
        old_off = 0;
        new_off = new_size - old_size;
    }
    for (; i >= 0; i--) {
        ((char *)expr->value)[new_off+i] = ((char *)value)[old_off+i];
    }
    
    free(value);
}
