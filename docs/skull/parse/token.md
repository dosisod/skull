# skull/parse/token

```c
static Token *setup_next(Token *token)
```

> Allocate and append next token, return newly created token.

```c
Token *tokenize(const char32_t *code)
```

> Tokenize `code` into linked list of tokens.

```c
static bool iter_comment(Token *token, const char32_t **_code, unsigned *line_num, unsigned *column)
```

> Iterate through comment, starting at `code`.
> \
> Return `true` if errors occurred.

```c
static bool iter_quote(Token *token, const char32_t **_code, unsigned *line_num, unsigned *column)
```

> Iterate through a quote, starting at `code`.
> \
> Return `true` if errors occurred.

```c
static __attribute__((const)) bool is_whitespace(char32_t c)
```

> Return true if `c` is whitespace.
> \
> Whitespace is considered as indent/line related control characters.

```c
static __attribute__((const)) bool is_quote(char32_t c)
```

> Return true if `c` is a double or single quote.

```c
Token *make_token(void)
```

> Allocate and return a token with set defaults.

```c
char32_t *token_str(const Token *const token)
```

> Make a heap allocated copy of the data inside `token`.
> \
> The result of this function must be freed.

```c
char *token_mbs_str(const Token *const token)
```

> Make a heap allocated copy of the data inside `token` as a multi-byte string.
> \
> The result of this function must be freed.

```c
bool token_cmp(const char32_t *const str, const Token *const token)
```

> Returns true if `str` is equal to the value of `token`.

```c
__attribute__((pure)) size_t token_len(const Token *const token)
```

> Return the string length of `token`.

```c
void free_tokens(Token *head)
```

> Free all tokens from `head` and beyond.

```c
typedef struct Location {
	unsigned line;
	unsigned column;
}
```

> Stores the line and column of a certain token/variable/function.

```c
typedef struct Token {
	const char32_t *begin;
	const char32_t *end;

	TokenType type;
	Location location;

	Token *next;
}
```

> Stores the smallest meaningful chunk of parsed code.
> \
> `begin` points to the first character in the token.
> \
> `end` points to the last character in the token.
> \
> `type` stores the type of token.
> \
> `next` stores the next parsed token.

