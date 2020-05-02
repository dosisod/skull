#pragma once

#include "types.h"

#define TOKEN_UNKNOWN 0
#define TOKEN_KEYWORD 1
#define TOKEN_BRACKET_OPEN 2
#define TOKEN_BRACKET_CLOSE 3
#define TOKEN_FUNCTION 4
#define TOKEN_FUNCTION_PARAM 5
#define TOKEN_TYPE 6
#define TOKEN_OPERATOR 7

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
	int len=token_len(token);
	char buf[len + 1];

	strncpyz(buf, code + token->start, len);

	type_t *current=&TYPES_AVAILABLE;

	while (current) {
		if (samestr(buf, current->name)) {
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
	int len=token_len(token);
	char buf[len + 1];

	strncpyz(buf, code + token->start, len);

	for (unsigned long i=0 ; i<TOKEN_KEYWORDS_LEN ; i++) {
		if (samestr(buf, TOKEN_KEYWORDS[i])) {
			return true;
		}
	}
	return false;
}

/*
Returns true if `token` is an operator token.
*/
bool is_operator_token(token_t *token, const char *code) {
	int len=token_len(token);
	char buf[len + 1];

	strncpyz(buf, code + token->start, len);

	for (unsigned long i=0 ; i<TOKEN_OPERATORS_LEN; i++) {
		if (samestr(buf, TOKEN_OPERATORS[i])) {
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

	strncpyz(buf, code + token->start, len);

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
	int len=token_len(token);
	char buf[len + 1];

	strncpyz(buf, code + token->start, len);

	if (len>2 && (buf[len - 1]==']' || buf[len - 1]==',')) {
		return true;
	}

	return false;
}

/*
Classify the token `token`.
*/
void classify_token(token_t *token, const char *code) {
	int len=token_len(token);
	char buf[len + 1];

	strncpyz(buf, code + token->start, len);

	if (samestr(buf, "[")) {
		token->token_type=TOKEN_BRACKET_OPEN;
	}
	else if (samestr(buf, "]")) {
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
