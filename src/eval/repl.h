#pragma once

#include <stdio.h>

#include "../common/color.h"
#include "../errors.h"
#include "../eval/context.h"
#include "../eval/eval_add.h"
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

	uint8_t err=0;
	int64_t tmp=eval_integer(token->next->next->next, &err);

	if (err==EVAL_INTEGER_OK) {
		MAKE_TOKEN_BUF(type, token->next);
		MAKE_TOKEN_BUF(name, token);

		variable_t *var=make_variable(type, name, false);
		variable_write(var, &tmp);
		var->is_const=is_const;

		if (context_add_var(ctx, var)) {
			return NULL;
		}
		free_variable(var);
		return ERROR_MSG[ERROR_VAR_ALREADY_DEFINED];
	}

	return ERROR_MSG[ERROR_WRITING_TO_VAR];
}

/*
Evaluates a single line, returns result as a string (if any).
*/
const wchar_t *repl_eval(wchar_t *str, context_t *ctx) {
	if (*str==L'\0') {
		return NULL;
	}

	token_t *token=tokenize(str);
	classify_tokens(token);
	MAKE_TOKEN_BUF(buf, token);
	variable_t *var=context_find_name(ctx, buf);

	//print user defined variable
	if (var!=NULL && token->next==NULL) {
		wchar_t *ret=fmt_var(var);

		free_tokens(token);
		return ret;
	}

	//reassigning an existing variable
	if (var!=NULL && token->next!=ast_token_cmp(token->next,
		TOKEN_OPER_EQUAL,
		TOKEN_INT_CONST, -1))
	{
		uint8_t err=0;
		int64_t data=eval_integer(token->next->next, &err);

		if (err==EVAL_INTEGER_ERR) {
			free_tokens(token);
			return ERROR_MSG[ERROR_WRITING_TO_VAR];
		}

		if (variable_write(var, &data)==VARIABLE_WRITE_ECONST) {
			free_tokens(token);
			return ERROR_MSG[ERROR_CANNOT_ASSIGN_CONST];
		}

		free_tokens(token);
		return NULL;
	}

	if (var!=NULL && token->next!=ast_token_cmp(token->next,
		TOKEN_OPER_PLUS,
		TOKEN_IDENTIFIER, -1))
	{
		MAKE_TOKEN_BUF(rhs_buf, token->next->next);
		variable_t *var_rhs=context_find_name(ctx, rhs_buf);
		if (var_rhs==NULL) {
			free_tokens(token);
			return ERROR_MSG[ERROR_INVALID_INPUT];
		}

		variable_t *result=eval_add(var, var_rhs);
		wchar_t *ret=fmt_var(result);

		free_variable(result);
		free_tokens(token);
		return ret;
	}

	free_tokens(token);
	ast_node_t *node=make_ast_tree(str);
	const wchar_t *ret=ERROR_MSG[ERROR_INVALID_INPUT];

	if (node->node_type==AST_NODE_NO_PARAM_FUNC && token_cmp(L"clear", node->token)) {
		ret=L"\033[2J\033[;1H";
	}

	else if (node->token==NULL) {
		ret=ERROR_MSG[ERROR_INVALID_INPUT];
	}

	else if (node->node_type==AST_NODE_VAR_DEF) {
		ret=repl_make_var(node->token, ctx, true);
	}

	else if (node->node_type==AST_NODE_MUT_VAR_DEF) {
		ret=repl_make_var(node->token->next, ctx, false);
	}

	else if (node->node_type==AST_NODE_RETURN) {
		uint8_t err=0;
		int64_t num=eval_integer(node->token->next, &err);

		if (err!=EVAL_INTEGER_ERR) {
			free_ast_tree(node);
			exit((int)num);
		}
		ret=NULL;
	}

	else if (node->node_type==AST_NODE_UNKNOWN) {
		ret=NULL;
	}

	free_ast_tree(node);
	return ret;
}