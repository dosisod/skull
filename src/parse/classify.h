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
	TOKEN_KW_OR,
	TOKEN_KW_AND,
	TOKEN_KW_NOT,

	TOKEN_BRACKET_OPEN,
	TOKEN_BRACKET_CLOSE,

	TOKEN_NEWLINE,
	TOKEN_COMMA,

	TOKEN_TYPE,

	//operators (add, subtract, assign)
	TOKEN_OPER_EQUAL,
	TOKEN_OPER_PLUS,
	TOKEN_OPER_EQUAL_EQUAL,
	TOKEN_OPER_NOT_EQUAL,
	TOKEN_OPER_AUTO_EQUAL, // ":="

	//constant values, such as ints and strings
	TOKEN_INT_CONST,
	TOKEN_FLOAT_CONST,
	TOKEN_BOOL_CONST,
	TOKEN_CHAR_CONST,
	TOKEN_STR_CONST,

	//cannot be assigned to a token directly
	TOKEN_ANY_NON_BRACKET_TOKEN
};

/*
Returns true if `token` is a type token.
*/
bool is_type_str(const wchar_t *name) {
	type_t *current=&TYPES_AVAILABLE;
	while (current) {
		if (wcscmp(current->name, name)==0) {
			return true;
		}
		current=current->next;
	}
	return false;
}

/*
Returns true if a `str` is a keyword.
*/
bool is_keyword_str(const wchar_t *str) {
	return (
		wcscmp(L"return", str)==0 ||
		wcscmp(L"mut", str)==0
	);
}

/*
Returns true if string is a valid hex/octal/binary/decimal integer.

Examples: `-123`, `123`, `0xFF`, `0xff`, `0b1010`, `0o777`
*/
bool is_constant_integer_str(const wchar_t *str) {
	return (
		wegex_match(L"?-+\n", str) ||
		wegex_match(L"0x+\b", str) ||
		wegex_match(L"0b+[01]", str) ||
		wegex_match(L"0o+[01234567]", str)
	);
}

/*
Returns true if string is a valid float (with decimal).

Examples: `123.0`, `-123.0`, `0.0`
*/
bool is_constant_float_str(const wchar_t *str) {
	return wegex_match(L"?-+\n.+\n", str);
}

/*
Returns true if the string is a valid bool (`true` or `false`).
*/
bool is_constant_bool_str(const wchar_t *str) {
	return wcscmp(L"false", str)==0 || wcscmp(L"true", str)==0;
}

/*
Returns true if the string is a valid char.

Examples: `'x'` and `' '`.
Won't work: `''`, `'x '`, or `' x'`.
*/
bool is_constant_char_str(const wchar_t *str) {
	return wcslen(str)==3 && str[0]=='\'' && str[2]=='\'';
}

/*
Returns true if the string is a valid string constant.

Examples: `""` and `"hello"`.
*/
bool is_constant_str_str(const wchar_t *str) {
	const size_t len=wcslen(str);

	return len>=2 && str[0]==L'\"' && str[len - 1]==L'\"';
}

/*
Returns true the passed character the start of a valid identifier.
*/
bool is_valid_identifier_str(const wchar_t *str) {
	return wegex_match(L"\a*[\f_]?:", str);
}

/*
Classify the token `token`.
*/
void classify_token(token_t *token) {
	MAKE_TOKEN_BUF(buf, token);

	if (token_cmp(L"[", token)) {
		token->token_type=TOKEN_BRACKET_OPEN;
	}
	else if (token_cmp(L"]", token)) {
		token->token_type=TOKEN_BRACKET_CLOSE;
	}
	else if (token_cmp(L"\n", token)) {
		token->token_type=TOKEN_NEWLINE;
	}
	else if (token_cmp(L",", token)) {
		token->token_type=TOKEN_COMMA;
	}
	else if (token_cmp(L"mut", token)) {
		token->token_type=TOKEN_KW_MUT;
	}
	else if (token_cmp(L"return", token)) {
		token->token_type=TOKEN_KW_RETURN;
	}
	else if (token_cmp(L"or", token)) {
		token->token_type=TOKEN_KW_OR;
	}
	else if (token_cmp(L"and", token)) {
		token->token_type=TOKEN_KW_AND;
	}
	else if (token_cmp(L"not", token)) {
		token->token_type=TOKEN_KW_NOT;
	}
	else if (token_cmp(L"=", token)) {
		token->token_type=TOKEN_OPER_EQUAL;
	}
	else if (token_cmp(L"+", token)) {
		token->token_type=TOKEN_OPER_PLUS;
	}
	else if (token_cmp(L"==", token)) {
		token->token_type=TOKEN_OPER_EQUAL_EQUAL;
	}
	else if (token_cmp(L"!=", token)) {
		token->token_type=TOKEN_OPER_NOT_EQUAL;
	}
	else if (token_cmp(L":=", token)) {
		token->token_type=TOKEN_OPER_AUTO_EQUAL;
	}
	else if (token_cmp(L"->", token)) {
		token->token_type=TOKEN_FUNCTION;
	}
	else if (is_type_str(buf)) {
		token->token_type=TOKEN_TYPE;
	}
	else if (is_constant_integer_str(buf)) {
		token->token_type=TOKEN_INT_CONST;
	}
	else if (is_constant_float_str(buf)) {
		token->token_type=TOKEN_FLOAT_CONST;
	}
	else if (is_constant_bool_str(buf)) {
		token->token_type=TOKEN_BOOL_CONST;
	}
	else if (is_constant_char_str(buf)) {
		token->token_type=TOKEN_CHAR_CONST;

		//dont include ''s as part of string
		token->begin++;
		token->end--;
	}
	else if (is_constant_str_str(buf)) {
		token->token_type=TOKEN_STR_CONST;

		//dont include ""s as part of string
		token->begin++;
		token->end--;
	}
	else if (is_valid_identifier_str(buf)) {
		token->token_type=TOKEN_IDENTIFIER;

		if (*(token->end - 1)==L':') {
			token->token_type=TOKEN_NEW_IDENTIFIER;
			token->end--;

			MAKE_TOKEN_BUF(tmp_buf, token);
			if (is_type_str(tmp_buf) || is_keyword_str(tmp_buf)) {
				token->token_type=TOKEN_UNKNOWN;
				token->end++;
			}
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
