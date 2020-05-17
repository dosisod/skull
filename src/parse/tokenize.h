#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

typedef struct token_t {
	const wchar_t *begin;
	const wchar_t *end;
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
	token->begin=NULL;
	token->end=NULL;
	token->token_type=0;
	token->next=NULL;

	return token;
}

/*
Tokenize the passed code, returning the head to a linked list of tokens.
*/
token_t *tokenize(const wchar_t *code) {
	const wchar_t *code_copy=code;

	token_t *head=make_token();

	token_t *current=head;
	token_t *last=current;

	wchar_t quote=0;

	while (*code!=L'\0') {
		if (quote!=0) {
			if (*code==quote) {
				quote=0;
			}
		}
		else if (!quote && is_quote(*code)) {
			quote=*code;

			if (current->begin==NULL) {
				current->begin=code;
			}
		}
		else if (*code==L'[' || *code==L']') {
			if (current->begin!=NULL) {
				current->end=code;

				token_t *next_token=make_token();
				next_token->begin=code;
				next_token->end=code+1;

				current->next=next_token;
				current=next_token;
			}
			else {
				current->begin=code;
				current->end=code+1;
			}

			token_t *next_token=make_token();

			last=current;
			current->next=next_token;
			current=next_token;
		}
		else if (current->begin==NULL) {
			if (!is_whitespace(*code)) {
				current->begin=code;
			}
		}
		else if (current->end==NULL) {
			if (is_whitespace(*code)) {
				current->end=code;

				token_t *next_token=make_token();

				last=current;
				current->next=next_token;
				current=next_token;
			}
		}
		code++;
	}

	//close dangling token if there was no whitespace at EOF
	if (current->begin!=NULL) {
		current->end=code;
	}
	//if there is a no token to be created, pop last token
	else if (current!=head) {
		last->next=NULL;
		free(current);
	}
	//there where no tokens to parse, set safe defaults
	else {
		head->begin=code_copy;
		head->end=code_copy;
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
	return (token->end - token->begin);
}

/*
Returns true if `str` is equal to the value of `token`.
*/
bool token_cmp(const wchar_t* str, const token_t *token) {
	return wcsncmp(str, token->begin, token_len(token))==0;
}