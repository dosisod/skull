# src/eval/eval_assign.c

#### `const char32_t *eval_assign(variable_t *var, ast_node_t *node, const context_t *ctx)`
Assign `node` to variable `var`.

Set `ctx` to allow for assigning variables to other variables.

Return an error (as a string) if any occured, else `NULL`.

#### `const char32_t *eval_auto_assign(variable_t *var, ast_node_t *node, const context_t *ctx)`
Evaluate assignment via auto assignment operator.

#### `const char32_t *eval_add_var_assign(variable_t *var, ast_node_t *node, const context_t *ctx)`
Evaluate assignment via adding of 2 variables.

#### `const char32_t *eval_sub_var_assign(variable_t *var, ast_node_t *node, const context_t *ctx)`
Evaluate assignment via subtracint 2 variables.

