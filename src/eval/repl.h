#pragma once

#include <stdio.h>

#include "../common/color.h"
#include "../common/malloc.h"
#include "../errors.h"
#include "../eval/context.h"
#include "../eval/eval_add.h"
#include "../eval/eval_assign.h"
#include "../eval/eval_float.h"
#include "../eval/eval_integer.h"
#include "../parse/ast/node.h"
#include "../parse/classify.h"
#include "../parse/tokenize.h"

#define REPL_MAX_LINE_LEN 1024

/*
Returns pointer to string read from file descriptor `fd`.
*/
char32_t *repl_read(FILE *fd) {
	if (fd==stdin) {
		printf(COLOR_BRIGHT_GREEN_FG "> " COLOR_RESET);
	}

	char *str=malloc(sizeof(char) * REPL_MAX_LINE_LEN);
	DIE_IF_MALLOC_FAILS(str);

	size_t offset=0;
	char c=(char)getc(fd);

	while (c!='\n') {
		//read char by char until we need to reallocate more memory
		if (offset!=0 && ((offset + 1) % REPL_MAX_LINE_LEN)==0) {
			char *new_str=realloc(str, sizeof(char) * (offset + REPL_MAX_LINE_LEN));
			DIE_IF_MALLOC_FAILS(new_str);

			str=new_str;
		}
		str[offset]=c;
		offset++;

		c=(char)getc(fd);
	}
	str[offset]='\0';

	char32_t *ret=mbstoc32s(str);

	free(str);
	return ret;
}

/*
Make and add a variable from passed `tokens` to context `ctx`.

Added variable will be constant if `is_const` is true.
*/
const char32_t *repl_make_var(const token_t *token, context_t *ctx, bool is_const) {
	if (ctx==NULL) {
		return NULL;
	}

	if (token->next->next==NULL) {
		return ERROR_MSG[ERROR_INVALID_INPUT];
	}

	token_t *value=token->next->next->next;
	bool is_auto_assign=false;

	if (token->next->token_type==TOKEN_OPER_AUTO_EQUAL) {
		value=token->next->next;
		is_auto_assign=true;
	}

	//token after "=" or ":=" is required
	if (value==NULL) {
		return ERROR_MSG[ERROR_INVALID_INPUT];
	}

	MAKE_TOKEN_BUF(name, token);
	variable_t *var=NULL;

	if (is_auto_assign) {
		const char32_t *type=NULL;
		if (value->token_type==TOKEN_INT_CONST) {
			type=U"int";
		}
		else if (value->token_type==TOKEN_FLOAT_CONST) {
			type=U"float";
		}
		else if (value->token_type==TOKEN_BOOL_CONST) {
			type=U"bool";
		}
		else if (value->token_type==TOKEN_STR_CONST) {
			type=U"str";
		}
		else if (value->token_type==TOKEN_CHAR_CONST) {
			type=U"char";
		}
		else if (value->token_type==TOKEN_IDENTIFIER) {
			MAKE_TOKEN_BUF(buf, token->next->next);
			variable_t *new_var=context_find_name(ctx, buf);

			if (new_var!=NULL) {
				type=new_var->type->name;
			}
			else {
				return ERROR_MSG[ERROR_INVALID_INPUT];
			}
		}
		else {
			return ERROR_MSG[ERROR_INVALID_INPUT];
		}
		var=make_variable(type, name, false);
	}
	else {
		MAKE_TOKEN_BUF(type, token->next);
		var=make_variable(type, name, false);
	}

	const char32_t *tmp=eval_assign(var, value, ctx);
	var->is_const=is_const;

	if (tmp!=NULL) {
		free_variable(var);
		return tmp;
	}
	if (context_add_var(ctx, var)) {
		return NULL;
	}
	free_variable(var);
	return ERROR_MSG[ERROR_VAR_ALREADY_DEFINED];
}

/*
Evaluates a single line, returns result as a string (if any).
*/
const char32_t *repl_eval(const char32_t *str, context_t *ctx) {
	if (*str==U'\0') {
		return NULL;
	}

	token_t *token=tokenize(str);
	classify_tokens(token);
	MAKE_TOKEN_BUF(buf, token);
	variable_t *var=context_find_name(ctx, buf);

	//print user defined variable
	if (var!=NULL && token->next==NULL) {
		char32_t *ret=fmt_var(var);

		free_tokens(token);
		return ret;
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
		char32_t *ret=fmt_var(result);

		free_variable(result);
		free_tokens(token);
		return ret;
	}

	free_tokens(token);
	ast_node_t *node=make_ast_tree(str);
	const char32_t *ret=ERROR_MSG[ERROR_INVALID_INPUT];

	if (node->node_type==AST_NODE_NO_PARAM_FUNC && token_cmp(U"clear", node->token)) {
		ret=c32sdup(U"\033[2J\033[;1H");
		DIE_IF_MALLOC_FAILS(ret);
	}

	else if (var!=NULL && node->node_type==AST_NODE_VAR_ASSIGN) {
		if (var->is_const) {
			return ERROR_MSG[ERROR_CANNOT_ASSIGN_CONST];
		}
		ret=eval_assign(var, node->token->next->next, ctx);
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

	else if (node->node_type==AST_NODE_AUTO_VAR_DEF) {
		ret=repl_make_var(node->token, ctx, true);
	}

	else if (node->node_type==AST_NODE_MUT_AUTO_VAR_DEF) {
		ret=repl_make_var(node->token->next, ctx, false);
	}

	else if (node->node_type==AST_NODE_RETURN) {
		const char32_t *err=NULL;
		int64_t num=eval_integer(node->token->next, &err);

		if (err==NULL) {
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