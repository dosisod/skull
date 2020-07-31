# skull/eval/types/str.c

#### `char32_t *fmt_str_type(const variable_t *var)`
Returns the string representation of string `var`

#### `variable_t *add_str_type(const variable_t *lhs, const variable_t *rhs)`
Concatenate `lhs` and `rhs` strings

#### `void *eval_str(const token_t *token, const char32_t **error)`
Return pointer to string, converted from `token`

