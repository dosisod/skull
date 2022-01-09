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
	TOKEN_KW_UNREACHABLE,
	TOKEN_KW_IF,
	TOKEN_KW_ELIF,
	TOKEN_KW_ELSE,
	TOKEN_KW_WHILE,
	TOKEN_KW_EXTERNAL,
	TOKEN_KW_EXPORT,
	TOKEN_KW_NOOP,
	TOKEN_KW_BREAK,
	TOKEN_KW_CONTINUE,

	TOKEN_NEWLINE,
	TOKEN_COMMA,
	TOKEN_BRACKET_OPEN,
	TOKEN_BRACKET_CLOSE,
	TOKEN_PAREN_OPEN,
	TOKEN_PAREN_CLOSE,

	TOKEN_OPER_EQUAL,
	TOKEN_OPER_PLUS,
	TOKEN_OPER_MINUS,
	TOKEN_OPER_MULT,
	TOKEN_OPER_DIV,
	TOKEN_OPER_MOD,
	TOKEN_OPER_AUTO_EQUAL,
	TOKEN_OPER_NOT,
	TOKEN_OPER_IS,
	TOKEN_OPER_ISNT,
	TOKEN_OPER_AND,
	TOKEN_OPER_OR,
	TOKEN_OPER_XOR,
	TOKEN_OPER_LESS_THAN,
	TOKEN_OPER_GTR_THAN,
	TOKEN_OPER_LESS_THAN_EQ,
	TOKEN_OPER_GTR_THAN_EQ,
	TOKEN_OPER_LSHIFT,
	TOKEN_OPER_RSHIFT,
	TOKEN_OPER_POW,
	TOKEN_OPER_REF,

	TOKEN_INT_CONST,
	TOKEN_FLOAT_CONST,
	TOKEN_BOOL_CONST,
	TOKEN_RUNE_CONST,
	TOKEN_STR_CONST,

	TOKEN_COMMENT,

	TOKEN_END
} TokenType;

/*
Stores the line and column of a certain token/variable/function.
*/
typedef struct Location {
	unsigned line;
	unsigned column;
} Location;

/*
Stores the smallest meaningful chunk of parsed code.

`begin` points to the first character in the token.

`end` points to the last character in the token.

`type` stores the type of token.

`next` stores the next parsed token.
*/
typedef struct Token {
	const char32_t *begin;
	Token *next;

	unsigned len;
	TokenType type;
	Location location;
} Token;

Token *make_token(void);

Token *tokenize(const char32_t *);

void free_tokens(Token *);

_Bool token_cmp(const char32_t *, const Token *);

char32_t *token_to_string(const Token *);
char *token_to_mbs_str(const Token *);
