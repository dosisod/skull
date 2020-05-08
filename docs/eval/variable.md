# src/eval/variable.h

#### `variable_t *make_variable(const char *type, const char *name, size_t bytes)`
Returns a variable called `name` with type `type`, allocating `bytes` amount of bytes.

#### `void free_variable_t(variable_t *var)`
Free a given `variable_t` variable.

