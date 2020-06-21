# src/eval/repl.h

#### `char32_t *repl_read(void)`
Returns pointer to string read from commandline.

#### `const char32_t *repl_make_var(const token_t *token, context_t *ctx, bool is_const)`
Make and add a variable from passed `tokens` to context `ctx`.

Added variable will be constant if `is_const` is true.

#### `const char32_t *repl_eval(const char32_t *str, context_t *ctx)`
Evaluates a single line, returns result as a string (if any).

