#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

typedef struct token_t {
	size_t start;
	size_t end;
	short int token_type;

	struct token_t *next;
} token_t;

/*
Return true if `c` is whitespace.

Whitespace includes control-characters, non-printable characters, and spaces.
*/
bool is_whitespace(wchar_t c) {
	return (c <= 32);
}

/*
Return true if `c` is a double or single quote.
*/
bool is_quote(wchar_t c) {
	return (c=='\'' || c=='\"');
}

/*
Allocate and return a token with set defaults.
*/
token_t *make_token() {
	token_t *token=malloc(sizeof(token_t));
	token->start=-1;
	token->end=-1;
	token->token_type=0;
	token->next=NULL;

	return token;
}

/*
Tokenize the passed code, returning the head to a linked list of tokens.
*/
token_t *tokenize(const wchar_t *code) {
	token_t *head=make_token();

	token_t *current=head;
	token_t *last=current;

	const size_t CODE_LEN=wcslen(code);

	char quote=0;
	size_t i=0;
	for (; i<CODE_LEN ; i++) {
		if (quote!=0) {
			if (code[i]==quote) {
				quote=0;
			}
		}
		else if (!quote && is_quote(code[i])) {
			quote=code[i];

			if (current->start==(size_t)-1) {
				current->start=i;
			}
		}
		else if (current->start==(size_t)-1) {
			if (!is_whitespace(code[i])) {
				current->start=i;
			}
		}
		else if (current->end==(size_t)-1) {
			if (is_whitespace(code[i])) {
				current->end=i;

				token_t *next_token=make_token();

				last=current;
				current->next=next_token;
				current=next_token;
			}
		}
	}

	//close dangling token if there was no whitespace at EOF
	if (current->start!=(size_t)-1) {
		current->end=i;
	}
	//if there is a no token to be created, pop last token
	else if (current!=head) {
		last->next=NULL;
		free(current);
	}
	//there where no tokens to parse, set safe defaults
	else {
		head->start=0;
		head->end=0;
		head->next=NULL;
	}

	return head;
}

/*
Free all the tokens from a linked list of tokens.

This function should be called at the head of the linked list.
*/
void free_tokens(token_t *head) {
	token_t *tmp;

	while (head!=NULL) {
		tmp=head;
		head=head->next;
		tmp->next=NULL;
		free(tmp);
	}
}

/*
Returns the character length of the passed token
*/
size_t token_len(const token_t *token) {
	return (token->end - token->start);
}

/*
Returns true if `str` is equal to the value of `token`.
*/
bool token_cmp(const wchar_t* str, const token_t *token, const wchar_t *code) {
	return wcsncmp(str, code + token->start, token_len(token))==0;
}