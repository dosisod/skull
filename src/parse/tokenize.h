#pragma once

#include <string.h>
#include <stdlib.h>

typedef struct token_t {
	unsigned long start;
	unsigned long end;
	int token_type;

	struct token_t *next;
} token_t;

/*
Return true if `c` is whitespace.

Whitespace includes control-characters, non-printable characters, and spaces.
*/
bool is_whitespace(char c) {
	return (c <= 32);
}

/*
Return true if `c` is a double or single quote.
*/
bool is_quote(char c) {
	return (c=='\'' || c=='\"');
}

/*
Tokenize the passed code, returning the head to a linked list of tokens.
*/
token_t *tokenize(const char *code) {
	struct token_t *head=malloc(sizeof(token_t));
	head->start=-1;
	head->end=-1;
	head->token_type=0;
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

			if (current->start==(unsigned long)-1) {
				current->start=i;
			}
		}
		else if (current->start==(unsigned long)-1) {
			if (!is_whitespace(code[i])) {
				current->start=i;
			}
		}
		else if (current->end==(unsigned long)-1) {
			if (is_whitespace(code[i])) {
				current->end=i;

				token_t *next_token=malloc(sizeof(token_t));
				next_token->start=-1;
				next_token->end=-1;
				next_token->token_type=0;
				next_token->next=NULL;

				last=current;
				current->next=next_token;
				current=next_token;
			}
		}
	}

	//close dangling token if there was no whitespace at EOF
	if (current->start!=(unsigned long)-1) {
		current->end=i;
	}
	//if there is a no token to be created, pop last token
	else {
		last->next=NULL;
		free(current);
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
		free(tmp);
		tmp->next=NULL;
	}
}