#include "interpreter.h"

mcl_interpreter_t *mcl_new_interpreter(da_t expressions) {
    mcl_interpreter_t *interpreter = malloc(sizeof(mcl_interpreter_t));
    
    interpreter->expressions = expressions,
    interpreter->expr_index = 0,
    interpreter->expr = NULL,

    mcl_interpreter_step(interpreter);

    return interpreter;
}

void mcl_interpreter_step(mcl_interpreter_t *interpreter) {
    if (interpreter->expr_index*sizeof(mcl_expression_t) <
            interpreter->expressions.count) {
        interpreter->expr = (mcl_expression_t *)interpreter->expressions.data + 
            interpreter->expressions.count/interpreter->expr_index;
    } else {
        interpreter->expr = NULL;
    }
    interpreter->expr_index++;
}

void mcl_interpreter_run(mcl_interpreter_t *interpreter) {
    while ((interpreter->expr_index < 
            interpreter->expressions.count/sizeof(mcl_expression_t)) && 
            interpreter->expr) {
        mcl_interpreter_eval_expression(interpreter->expr);
        mcl_interpreter_step(interpreter);
    }
}

mcl_value_expr_t *mcl_interpreter_eval_expression(mcl_expression_t *expr) {
    switch (expr->kind) {
    case MCL_EXPR_INFIX:
        return mcl_interpreter_eval_infix_expr(expr->expr);
    case MCL_EXPR_VALUE:
        return (mcl_value_expr_t *)expr->expr;
    default:
        return NULL;
    }
}

mcl_value_expr_t *mcl_interpreter_eval_infix_expr(mcl_infix_expr_t *expr) {
    mcl_value_expr_t *left = mcl_interpreter_eval_expression(expr->l_value);
    mcl_value_expr_t *right = mcl_interpreter_eval_expression(expr->r_value);

    if (expr->l_value) mcl_del_expression(&expr->l_value);
    if (expr->r_value) mcl_del_expression(&expr->r_value);
    if (!(left || right)) {
        if (left) mcl_del_value_expr(&left);
        if (right) mcl_del_value_expr(&right);
        return NULL;
    }

    switch (expr->op) {
    case MCL_INFIX_PLUS:
        if (is_integer_value (left) && is_integer_value (right)) {
            (int64_t)left->value += (int64_t)right->value;
        }
    }

    return left;
}
