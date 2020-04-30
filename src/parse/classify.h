#include "types.h"

#define TOKEN_UNKNOWN 0
#define TOKEN_KEYWORD 1
#define TOKEN_BRACKET_OPEN 2
#define TOKEN_BRACKET_CLOSE 3
#define TOKEN_FUNCTION 4
#define TOKEN_TYPE 5
#define TOKEN_OPERATOR 6

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

bool is_type_token(token_t *token, const char *code) {
	int len=(token->end - token->start);
	char buf[len + 1];

	strncpy(buf, code + token->start, len);
	buf[len]='\0';

	type_t *current=&TYPES_AVAILABLE;

	while (current) {
		if (strcmp(buf, current->name)==0) {
			return true;
		}
		current=current->next;
	}
	return false;
}

bool is_keyword_token(token_t *token, const char *code) {
	int len=(token->end - token->start);
	char buf[len + 1];

	strncpy(buf, code + token->start, len);
	buf[len]='\0';

	for (unsigned long i=0 ; i<TOKEN_KEYWORDS_LEN ; i++) {
		if (strcmp(buf, TOKEN_KEYWORDS[i])==0) {
			return true;
		}
	}
	return false;
}

bool is_operator_token(token_t *token, const char *code) {
	int len=(token->end - token->start);
	char buf[len + 1];

	strncpy(buf, code + token->start, len);
	buf[len]='\0';

	for (unsigned long i=0 ; i<TOKEN_OPERATORS_LEN; i++) {
		if (strcmp(buf, TOKEN_OPERATORS[i])==0) {
			return true;
		}
	}
	return false;
}

bool is_function_token(token_t *token, const char *code) {
	int len=(token->end - token->start);
	char buf[len + 1];

	strncpy(buf, code + token->start, len);
	buf[len]='\0';

	//functions can end in "[]", or have a "[" without having a "]"
	if (len>3 && buf[len - 2]=='[' && buf[len - 1]==']') {
		return true;
	}
	else if (strchr(buf, '[')!=NULL && strchr(buf, ']')==NULL) {
		return true;
	}

	return false;
}

//determine what to classify a single token as
void classify_token(token_t *token, const char *code) {
	int len=(token->end - token->start);
	char buf[len + 1];

	strncpy(buf, code + token->start, len);
	buf[len]='\0';

	if (strcmp(buf, "[")==0) {
		token->token_type=TOKEN_BRACKET_OPEN;
	}
	else if (strcmp(buf, "]")==0) {
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
}

//classify all tokens starting from head
void classify_tokens(token_t *head, const char *code) {
	token_t *current=head;

	while (current!=NULL) {
		classify_token(current, code);
		current=current->next;
	}
}
