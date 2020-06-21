# src/parse/tokenize.h

#### `bool is_whitespace(char32_t c)`
Return true if `c` is whitespace.

Whitespace includes control-characters, non-printable characters, and spaces.

#### `bool is_quote(char32_t c)`
Return true if `c` is a double or single quote.

#### `token_t *make_token(void)`
Allocate and return a token with set defaults.

#### `token_t *tokenize(const char32_t *code)`
Tokenize the passed code, returning the head to a linked list of tokens.

#### `void free_tokens(token_t *head)`
Free all the tokens from a linked list of tokens.

This function should be called at the head of the linked list.

#### `size_t token_len(const token_t *token)`
Returns the character length of the passed token

#### `bool token_cmp(const char32_t *str, const token_t *token)`
Returns true if `str` is equal to the value of `token`.

#### `char32_t *token_str(const token_t *token)`
Return a copy of the string data inside `token`.

