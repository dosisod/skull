#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//a token is text between whitespace/control characters
typedef struct token_t {
	unsigned long start;
	unsigned long end;

	struct token_t *next;
} token_t;

bool is_whitespace(char c) {
	return (c <= 32);
}

//tokenize passed in string, returns a linked list of tokens
token_t *tokenize(const char *code) {
	struct token_t *head=malloc(sizeof(token_t));
	head->start=-1;
	head->end=-1;
	head->next=NULL;

	struct token_t *current=head;
	struct token_t *last=current;

	unsigned long CODE_LEN=strlen(code);

	unsigned long i=0;
	for (; i<CODE_LEN ; i++) {
		if (current->start==-1) {
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