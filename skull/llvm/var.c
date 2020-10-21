#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/types/defs.h"
#include "skull/parse/classify.h"

#include "skull/llvm/aliases.h"

#include "skull/llvm/var.h"

#define PANIC(str) \
	char *const panic_str = c32stombs(str); \
	printf("Compilation error: %s\n", panic_str); \
	free(panic_str); \
	exit(1)

extern LLVMBuilderRef builder;

const Type *token_type_to_type(const Token *token) {
	if (token->token_type == TOKEN_INT_CONST) {
		return &TYPE_INT;
	}
	if (token->token_type == TOKEN_FLOAT_CONST) {
		return &TYPE_FLOAT;
	}
	if (token->token_type == TOKEN_RUNE_CONST) {
		return &TYPE_RUNE;
	}
	if (token->token_type == TOKEN_BOOL_CONST) {
		return &TYPE_BOOL;
	}
	if (token->token_type == TOKEN_STR_CONST) {
		return &TYPE_STR;
	}
	return NULL;
}

/*
Make and add a variable from `node` to `scope`.
*/
void node_make_var(const AstNode *const node, Scope *const scope) {
	const Token *token = node->token;
	const bool is_const = ATTR(AstNodeVarDef, node, is_const);
	if (!is_const) {
		token = token->next;
	}

	if (!node->next) {
		PANIC(FMT_ERROR(ERR_MISSING_ASSIGNMENT, { .tok = token }));
	}

	char32_t *const name = token_str(token);
	Variable *var = NULL;

	if (ATTR(AstNodeVarDef, node, is_implicit)) {
		const Type *type = NULL;
		if (node->next->node_type == AST_NODE_INT_CONST) {
			type = &TYPE_INT;
		}
		else if (node->next->node_type == AST_NODE_FLOAT_CONST) {
			type = &TYPE_FLOAT;
		}
		else if (node->next->node_type == AST_NODE_BOOL_CONST) {
			type = &TYPE_BOOL;
		}
		else if (node->next->node_type == AST_NODE_STR_CONST) {
			type = &TYPE_STR;
		}
		else if (node->next->node_type == AST_NODE_RUNE_CONST) {
			type = &TYPE_RUNE;
		}
		else if (node->next->node_type == AST_NODE_IDENTIFIER) {
			char32_t *const lookup = token_str(node->next->token);
			Variable *const new_var = scope_find_name(scope, lookup);

			if (!new_var) {
				free(name);
				PANIC(FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = lookup }));
			}
			free(lookup);
			type = new_var->type;
		}
		else if (node->next->node_type == AST_NODE_ADD_CONSTS ||
			node->next->node_type == AST_NODE_SUB_CONSTS ||
			node->next->node_type == AST_NODE_MULT_CONSTS ||
			node->next->node_type == AST_NODE_DIV_CONSTS
		) {
			type = token_type_to_type(node->next->token);
		}
		else {
			free(name);
			PANIC(FMT_ERROR(ERR_INVALID_INPUT, { .tok = node->next->token }));
		}
		var = make_variable(type, name, is_const);
	}
	else {
		char32_t *const wide_type_name = token_str(token->next);
		char *const type_name = c32stombs(wide_type_name);
		free(wide_type_name);

		var = make_variable(find_type(type_name), name, is_const);
		free(type_name);
	}

	if (scope_add_var(scope, var)) {
		free(name);
		return;
	}
	free_variable(var);

	PANIC(FMT_ERROR(ERR_VAR_ALREADY_DEFINED, { .real = name }));
}
