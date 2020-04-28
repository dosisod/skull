#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

//a token is text between whitespace/control characters
typedef struct token_t {
	unsigned long start;
	unsigned long end;
	int token_type;

	struct token_t *next;
} token_t;

bool is_whitespace(char c) {
	return (c <= 32);
}

bool is_quote(char c) {
	return (c=='\'' || c=='\"');
}

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

//tokenize passed in string, returns a linked list of tokens
token_t *tokenize(const char *code) {
	struct token_t *head=malloc(sizeof(token_t));
	head->start=-1;
	head->end=-1;
	head->token_type=TOKEN_UNKNOWN;
	head->next=NULL;

	struct token_t *current=head;
	struct token_t *last=current;

	unsigned long CODE_LEN=strlen(code);

	char quote=0;
	unsigned long i=0;
	for (; i<CODE_LEN ; i++) {
		if (quote!=0) {
			if (code[i]==quote) {
				quote=0;
			}
		}
		else if (!quote && is_quote(code[i])) {
			quote=code[i];

			if (current->start==-1) {
				current->start=i;
			}
		}
		else if (current->start==-1) {
			if (!is_whitespace(code[i])) {
				current->start=i;
			}
		}
		else if (current->end==-1) {
			if (is_whitespace(code[i])) {
				current->end=i;

				token_t *next_token=malloc(sizeof(token_t));
				next_token->start=-1;
				next_token->end=-1;
				next_token->token_type=TOKEN_UNKNOWN;

				last=current;
				current->next=next_token;
				current=next_token;
			}
		}
	}

	//close dangling token if there was no whitespace at EOF
	if (current->start!=-1) {
		current->end=i;
	}
	//if there is a no token to be created, pop last token
	else {
		last->next=NULL;
		free(current);
	}

	return head;
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
}

//classify all tokens starting from head
void classify_tokens(token_t *head, const char *code) {
	token_t *current=head;

	while (current!=NULL) {
		classify_token(current, code);
		current=current->next;
	}
}

void free_tokens(token_t *head) {
	token_t *tmp;

	while (head!=NULL) {
		tmp=head;
		head=head->next;
		free(tmp);
		tmp->next=NULL;
	}
}