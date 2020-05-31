# src/eval/repl.h

#### `wchar_t *repl_read()`
Returns pointer to string read from commandline.

#### `const wchar_t *repl_eval(wchar_t *str, context_t *ctx)`
Evaluates a single line, returns result as a string (if any).

