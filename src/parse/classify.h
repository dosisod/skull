#pragma once

#include "../../src/common/wegex.h"
#include "../../src/common/str.h"
#include "tokenize.h"
#include "types.h"

#define TOKEN_UNKNOWN 0
#define TOKEN_KEYWORD 1
#define TOKEN_BRACKET_OPEN 2
#define TOKEN_BRACKET_CLOSE 3
#define TOKEN_TYPE 4
#define TOKEN_FUNCTION 5
#define TOKEN_OPERATOR 6
#define TOKEN_INT_CONST 7
#define TOKEN_FLOAT_CONST 8
#define TOKEN_BOOL_CONST 9
#define TOKEN_CHAR_CONST 10
#define TOKEN_STR_CONST 11
#define TOKEN_IDENTIFIER 12

const wchar_t *TOKEN_KEYWORDS[] = {
	L"return",
	L"if",
	L"elif",
	L"else",
	L"do",
	L"while",
	L"import",
	L"for",
	L""
};

const wchar_t *TOKEN_OPERATORS[] = {
	L"+", L"-", L"/", L"//", L"*", L"%", L"<<", L">>", L"|", L"&", L"^",
	L"+=", L"-=", L"/=", L"*=", L"|=", L"&=", L"^=",
	L"<", L">", L"<=", L">=",
	L"=", L"==", L"!=",
	L"and", L"or", L"not",
	L""
};

/*
Returns true if `token` is a type token.
*/
bool is_type_token(const token_t *token) {
	type_t *current=&TYPES_AVAILABLE;
	while (current) {
		if (token_cmp(current->name, token)) {
			return true;
		}
		current=current->next;
	}
	return false;
}

/*
Returns true if `token` is a keyword token.
*/
bool is_keyword_token(const token_t *token) {
	const wchar_t **head=TOKEN_KEYWORDS;
	while (*head[0]!=L'\0') {
		if (token_cmp(*head, token)) {
			return true;
		}
		head++;
	}
	return false;
}

/*
Returns true if `token` is an operator token.
*/
bool is_operator_token(const token_t *token) {
	const wchar_t **head=TOKEN_OPERATORS;
	while (*head[0]!=L'\0') {
		if (token_cmp(*head, token)) {
			return true;
		}
		head++;
	}
	return false;
}

/*
Returns true if a token is a function token.
*/
bool is_function_token(const token_t *token) {
	return token_cmp(L"->", token);
}

/*
Returns true if string is a valid hex/binary/decimal integer.

Examples: `-123`, `123`, `0xFF`, `0xff`, `0b1010`
*/
bool is_constant_integer(const wchar_t *str) {
	return (
		wegex_match(L"?-+\n", str) ||
		wegex_match(L"0x+\b", str) ||
		wegex_match(L"0b+[01]", str)
	);
}

/*
Returns true if the passed token is an integer constant.

See above function for examples of valid inputs.
*/
bool is_constant_integer_token(const token_t *token) {
	MAKE_TOKEN_BUF(buf, token);

	return is_constant_integer(buf);
}

/*
Returns true if string is a valid float (with decimal).

Examples: `123.0`, `-123.0`, `0.0`
*/
bool is_constant_float(const wchar_t *str) {
	return wegex_match(L"?-+\n.+\n", str);
}

/*
Returns true if the passed token is a float constant.

See above function for examples of valid inputs.
*/
bool is_constant_float_token(const token_t *token) {
	MAKE_TOKEN_BUF(buf, token);

	return is_constant_float(buf);
}

/*
Returns true if the string is a valid bool (`true` or `false`).
*/
bool is_constant_bool(const wchar_t *str) {
	return wcscmp(L"false", str)==0 || wcscmp(L"true", str)==0;
}

/*
Returns true if the passed token is a boolean constant.
*/
bool is_constant_bool_token(const token_t *token) {
	MAKE_TOKEN_BUF(buf, token);

	return is_constant_bool(buf);
}

/*
Returns true if the string is a valid char.

Examples: `'x'` and `' '`.
Won't work: `''`, `'x '`, or `' x'`.
*/
bool is_constant_char(const wchar_t *str) {
	return wcslen(str)==3 && str[0]=='\'' && str[2]=='\'';
}

/*
Returns true if the passed token is a char constant.

Examples of valid inputs can be seen in the above function.
*/
bool is_constant_char_token(const token_t *token) {
	MAKE_TOKEN_BUF(buf, token);

	return is_constant_char(buf);
}

/*
Returns true if the string is a valid string constant.

Examples: `""` and `"hello"`.
*/
bool is_constant_str(const wchar_t *str) {
	const size_t len=wcslen(str);

	return len>=2 && str[0]==L'\"' && str[len - 1]==L'\"';
}

/*
Returns true if the passed token is a string constant.

Examples of valid inputs can be seen in the above function.
*/
bool is_constant_str_token(const token_t *token) {
	MAKE_TOKEN_BUF(buf, token);

	return is_constant_str(buf);
}

/*
Returns true the passed character the start of a valid identifier.
*/
bool is_valid_identifier(const wchar_t *str) {
	return (
		(*str>=L'A' && *str<=L'Z') ||
		(*str>=L'a' && *str<=L'z')
	);
}

/*
Return true if passed token is a valid identifier.
*/
bool is_valid_identifier_token(const token_t *token) {
	return is_valid_identifier(token->begin);
}

/*
Classify the token `token`.
*/
void classify_token(token_t *token) {
	if (token_cmp(L"[", token)) {
		token->token_type=TOKEN_BRACKET_OPEN;
	}
	else if (token_cmp(L"]", token)) {
		token->token_type=TOKEN_BRACKET_CLOSE;
	}
	else if (is_keyword_token(token)) {
		token->token_type=TOKEN_KEYWORD;
	}
	else if (is_operator_token(token)) {
		token->token_type=TOKEN_OPERATOR;
	}
	else if (is_type_token(token)) {
		token->token_type=TOKEN_TYPE;
	}
	else if (is_function_token(token)) {
		token->token_type=TOKEN_FUNCTION;
	}
	else if (is_constant_integer_token(token)) {
		token->token_type=TOKEN_INT_CONST;
	}
	else if (is_constant_float_token(token)) {
		token->token_type=TOKEN_FLOAT_CONST;
	}
	else if (is_constant_bool_token(token)) {
		token->token_type=TOKEN_BOOL_CONST;
	}
	else if (is_constant_char_token(token)) {
		token->token_type=TOKEN_CHAR_CONST;
	}
	else if (is_constant_str_token(token)) {
		token->token_type=TOKEN_STR_CONST;
	}
	else if (is_valid_identifier_token(token)) {
		token->token_type=TOKEN_IDENTIFIER;

		if (*(token->end - 1)==L':') {
			token->end--;
		}
	}
}

/*
Starting at token `token`, go through and classify each token in linked list.
*/
void classify_tokens(token_t *head) {
	token_t *current=head;

	while (current!=NULL) {
		classify_token(current);
		current=current->next;
	}
}
