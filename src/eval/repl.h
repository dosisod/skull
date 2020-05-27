#pragma once

#include <stdio.h>

#include "../common/color.h"
#include "../parse/classify.h"
#include "../parse/tokenize.h"

#define REPL_MAX_LINE_LEN 1024

/*
Returns pointer to string read from commandline.
*/
wchar_t *repl_read() {
	wchar_t *line=malloc(REPL_MAX_LINE_LEN * sizeof(wchar_t));

	wprintf(COLOR_BRIGHT_GREEN_FG L"> " COLOR_RESET);
	fgetws(line, REPL_MAX_LINE_LEN, stdin);
	line[wcscspn(line, L"\n")]=0;

	return line;
}

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