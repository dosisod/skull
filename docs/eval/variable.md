# src/eval/variable.h

#### `variable_t *make_variable(const wchar_t *type, const wchar_t *name, bool is_const)`
Make a variable called `name` with type `type`, and make it const if `is_const` is true.

Returns NULL if var cannot be created, else pointer to created var.

#### `void free_variable_t(variable_t *var)`
Free a given `variable_t` variable.

