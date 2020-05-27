#pragma once

#include "../parse/classify.h"
#include "../parse/tokenize.h"

/*
Evaluates a single line, returns result as a string (if any).
*/
wchar_t *repl_eval(wchar_t *str) {
	wchar_t *ret=str;
	token_t *token=tokenize(str);
	classify_tokens(token);

	if (token->token_type==TOKEN_TYPE &&
		token->next!=NULL &&
		token->next->token_type==TOKEN_UNKNOWN &&
		token->next->next!=NULL &&
		token->next->next->token_type==TOKEN_OPERATOR &&
		token->next->next->next!=NULL &&
		token->next->next->next->token_type==TOKEN_INT_CONST)
	{
		ret=NULL;
	}

	free_tokens(token);
	return ret;
}