# src/eval/variable.h

#### `variable_t *make_variable(const char *type, const char *name)`
Make a variable called `name` with type `type`.

Returns NULL if var cannot be created, else pointer to created var.

#### `void free_variable_t(variable_t *var)`
Free a given `variable_t` variable.

