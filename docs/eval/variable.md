# src/eval/variable.h

#### `bool is_valid_identifier(wchar_t wc)`
Returns true the passed character the start of a valid identifier.

#### `variable_t *make_variable(const wchar_t *type, const wchar_t *name, bool is_const)`
Make a variable called `name` with type `type`, and make it const if `is_const` is true.

Returns `NULL` if var cannot be created, else pointer to created var.

#### `uint8_t variable_write(const variable_t *var, const void *data)`
Write `data` to `var`.

If `var` is const, `VARIABLE_WRITE_ECONST` is returned.
Upon success, `VARIABLE_WRITE_OK` is returned.

#### `void variable_read(void *dest, const variable_t *var)`
Read variable `var`s memory into `dest`.

#### `void free_variable(variable_t *var)`
Free a given `variable_t` variable.

