#pragma once

#include <stdio.h>

#include "../common/color.h"
#include "../eval/context.h"
#include "../eval/eval_integer.h"
#include "../parse/ast/node.h"
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
Make and add a variable from passed `tokens` to context `ctx`.

Added variable will be constant if `is_const` is true.
*/
const wchar_t *repl_make_var(const token_t *token, context_t *ctx, bool is_const) {
	if (ctx==NULL) {
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

			return L"variable already defined";
		}
	}
	else {
		free_variable(var);
	}
	return NULL;
}

/*
Evaluates a single line, returns result as a string (if any).
*/
const wchar_t *repl_eval(wchar_t *str, context_t *ctx) {
	token_t *token=tokenize(str);
	token_t *head=token;
	classify_tokens(token);

	//blank or empty line
	if (token->end==token->begin) {
		free_tokens(head);
		return NULL;
	}

	// x: int = 0
	if (token!=ast_token_cmp(token,
		TOKEN_IDENTIFIER,
		TOKEN_TYPE,
		TOKEN_OPER_EQUAL,
		TOKEN_INT_CONST, -1))
	{
		const wchar_t *ret=repl_make_var(token, ctx, true);

		free_tokens(head);
		return ret;
	}

	// mut x: int = 0
	if (token!=ast_token_cmp(token,
		TOKEN_KW_MUT,
		TOKEN_IDENTIFIER,
		TOKEN_TYPE,
		TOKEN_OPER_EQUAL,
		TOKEN_INT_CONST, -1))
	{
		const wchar_t *ret=repl_make_var(token->next, ctx, false);

		free_tokens(head);
		return ret;
	}

	MAKE_TOKEN_BUF(buf, token);
	variable_t *var=context_find_name(ctx, buf);

	//print user defined variable
	if (var!=NULL && token->next==NULL) {
		int64_t val=0;
		variable_read(&val, var);

		wprintf(L"%lli\n", val);

		free_tokens(head);
		return NULL;
	}

	//reassigning an existing variable
	if (var!=NULL && token->next!=ast_token_cmp(token->next,
		TOKEN_OPER_EQUAL,
		TOKEN_INT_CONST, -1))
	{
		uint8_t err=0;
		int64_t data=eval_integer(token->next->next, &err);

		if (err==EVAL_INTEGER_ERR) {
			free_tokens(head);
			return NULL;
		}

		if (variable_write(var, &data)==VARIABLE_WRITE_ECONST) {
			free_tokens(head);
			return L"cannot assign to const";
		}

		free_tokens(head);
		return NULL;
	}

	if (token!=ast_token_cmp(token,
		TOKEN_KW_RETURN,
		TOKEN_INT_CONST, -1))
	{
		uint8_t err=0;
		int64_t ret=eval_integer(token->next, &err);

		if (err==EVAL_INTEGER_ERR) {
			return NULL;
		}

		free_tokens(head);
		exit((int)ret);
	}

	if (token_cmp(L"clear", token) && token!=ast_token_cmp(token->next,
		TOKEN_BRACKET_OPEN,
		TOKEN_BRACKET_CLOSE, -1))
	{
		free_tokens(head);
		return L"\033[2J\033[;1H";
	}

	free_tokens(head);
	return L"invalid input";
}