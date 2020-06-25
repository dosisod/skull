#pragma once

#include "../parse/tokenize.h"

/*
Returns `true` or `false` if token is `"true"` or `"false"`.

`error` is `NULL` if no error occurs, else `error` points to error msg.
*/
bool eval_bool_true(const token_t *token, const char32_t **error);

/*
Return the result of an equality comparison.

Examples include:

```
"true == false"
"true != true"
"123 == 123"
"3.14 == 3.14"
```

If an error occurs on either side, `error` is set to non `NULL`.
*/
bool eval_equality_comparison(const token_t *token, const char32_t **error);

/*
Resolve a boolean expression from string into `true` or `false`.

Examples include:

```
"true"
"false"
"true == false"
"false != false"
"not true"
```

If an error an occurs, `error` is set to non `NULL`.
*/
bool eval_bool(const token_t *token, const char32_t **error);