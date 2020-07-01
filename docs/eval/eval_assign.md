# src/eval/eval_assign.c

#### `const char32_t *eval_assign(variable_t *var, token_t *token, const context_t *ctx)`
Assign the value of `token` to a variable `var`.

Set `ctx` to allow for assigning variables to other variables.

Return an error (as a string) if any occured, else `NULL`.

