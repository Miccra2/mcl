#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#include <stddef.h>
#include "dynamic_array.h"
#include "ast.h"

typedef struct {
    da_t expressions;
    size_t expr_index;
    mcl_expression_t *expr;
} mcl_interpreter_t;

mcl_interpreter_t *mcl_new_interpreter(da_t expressions);
void mcl_interpreter_step(mcl_interpreter_t *interpreter);
void mcl_interpreter_run(mcl_interpreter_t *interpreter);
mcl_value_expr_t *mcl_interpreter_eval_expression(mcl_expression_t *expr);
mcl_value_expr_t *mcl_interpreter_eval_infix_expr(mcl_infix_expr_t *expr);

#endif // __INTERPRETER_H__
