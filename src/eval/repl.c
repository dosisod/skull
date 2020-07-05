#include <stdio.h>
#include <stdlib.h>

#include "../common/color.h"
#include "../common/malloc.h"
#include "../errors.h"
#include "../eval/eval_add.h"
#include "../eval/eval_assign.h"
#include "../eval/eval_float.h"
#include "../eval/eval_integer.h"
#include "../parse/ast/node.h"
#include "../parse/classify.h"

#include "repl.h"

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

/*
Make and add a variable from passed `tokens` to context `ctx`.

Added variable will be constant if `is_const` is true.
*/
const char32_t *repl_make_var(const token_t *token, context_t *ctx, bool is_const) {
	if (ctx==NULL) {
		return NULL;
	}

	if (token->next->next==NULL) {
		return ERR_MISSING_ASSIGNMENT;
	}

	token_t *value=token->next->next->next;
	bool is_auto_assign=false;

	if (token->next->token_type==TOKEN_OPER_AUTO_EQUAL) {
		value=token->next->next;
		is_auto_assign=true;
	}

	//token after "=" or ":=" is required
	if (value==NULL) {
		return ERR_MISSING_ASSIGNMENT;
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
				return ERR_VAR_NOT_FOUND;
			}
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
	return ERR_VAR_ALREADY_DEFINED;
}

/*
Evaluates a string `str` given context `ctx`, returns result as a string (if any).
*/
const char32_t *repl_eval(const char32_t *str, context_t *ctx) {
	if (*str==U'\0') {
		return NULL;
	}

	token_t *token=tokenize(str);
	classify_tokens(token);
	MAKE_TOKEN_BUF(buf, token);
	variable_t *var=context_find_name(ctx, buf);

	free_tokens(token);
	ast_node_t *node=make_ast_tree(str);
	const char32_t *ret=ERR_INVALID_INPUT;

	if (node->node_type==AST_NODE_NO_PARAM_FUNC && token_cmp(U"clear", node->token)) {
		ret=c32sdup(U"\033[2J\033[;1H");
		DIE_IF_MALLOC_FAILS(ret);
	}

	else if (node->node_type==AST_NODE_ONE_PARAM_FUNC &&
		token_cmp(U"print", node->token) &&
		node->token->next->next->token_type==TOKEN_IDENTIFIER)
	{
		MAKE_TOKEN_BUF(var_name, node->token->next->next);
		variable_t *found_var=context_find_name(ctx, var_name);

		if (found_var==NULL) {
			ret=ERR_VAR_NOT_FOUND;
		}
		else {
			ret=fmt_var(found_var);
		}
	}

	else if (node->node_type==AST_NODE_COMMENT) {
		ret=NULL;
	}

	else if (var!=NULL && node->node_type==AST_NODE_VAR_ASSIGN) {
		if (var->is_const) {
			return ERR_CANNOT_ASSIGN_CONST;
		}
		ret=eval_assign(var, node->token->next->next, ctx);
	}

	else if (node->token==NULL) {
		ret=ERR_INVALID_INPUT;
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
		if (node->token->next->token_type==TOKEN_IDENTIFIER) {
			MAKE_TOKEN_BUF(var_name, node->token->next);
			const variable_t *found_var=context_find_name(ctx, var_name);

			if (found_var!=NULL) {
				if (found_var->type!=&TYPE_INT) {
					ret=ERR_TYPE_MISMATCH;
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
Read from `fd`, eval with context `ctx`, and print out result.
*/
void repl_loop(FILE *fd, context_t *ctx) {
	char32_t *line=repl_read(fd);

	const char32_t *tmp=repl_eval(line, ctx);
	char *output=c32stombs(tmp);
	if (output!=NULL) {
		printf("%s\n", output);
	}

	free(output);
	free(line);
	if (!is_error_msg(tmp)) {
		free((char32_t*)tmp);
	}
}