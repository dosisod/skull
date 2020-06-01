#pragma once

#include <stdio.h>

#include "../common/color.h"
#include "../eval/context.h"
#include "../eval/eval_integer.h"
#include "../parse/classify.h"
#include "../parse/tokenize.h"

#define REPL_MAX_LINE_LEN 1024

/*
Returns pointer to string read from commandline.
*/
wchar_t *repl_read(void) {
	wchar_t *line=malloc(REPL_MAX_LINE_LEN * sizeof(wchar_t));

	wprintf(COLOR_BRIGHT_GREEN_FG L"> " COLOR_RESET);
	fgetws(line, REPL_MAX_LINE_LEN, stdin);
	line[wcscspn(line, L"\n")]=0;

	return line;
}

/*
Evaluates a single line, returns result as a string (if any).
*/
const wchar_t *repl_eval(wchar_t *str, context_t *ctx) {
	token_t *token=tokenize(str);
	token_t *head=token;
	classify_tokens(token);

	bool is_const=true;
	if (token->token_type==TOKEN_KW_MUT) {
		is_const=false;
		token=token->next;

		if (token==NULL) {
			free_tokens(head);
			return NULL;
		}
	}

	if (token->token_type==TOKEN_IDENTIFIER &&
		token->next!=NULL &&
		token->next->token_type==TOKEN_TYPE &&
		token->next->next!=NULL &&
		token->next->next->token_type==TOKEN_OPERATOR &&
		token->next->next->next!=NULL &&
		token->next->next->next->token_type==TOKEN_INT_CONST)
	{
		if (ctx==NULL) {
			free_tokens(head);
			return NULL;
		}

		MAKE_TOKEN_BUF(type, token->next);
		MAKE_TOKEN_BUF(name, token);

		variable_t *var=make_variable(type, name, false);

		uint8_t err=0;
		int64_t tmp=eval_integer(token->next->next->next, &err);

		if (err==EVAL_INTEGER_OK) {
			variable_write(var, &tmp);
			var->is_const=is_const;
			if (!context_add_var(ctx, var)) {
				free_variable(var);
				free_tokens(head);

				return L"variable already defined";
			}
		}
		else {
			free_variable(var);
		}
		free_tokens(head);
		return NULL;
	}

	variable_t *var=NULL;
	if (ctx!=NULL) {
		var=context_find_name(ctx, token->begin);
	}
	if (var!=NULL) {
		int64_t val=0;
		variable_read(&val, var);

		wprintf(L"%lli\n", val);

		free_tokens(token);
		return NULL;
	}

	if (*token->begin!=L'\0' && token_cmp(L"return", token)) {
		free_tokens(token);
		exit(0);
	}

	free_tokens(token);
	return L"invalid input";
}