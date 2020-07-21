#include <stdio.h>
#include <stdlib.h>

#include "../common/color.h"
#include "../common/malloc.h"
#include "../errors.h"
#include "../parse/classify.h"
#include "eval_add.h"
#include "eval_assign.h"
#include "eval_float.h"
#include "eval_integer.h"
#include "function.h"

#include "repl.h"

/*
Evaluates a string `str` given context `ctx`, returns result as a string (if any).
*/
const char32_t *repl_eval(const char32_t *str, context_t *ctx) {
	if (*str==U'\0') {
		return NULL;
	}

	ast_node_t *node=make_ast_tree(str);
	const char32_t *ret=ERR_INVALID_INPUT;

	if ((node->node_type==AST_NODE_ONE_PARAM_FUNC ||
		node->node_type==AST_NODE_NO_PARAM_FUNC) &&
		is_func_name(node))
	{
		if (token_cmp(U"clear", node->token)) {
			ret=func_clear(node);
		}
		else if (token_cmp(U"print", node->token)) {
			ret=func_print(node, ctx);
		}
	}

	else if (node->node_type==AST_NODE_COMMENT) {
		ret=NULL;
	}

	else if (node->node_type==AST_NODE_VAR_ASSIGN) {
		MAKE_TOKEN_BUF(buf, node->token);
		variable_t *var=context_find_name(ctx, buf);

		if (var==NULL) {
			return ERR_VAR_NOT_FOUND;
		}

		if (var->is_const) {
			return ERR_CANNOT_ASSIGN_CONST;
		}
		ret=eval_assign(var, node->next, ctx);
	}

	else if (node->token==NULL) {
		ret=ERR_INVALID_INPUT;
	}

	else if (node->node_type==AST_NODE_VAR_DEF ||
		node->node_type==AST_NODE_AUTO_VAR_DEF)
	{
		ret=repl_make_var(node, ctx, true);
	}

	else if (node->node_type==AST_NODE_MUT_VAR_DEF ||
		node->node_type==AST_NODE_MUT_AUTO_VAR_DEF)
	{
		ret=repl_make_var(node, ctx, false);
	}

	else if (node->node_type==AST_NODE_RETURN) {
		if (node->token->next->token_type==TOKEN_IDENTIFIER) {
			MAKE_TOKEN_BUF(var_name, node->token->next);
			const variable_t *found_var=context_find_name(ctx, var_name);

			if (found_var!=NULL) {
				if (found_var->type!=&TYPE_INT) {
					ret=ERR_NON_INT_RETURN;
				}
				else {
					int64_t num=0;
					variable_read(&num, found_var);
					exit((int)num);
				}
			}
			else {
				ret=ERR_VAR_NOT_FOUND;
			}
		}
		else { //token is an int
			ret=NULL;
			int64_t num=eval_integer(node->token->next, &ret);

			if (ret==NULL) {
				free_ast_tree(node);
				exit((int)num);
			}
		}
	}

	else if (node->node_type==AST_NODE_UNKNOWN) {
		ret=NULL;
	}

	free_ast_tree(node);
	return ret;
}

/*
Make and add a variable from `node` to context `ctx`.

Added variable will be constant if `is_const` is true.

Returns pointer to error message if one occurs, else `NULL`.
*/
const char32_t *repl_make_var(const ast_node_t *node, context_t *ctx, bool is_const) {
	if (ctx==NULL) {
		return NULL;
	}

	if (node->next==NULL) {
		return ERR_MISSING_ASSIGNMENT;
	}

	const token_t *token=node->token;
	if (!is_const) {
		token=token->next;
	}

	MAKE_TOKEN_BUF(name, token);
	variable_t *var=NULL;

	if (token->next->token_type==TOKEN_OPER_AUTO_EQUAL) {
		if (is_func_name_str(name)) {
			return ERR_ASSIGN_FUNC;
		}

		const char32_t *type=NULL;
		if (node->next->node_type==AST_NODE_INT_CONST) {
			type=U"int";
		}
		else if (node->next->node_type==AST_NODE_FLOAT_CONST) {
			type=U"float";
		}
		else if (node->next->node_type==AST_NODE_BOOL_CONST) {
			type=U"bool";
		}
		else if (node->next->node_type==AST_NODE_STR_CONST) {
			type=U"str";
		}
		else if (node->next->node_type==AST_NODE_CHAR_CONST) {
			type=U"char";
		}
		else if (node->next->node_type==AST_NODE_IDENTIFIER ||
			node->next->node_type==AST_NODE_ADD_VAR ||
			node->next->node_type==AST_NODE_SUB_VAR ||
			node->next->node_type==AST_NODE_MULT_VAR
		) {
			MAKE_TOKEN_BUF(buf, node->next->token);
			variable_t *new_var=context_find_name(ctx, buf);

			if (new_var==NULL) {
				return ERR_VAR_NOT_FOUND;
			}
			type=new_var->type->name;
		}
		else {
			return ERR_INVALID_INPUT;
		}
		var=make_variable(type, name, false);
	}
	else {
		MAKE_TOKEN_BUF(type, token->next);
		var=make_variable(type, name, false);
	}

	const char32_t *tmp=eval_assign(var, node->next, ctx);
	var->is_const=is_const;

	if (tmp!=NULL) {
		free_variable(var);
		return tmp;
	}
	if (context_add_var(ctx, var)) {
		return NULL;
	}
	free_variable(var);
	return ERR_VAR_ALREADY_DEFINED;
}

/*
Read from `fd`, eval with context `ctx`, and print out result.
*/
void repl_loop(FILE *fd, context_t *ctx) {
	char32_t *line=repl_read(fd);

	const char32_t *tmp=repl_eval(line, ctx);
	char *output=c32stombs(tmp);
	if (output!=NULL) {
		puts(output);
	}

	free(output);
	free(line);
	if (!is_error_msg(tmp)) {
		free((char32_t*)tmp);
	}
}

/*
Returns pointer to UTF-32 string read from file descriptor `fd`.
*/
char32_t *repl_read(FILE *fd) {
	if (fd==stdin) {
		printf(COLOR_BRIGHT_GREEN_FG "> " COLOR_RESET);
	}

	char *str=malloc(sizeof(char) * REPL_MAX_LINE_LEN);
	DIE_IF_MALLOC_FAILS(str);

	size_t offset=0;
	int c=getc(fd);

	while (c!='\n' && c!=EOF) {
		//read char by char until we need to reallocate more memory
		if (offset!=0 && ((offset + 1) % REPL_MAX_LINE_LEN)==0) {
			char *new_str=realloc(str, sizeof(char) * (offset + REPL_MAX_LINE_LEN));
			DIE_IF_MALLOC_FAILS(new_str);

			str=new_str;
		}
		str[offset]=(char)c;
		offset++;

		c=getc(fd);
	}
	str[offset]='\0';

	char32_t *ret=mbstoc32s(str);

	free(str);
	return ret;
}