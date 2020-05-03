# src/parse/tokenize.h

#### `bool is_whitespace(char c)`
Return true if `c` is whitespace.

Whitespace includes control-characters, non-printable characters, and spaces.

#### `bool is_quote(char c)`
Return true if `c` is a double or single quote.

#### `token_t *tokenize(const char *code)`
Tokenize the passed code, returning the head to a linked list of tokens.

#### `void free_tokens(token_t *head)`
Free all the tokens from a linked list of tokens.

This function should be called at the head of the linked list.

#### `int token_len(token_t *token)`
Returns the character length of the passed token

#### `bool token_cmp(const char* str, token_t *token, const char *code)`
Returns true if `str` is equal to the value of `token`.

