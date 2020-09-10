#include <stdlib.h>

#include "skull/common/color.h"
#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/eval/eval_assign.h"
#include "skull/eval/function.h"
#include "skull/eval/types/defs.h"
#include "skull/parse/classify.h"

#include "skull/eval/repl.h"

/*
Make and add a variable from `node` to context `ctx`.

Added variable will be constant if `is_const` is true.

Returns pointer to error message if one occurs, else `NULL`.
*/
const char32_t *repl_make_var(const AstNode *node, Context *ctx, bool is_const) {
	if (!ctx) {
		return NULL;
	}

	const Token *token = node->token;
	if (!is_const) {
		token = token->next;
	}

	if (!node->next) {
		return FMT_ERROR(ERR_MISSING_ASSIGNMENT, { .tok = token });
	}

	char32_t *name = token_str(token);
	Variable *var = NULL;

	if (token->next->token_type == TOKEN_OPER_AUTO_EQUAL) {
		if (is_func_name_str(name)) {
			char32_t *error = FMT_ERROR(ERR_ASSIGN_FUNC, { .str = name });

			free(name);
			return error;
		}

		const char *type = NULL;
		if (node->next->node_type == AST_NODE_INT_CONST) {
			type = TYPE_INT.name;
		}
		else if (node->next->node_type == AST_NODE_FLOAT_CONST) {
			type = TYPE_FLOAT.name;
		}
		else if (node->next->node_type == AST_NODE_BOOL_CONST) {
			type = TYPE_BOOL.name;
		}
		else if (node->next->node_type == AST_NODE_STR_CONST) {
			type = TYPE_STR.name;
		}
		else if (node->next->node_type == AST_NODE_RUNE_CONST) {
			type = TYPE_RUNE.name;
		}
		else if (node->next->node_type == AST_NODE_IDENTIFIER ||
			node->next->node_type == AST_NODE_ADD_VAR ||
			node->next->node_type == AST_NODE_SUB_VAR ||
			node->next->node_type == AST_NODE_MULT_VAR ||
			node->next->node_type == AST_NODE_DIV_VAR
		) {
			char32_t *lookup = token_str(node->next->token);
			Variable *new_var = context_find_name(ctx, lookup);

			if (!new_var) {
				free(name);
				return FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = lookup });
			}
			free(lookup);
			type = new_var->type->name;
		}
		else {
			free(name);
			return FMT_ERROR(ERR_INVALID_INPUT, { .tok = node->next->token });
		}
		var = make_variable(type, name, false);
	}
	else {
		char32_t *tmp_name = token_str(token->next);
		char *type_name = c32stombs(tmp_name);
		free(tmp_name);

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