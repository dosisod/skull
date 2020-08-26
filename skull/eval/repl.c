#include <stdio.h>
#include <stdlib.h>

#include "skull/common/color.h"
#include "skull/common/io.h"
#include "skull/common/malloc.h"
#include "skull/errors.h"
#include "skull/eval/eval_add.h"
#include "skull/eval/eval_assign.h"
#include "skull/eval/eval_float.h"
#include "skull/eval/eval_integer.h"
#include "skull/eval/function.h"
#include "skull/parse/classify.h"

#include "skull/eval/repl.h"

/*
Evaluates a string `str` given context `ctx`, returns result as a string (if any).
*/
const char32_t *repl_eval(const char32_t *str, context_t *ctx) {
	if (!*str) {
		return NULL;
	}

	ast_node_t *node = make_ast_tree(str);
	const char32_t *ret = ERR_INVALID_INPUT;

	if (!node->token) {
		ret = ERR_INVALID_INPUT;
	}

	else if ((node->node_type == AST_NODE_ONE_PARAM_FUNC ||
		node->node_type == AST_NODE_NO_PARAM_FUNC) &&
		is_func_name(node))
	{
		if (token_cmp(U"clear", node->token)) {
			ret = func_clear(node);
		}
		else if (token_cmp(U"print", node->token)) {
			ret = func_print(node, ctx);
		}
	}

	else if (node->node_type == AST_NODE_COMMENT || node->node_type == AST_NODE_UNKNOWN) {
		ret = NULL;
	}

	else if (node->node_type == AST_NODE_VAR_ASSIGN) {
		char32_t *name = token_str(node->token);
		variable_t *var = context_find_name(ctx, name);

		if (!var) {
			return FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = name });
		}
		free(name);

		if (var->is_const) {
			return FMT_ERROR(ERR_CANNOT_ASSIGN_CONST, { .var = var });
		}
		ret = eval_assign(var, node->next, ctx);
	}

	else if (node->node_type == AST_NODE_VAR_DEF ||
		node->node_type == AST_NODE_AUTO_VAR_DEF)
	{
		ret = repl_make_var(node, ctx, true);
	}

	else if (node->node_type == AST_NODE_MUT_VAR_DEF ||
		node->node_type == AST_NODE_MUT_AUTO_VAR_DEF)
	{
		ret = repl_make_var(node, ctx, false);
	}

	else if (node->node_type == AST_NODE_RETURN) {
		if (node->token->next->token_type == TOKEN_IDENTIFIER) {
			char32_t *var_name = token_str(node->token->next);
			const variable_t *found_var = context_find_name(ctx, var_name);

			if (found_var) {
				free(var_name);
				if (found_var->type != &TYPE_INT) {
					ret = FMT_ERROR(ERR_NON_INT_RETURN, { .var = found_var });
				}
				else {
					int64_t num = 0;
					variable_read(&num, found_var);
					exit((int)num);
				}
			}
			else {
				ret = FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = var_name });
			}
		}
		else { //token is an int
			ret = NULL;
			int64_t *num = eval_integer(node->token->next, &ret);

			if (!ret) {
				free_ast_tree(node);
				exit((int)*num);
			}
		}
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
	if (!ctx) {
		return NULL;
	}

	const token_t *token = node->token;
	if (!is_const) {
		token = token->next;
	}

	if (!node->next) {
		return FMT_ERROR(ERR_MISSING_ASSIGNMENT, { .tok = token });
	}

	char32_t *name = token_str(token);
	variable_t *var = NULL;

	if (token->next->token_type == TOKEN_OPER_AUTO_EQUAL) {
		if (is_func_name_str(name)) {
			char32_t *error = FMT_ERROR(ERR_ASSIGN_FUNC, { .str = name });

			free(name);
			return error;
		}

		const char32_t *type = NULL;
		if (node->next->node_type == AST_NODE_INT_CONST) {
			type = U"int";
		}
		else if (node->next->node_type == AST_NODE_FLOAT_CONST) {
			type = U"float";
		}
		else if (node->next->node_type == AST_NODE_BOOL_CONST) {
			type = U"bool";
		}
		else if (node->next->node_type == AST_NODE_STR_CONST) {
			type = U"str";
		}
		else if (node->next->node_type == AST_NODE_CHAR_CONST) {
			type = U"char";
		}
		else if (node->next->node_type == AST_NODE_IDENTIFIER ||
			node->next->node_type == AST_NODE_ADD_VAR ||
			node->next->node_type == AST_NODE_SUB_VAR ||
			node->next->node_type == AST_NODE_MULT_VAR ||
			node->next->node_type == AST_NODE_DIV_VAR
		) {
			char32_t *lookup = token_str(node->next->token);
			variable_t *new_var = context_find_name(ctx, lookup);

			if (!new_var) {
				free(name);
				return FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = lookup });
			}
			free(lookup);
			type = new_var->type->name;
		}
		else {
			free(name);
			return ERR_INVALID_INPUT;
		}
		var = make_variable(type, name, false);
	}
	else {
		char32_t *type_name = token_str(token->next);
		var = make_variable(type_name, name, false);
		free(type_name);
	}

	const char32_t *tmp = eval_assign(var, node->next, ctx);
	var->is_const = is_const;

	if (tmp) {
		free_variable(var);
		free(name);
		return tmp;
	}
	if (context_add_var(ctx, var)) {
		free(name);
		return NULL;
	}
	free_variable(var);

	return FMT_ERROR(ERR_VAR_ALREADY_DEFINED, { .real = name });
}

/*
Read from `fd`, eval with context `ctx`, and print out result.
*/
void repl_loop(FILE *fd, context_t *ctx) {
	char32_t *line = repl_read(fd);

	const char32_t *tmp = repl_eval(line, ctx);
	char *output = c32stombs(tmp);
	if (output) {
		puts(output);
	}

	free(output);
	free(line);
	if (!is_error_msg(tmp)) {
		free((char32_t*)tmp);
	}
}

/*
Read from `fd`, add prompt if reading from `stdin`.
*/
char32_t *repl_read(FILE *fd) {
	if (fd == stdin) {
		printf(COLOR_BRIGHT_GREEN_FG "> " COLOR_RESET);
	}

	return read_file(fd, true);
}