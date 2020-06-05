#pragma once

#include "../../src/common/wegex.h"
#include "../../src/common/str.h"
#include "tokenize.h"
#include "types.h"

enum token_types {
	//default token type
	TOKEN_UNKNOWN,

	//identifiers are things like variable and function names
	TOKEN_IDENTIFIER,
	TOKEN_NEW_IDENTIFIER, //must suffix new var names with ":"

	//indicates the start of a function
	TOKEN_FUNCTION,

	//keyword related tokens
	TOKEN_KW_MUT,
	TOKEN_KW_RETURN,

	TOKEN_BRACKET_OPEN,
	TOKEN_BRACKET_CLOSE,

	TOKEN_TYPE,

	//operators (add, subtract, assign)
	TOKEN_OPER_EQUAL,
	TOKEN_OPER_PLUS,

	//constant values, such as ints and strings
	TOKEN_INT_CONST,
	TOKEN_FLOAT_CONST,
	TOKEN_BOOL_CONST,
	TOKEN_CHAR_CONST,
	TOKEN_STR_CONST
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
	return wegex_match(L"\a*[\f_]?:", str);
}

/*
Return true if passed token is a valid identifier.
*/
bool is_valid_identifier_token(const token_t *token) {
	MAKE_TOKEN_BUF(buf, token);

	return is_valid_identifier(buf);
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
	else if (token_cmp(L"mut", token)) {
		token->token_type=TOKEN_KW_MUT;
	}
	else if (token_cmp(L"return", token)) {
		token->token_type=TOKEN_KW_RETURN;
	}
	else if (token_cmp(L"=", token)) {
		token->token_type=TOKEN_OPER_EQUAL;
	}
	else if (token_cmp(L"+", token)) {
		token->token_type=TOKEN_OPER_PLUS;
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
			token->token_type=TOKEN_NEW_IDENTIFIER;
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
