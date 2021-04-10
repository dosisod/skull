# skull/parse/token

```c
Token *tokenize(const char32_t *code)
```

> Tokenize `code`, return pointer to first token.

```c
static bool iter_comment(Token *current, const char32_t **_code, unsigned *line_num, unsigned *column)
```

> Iterate through comment, starting at `code`.
> \
> Return `true` if the caller should break (EOF was reached).

```c
static void iter_quote(Token *current, const char32_t **_code, unsigned *line_num, unsigned *column)
```

> Iterate through a quote, starting at `code`.

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
typedef struct Token {
	const char32_t *begin;
	const char32_t *end;

	TokenType type;
	unsigned line;
	unsigned column;

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
> `line` stores the line that the token starts at.
> \
> `column` stores the column that the token start of the token is in.
> \
> `next` stores the next parsed token.

