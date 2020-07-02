# src/eval/repl.c

#### `char32_t *repl_read(FILE *fd)`
Returns pointer to UTF-32 string read from file descriptor `fd`.

#### `const char32_t *repl_make_var(const token_t *token, context_t *ctx, bool is_const)`
Make and add a variable from passed `tokens` to context `ctx`.

Added variable will be constant if `is_const` is true.

#### `const char32_t *repl_eval(const char32_t *str, context_t *ctx)`
Evaluates a string `str` given context `ctx`, returns result as a string (if any).

#### `void repl_loop(FILE *fd, context_t *ctx)`
Read from `fd`, eval with context `ctx`, and print out result.

