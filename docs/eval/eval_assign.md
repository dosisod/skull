# src/eval/eval_assign.h

#### `const wchar_t *eval_assign(variable_t *var, token_t *token)`
Assign the value of `token` to a variable `var`.

Return an error (as a string) if any occured, else `NULL`.

