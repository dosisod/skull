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
static bool tokenize_inner_loop(TokenizeCtx *ctx)
```

> Run one iteration of the main tokenizer function.
> \
> Returns `true` if en error occurred.

```c
static bool iter_comment(TokenizeCtx *ctx)
```

> Iterate through comment.
> \
> Return `true` if errors occurred.

```c
static bool iter_quote(TokenizeCtx *ctx)
```

> Iterate through a quote.
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
char32_t *token_to_string(const Token *token)
```

> Make a heap allocated copy of the data inside `token`.
> \
> The result of this function must be freed.

```c
char *token_to_mbs_str(const Token *token)
```

> Make a heap allocated copy of the data inside `token` as a multi-byte string.
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

