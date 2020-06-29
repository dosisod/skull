#include <stdlib.h>
#include <stdlib.h>

#include "../common/malloc.h"

#include "tokenize.h"

__attribute__((const)) bool is_whitespace(char32_t c) {
	return (c <= 32);
}

__attribute__((const)) bool is_quote(char32_t c) {
	return (c=='\'' || c=='\"');
}

token_t *make_token(void) {
	token_t *token=malloc(sizeof(token_t));
	DIE_IF_MALLOC_FAILS(token);

	token->begin=NULL;
	token->end=NULL;
	token->token_type=0;
	token->next=NULL;

	return token;
}

token_t *tokenize(const char32_t *code) {
	const char32_t *code_copy=code;

	token_t *head=make_token();

	token_t *current=head;
	token_t *last=current;

	char32_t quote=0;

	while (*code!=U'\0') {
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
		else if (*code==U'[' || *code==U']' || *code==U',' || *code==U'\n') {
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

void free_tokens(token_t *head) {
	token_t *tmp;

	while (head!=NULL) {
		tmp=head;
		head=head->next;
		tmp->next=NULL;
		free(tmp);
	}
}

__attribute__((pure)) size_t token_len(const token_t *token) {
	return (size_t)(token->end - token->begin);
}

bool token_cmp(const char32_t *str, const token_t *token) {
	const size_t len=token_len(token);
	return (
		c32slen(str)==len &&
		c32sncmp(str, token->begin, len)
	);
}

char32_t *token_str(const token_t *token) {
	const size_t len=token_len(token);
	char32_t *str=malloc((len + 1) * sizeof(char32_t));
	DIE_IF_MALLOC_FAILS(str);

	c32slcpy(str, token->begin, len + 1);

	return str;
}
