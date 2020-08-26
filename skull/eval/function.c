#include <stdio.h>
#include <stdlib.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/parse/classify.h"

#include "skull/eval/function.h"

/*
Returns string that when printed, clears the screen.
*/
const char32_t *func_clear(ast_node_t *node) {
	if (node->node_type != AST_NODE_NO_PARAM_FUNC) {
		return FMT_ERROR(ERR_UNEXPECTED_PARAM, { .str = U"clear" });
	}
	const char32_t *ret = c32sdup(U"\033[2J\033[;1H");
	DIE_IF_MALLOC_FAILS(ret);

	return ret;
}

/*
Print out a variable.
*/
const char32_t *func_print(ast_node_t *node, context_t *ctx) {
	if (node->node_type != AST_NODE_ONE_PARAM_FUNC ||
		node->token->next->next->token_type != TOKEN_IDENTIFIER
	) {
		return FMT_ERROR(ERR_INVALID_PARAMS, { .str = U"print" });
	}
	char32_t *name = token_str(node->token->next->next);
	const variable_t *var = context_find_name(ctx, name);

	if (!var) {
		return FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = name });
	}

	free(name);
	return fmt_var(var);
}

/*
Returns `true` if the given node is a function name.
*/
bool is_func_name(const ast_node_t *node) {
	if (!node || !node->token) {
		return false;
	}
	char32_t *name = token_str(node->token);

	const bool ret = is_func_name_str(name);

	free(name);
	return ret;
}

/*
Returns `true` if the given string is a function name.
*/
bool is_func_name_str(const char32_t *name) {
	return c32scmp(name, U"clear") || c32scmp(name, U"print");
}
