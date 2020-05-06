#pragma once

#include <regex.h>

#include "types.h"

#define TOKEN_UNKNOWN 0
#define TOKEN_KEYWORD 1
#define TOKEN_BRACKET_OPEN 2
#define TOKEN_BRACKET_CLOSE 3
#define TOKEN_FUNCTION 4
#define TOKEN_FUNCTION_PARAM 5
#define TOKEN_TYPE 6
#define TOKEN_OPERATOR 7
#define TOKEN_INT_CONST 8
#define TOKEN_FLOAT_CONST 9
#define TOKEN_BOOL_CONST 10
#define TOKEN_CHAR_CONST 11
#define TOKEN_STR_CONST 12

#define TOKEN_KEYWORDS_LEN 8
const char *TOKEN_KEYWORDS[TOKEN_KEYWORDS_LEN] = {
	"return",
	"if",
	"elif",
	"else",
	"do",
	"while",
	"import",
	"for"
};

#define TOKEN_OPERATORS_LEN 28
const char *TOKEN_OPERATORS[TOKEN_OPERATORS_LEN] = {
	"+", "-", "/", "//", "*", "%", "<<", ">>", "|", "&", "^",
	"+=", "-=", "/=", "*=", "|=", "&=", "^=",
	"<", ">", "<=", ">=",
	"=", "==", "!=",
	"and", "or", "not"
};

/*
Returns true if `token` is a type token.
*/
bool is_type_token(token_t *token, const char *code) {
	type_t *current=&TYPES_AVAILABLE;
	while (current) {
		if (token_cmp(current->name, token, code)) {
			return true;
		}
		current=current->next;
	}
	return false;
}

/*
Returns true if `token` is a keyword token.
*/
bool is_keyword_token(token_t *token, const char *code) {
	for (unsigned long i=0 ; i<TOKEN_KEYWORDS_LEN ; i++) {
		if (token_cmp(TOKEN_KEYWORDS[i], token, code)) {
			return true;
		}
	}
	return false;
}

/*
Returns true if `token` is an operator token.
*/
bool is_operator_token(token_t *token, const char *code) {
	for (unsigned long i=0 ; i<TOKEN_OPERATORS_LEN; i++) {
		if (token_cmp(TOKEN_OPERATORS[i], token, code)) {
			return true;
		}
	}
	return false;
}

/*
Returns true if `token` is a function token.

Function tokens are tokens that look like `name[]`, or `name[`.
They indicate the start of a function declaration.
*/
bool is_function_token(token_t *token, const char *code) {
	int len=token_len(token);
	char buf[len + 1];

	strlcpy(buf, code + token->start, len);

	//functions can end in "[]", or have a "[" without having a "]"
	if (len>3 && buf[len - 2]=='[' && buf[len - 1]==']') {
		return true;
	}
	else if (strchr(buf, '[')!=NULL && strchr(buf, ']')==NULL) {
		return true;
	}

	return false;
}

/*
Returns true if `token` is a function parameter.

Function parameters are tokens that look like `name]`, or `name,`.
They indicate that there is a parameter for a given function.
*/
bool is_function_param_token(token_t *token, const char *code) {
	if (token_len(token)<=2) return false;

	return (code[token->end - 1]==']' || code[token->end - 1]==',');
}

regex_t int_regex;
bool int_regex_compiled=false;
/*
Returns true if string is a valid hex/binary/decimal integer.

Examples: `-123`, `123`, `0xFF`, `0xff`, `0b1101`
*/
bool is_constant_integer(const char *str) {
	if (!int_regex_compiled) {
		regcomp(&int_regex, "^(-?[0-9]+|0x[0-9a-fA-F]+|0b[10]+)$", REG_EXTENDED);
		int_regex_compiled=true;
	}

	return regexec(&int_regex, str, 0, NULL, 0)==0;
}

/*
Returns true if the passed token is an integer constant.

See above function for examples of valid inputs.
*/
bool is_constant_integer_token(token_t *token, const char *code) {
	int len=token_len(token);
	char buf[len + 1];
	strlcpy(buf, code + token->start, len);

	return is_constant_integer(buf);
}

regex_t float_regex;
bool float_regex_compiled=false;
/*
Returns true if string is a valid float (with decimal).

Examples: `123.0`, `-123.0`, `0.0`
*/
bool is_constant_float(const char *str) {
	if (!float_regex_compiled) {
		regcomp(&float_regex, "^-?[0-9]+\\.[0-9]+$", REG_EXTENDED);
		float_regex_compiled=true;
	}

	return regexec(&float_regex, str, 0, NULL, 0)==0;
}

/*
Returns true if the passed token is a float constant.

See above function for examples of valid inputs.
*/
bool is_constant_float_token(token_t *token, const char *code) {
	int len=token_len(token);
	char buf[len + 1];
	strlcpy(buf, code + token->start, len);

	return is_constant_float(buf);
}

/*
Returns true if the string is a valid bool (`true` or `false`).
*/
bool is_constant_bool(const char *str) {
	return strcmp("false", str)==0 || strcmp("true", str)==0;
}

/*
Returns true if the passed token is a boolean constant.
*/
bool is_constant_bool_token(token_t *token, const char *code) {
	int len=token_len(token);
	char buf[len + 1];
	strlcpy(buf, code + token->start, len);

	return is_constant_bool(buf);
}

/*
Returns true if the string is a valid char.

Examples: `'x'` and `' '`.
Won't work: `''`, `'x '`, or `' x'`.
*/
bool is_constant_char(const char *str) {
	return strlen(str)==3 && str[0]=='\'' && str[2]=='\'';
}

/*
Returns true if the passed token is a char constant.

Examples of valid inputs can be seen in the above function.
*/
bool is_constant_char_token(token_t *token, const char *code) {
	int len=token_len(token);
	char buf[len + 1];
	strlcpy(buf, code + token->start, len);

	return is_constant_char(buf);
}

/*
Returns true if the string is a valid string constant.

Examples: `""` and `"hello"`.
*/
bool is_constant_str(const char *str) {
	int len=strlen(str);

	return len>=2 && str[0]=='\"' && str[len - 1]=='\"';
}

/*
Returns true if the passed token is a string constant.

Examples of valid inputs can be seen in the above function.
*/
bool is_constant_str_token(token_t *token, const char *code) {
	int len=token_len(token);
	char buf[len + 1];
	strlcpy(buf, code + token->start, len);

	return is_constant_str(buf);
}

/*
Classify the token `token`.
*/
void classify_token(token_t *token, const char *code) {
	if (token_cmp("[", token, code)) {
		token->token_type=TOKEN_BRACKET_OPEN;
	}
	else if (token_cmp("]", token, code)) {
		token->token_type=TOKEN_BRACKET_CLOSE;
	}
	else if (is_keyword_token(token, code)) {
		token->token_type=TOKEN_KEYWORD;
	}
	else if (is_operator_token(token, code)) {
		token->token_type=TOKEN_OPERATOR;
	}
	else if (is_type_token(token, code)) {
		token->token_type=TOKEN_TYPE;
	}
	else if (is_function_token(token, code)) {
		token->token_type=TOKEN_FUNCTION;
	}
	else if (is_function_param_token(token, code)) {
		token->token_type=TOKEN_FUNCTION_PARAM;
	}
	else if (is_constant_integer_token(token, code)) {
		token->token_type=TOKEN_INT_CONST;
	}
	else if (is_constant_float_token(token, code)) {
		token->token_type=TOKEN_FLOAT_CONST;
	}
	else if (is_constant_bool_token(token, code)) {
		token->token_type=TOKEN_BOOL_CONST;
	}
	else if (is_constant_char_token(token, code)) {
		token->token_type=TOKEN_CHAR_CONST;
	}
	else if (is_constant_str_token(token, code)) {
		token->token_type=TOKEN_STR_CONST;
	}

	regfree(&int_regex);
	int_regex_compiled=false;

	regfree(&float_regex);
	float_regex_compiled=false;
}

/*
Starting at token `token`, go through and classify each token in linked list.
*/
void classify_tokens(token_t *head, const char *code) {
	token_t *current=head;

	while (current!=NULL) {
		classify_token(current, code);
		current=current->next;
	}
}
