#include <stdio.h>

#include "../common/malloc.h"
#include "../common/str.h"
#include "../errors.h"
#include "../parse/classify.h"

#include "function.h"

/*
Returns `true` if the given node is a function name.
*/
bool is_func_name(const ast_node_t *node) {
	if (node==NULL || node->token==NULL) {
		return false;
	}
	MAKE_TOKEN_BUF(name, node->token);

	return c32scmp(name, U"clear") || c32scmp(name, U"print");
}

/*
Returns string that when printed, clears the screen.
*/
const char32_t *func_clear(ast_node_t *node) {
	if (node->node_type!=AST_NODE_NO_PARAM_FUNC) {
		return ERR_UNEXPECTED_PARAM;
	}
	const char32_t *ret=c32sdup(U"\033[2J\033[;1H");
	DIE_IF_MALLOC_FAILS(ret);

	return ret;
}

/*
Print out a variable.
*/
const char32_t *func_print(ast_node_t *node, context_t *ctx) {
	if (node->node_type!=AST_NODE_ONE_PARAM_FUNC ||
		node->token->next->next->token_type!=TOKEN_IDENTIFIER
	) {
		return ERR_INVALID_PARAMS;
	}
	MAKE_TOKEN_BUF(name, node->token->next->next);
	const variable_t *var=context_find_name(ctx, name);

	if (var==NULL) {
		return ERR_VAR_NOT_FOUND;
	}
	return fmt_var(var);
}
