# skull/parse/tokenize.c

```c
Token *tokenize(const char32_t *code)
```

> Tokenize `code`, return pointer to first token.

```c
__attribute__((const)) bool is_whitespace(char32_t c)
```

> Return true if `c` is whitespace.
> \
> Whitespace is considered as indent/line related control characters.

```c
__attribute__((const)) bool is_quote(char32_t c)
```

> Return true if `c` is a double or single quote.

```c
Token *make_token(void)
```

> Allocate and return a token with set defaults.

```c
char32_t *token_str(const Token *token)
```

> Make a heap allocated copy of the data inside `token`.
> \
> The result of this function must be freed.

```c
bool token_cmp(const char32_t *str, const Token *token)
```

> Returns true if `str` is equal to the value of `token`.

```c
__attribute__((pure)) size_t token_len(const Token *token)
```

> Return the string length of `token`.

```c
void free_tokens(Token *head)
```

> Free all tokens from `head` and beyond.

