#pragma once

#include <uchar.h>

typedef struct Token Token;

typedef enum {
	// default token type
	TOKEN_UNKNOWN,

	TOKEN_IDENTIFIER,
	TOKEN_NEW_IDENTIFIER, // must suffix new var names with ":"

	TOKEN_KW_MUT,
	TOKEN_KW_RETURN,
	TOKEN_KW_IF,
	TOKEN_KW_ELIF,
	TOKEN_KW_ELSE,
	TOKEN_KW_WHILE,
	TOKEN_KW_EXTERNAL,
	TOKEN_KW_EXPORT,

	TOKEN_NEWLINE,
	TOKEN_COMMA,
	TOKEN_BRACKET_OPEN,
	TOKEN_BRACKET_CLOSE,
	TOKEN_PAREN_OPEN,
	TOKEN_PAREN_CLOSE,

	TOKEN_TYPE,

	TOKEN_OPER_EQUAL,
	TOKEN_OPER_PLUS,
	TOKEN_OPER_MINUS,
	TOKEN_OPER_MULT,
	TOKEN_OPER_DIV,
	TOKEN_OPER_AUTO_EQUAL, // ":="
	TOKEN_OPER_NOT,
	TOKEN_OPER_IS,
	TOKEN_OPER_AND,
	TOKEN_OPER_OR,
	TOKEN_OPER_XOR,
	TOKEN_OPER_LESS_THAN,
	TOKEN_OPER_GTR_THAN,

	TOKEN_INT_CONST,
	TOKEN_FLOAT_CONST,
	TOKEN_BOOL_CONST,
	TOKEN_RUNE_CONST,
	TOKEN_STR_CONST,

	TOKEN_COMMENT,

	TOKEN_END
} TokenType;

/*
Stores the smallest meaningful chunk of parsed code.

`begin` points to the first character in the token.

`end` points to the last character in the token.

`type` stores the type of token.

`line` stores the line that the token starts at.

`column` stores the column that the token start of the token is in.

`next` stores the next parsed token.
*/
typedef struct Token {
	const char32_t *begin;
	const char32_t *end;

	TokenType type;
	unsigned line;
	unsigned column;

	Token *next;
} Token;

_Bool is_whitespace(char32_t);
_Bool is_quote(char32_t);

Token *make_token(void);

Token *tokenize(const char32_t *);

void free_tokens(Token *const);

size_t token_len(const Token *const);
_Bool token_cmp(const char32_t *const, const Token *const);

char32_t *token_str(const Token *const);
char *token_mbs_str(const Token *const);
