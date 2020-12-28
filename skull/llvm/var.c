#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/eval/types/bool.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/types/float.h"
#include "skull/eval/types/int.h"
#include "skull/eval/types/rune.h"
#include "skull/eval/types/str.h"
#include "skull/llvm/aliases.h"
#include "skull/llvm/func.h"
#include "skull/llvm/scope.h"

#include "skull/llvm/var.h"

extern LLVMBuilderRef BUILDER;
extern Scope *SCOPE;

const Type *token_type_to_type(const Token *const token) {
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

LLVMValueRef llvm_token_get_value(const Token *const token, Variable **variable) {
	if (token->token_type == TOKEN_IDENTIFIER) {
		Variable *var_found = scope_find_var(token);

		if (variable) {
			*variable = var_found;
		}

		if (var_found->is_const && !(var_found->is_global && !var_found->is_const_lit)) {
			return var_found->llvm_value;
		}

		return LLVMBuildLoad2(
			BUILDER,
			var_found->type->llvm_type(),
			var_found->llvm_value,
			""
		);
	}

	return llvm_parse_token(token);
}

/*
Make and add a variable from `node` to global scope.
*/
void node_make_var(const AstNode *const node) {
	const Token *token = node->token;

	const bool is_const = ATTR(AstNodeVarDef, node, is_const);
	if (!is_const) {
		token = token->next;
	}

	if (!node->next) {
		PANIC(ERR_MISSING_ASSIGNMENT, { .tok = token });
	}

	char32_t *const name = token_str(token);
	Variable *var = NULL;

	if (ATTR(AstNodeVarDef, node, is_implicit)) {
		const Type *type = token_type_to_type(node->next->token);

		if (type) {
			// fallthrough
		}
		else if (node->next->node_type == AST_NODE_FUNCTION) {
			char *func_name = token_mbs_str(node->next->token);

			FunctionDeclaration *function = FUNCTION_DECLARATIONS;
			while (function) {
				if (strcmp(func_name, function->name) == 0) {
					type = function->return_type;

					if (!type) {
						PANIC(ERR_NO_VOID_ASSIGN, {
							.tok = node->next->token,
							.str = name
						});
					}
					break;
				}
				function = function->next;
			}

			if (!function) {
				PANIC(ERR_MISSING_DECLARATION, { .tok = node->next->token });
			}
		}
		else if (node->next->node_type == AST_NODE_IDENTIFIER || (
			(node->next->node_type == AST_NODE_ADD ||
			node->next->node_type == AST_NODE_SUB ||
			node->next->node_type == AST_NODE_MULT ||
			node->next->node_type == AST_NODE_DIV) &&
			node->next->token->token_type == TOKEN_IDENTIFIER)
		) {
			type = scope_find_var(node->next->token)->type;
		}
		else {
			free(name);
			PANIC(ERR_INVALID_INPUT, { .tok = node->next->token });
		}
		var = make_variable(type, name, is_const);
	}
	else {
		char *const type_name = token_mbs_str(token->next);

		var = make_variable(find_type(type_name), name, is_const);
		free(type_name);
	}

	if (scope_add_var(SCOPE, var)) {
		free(name);
		return;
	}
	free_variable(var);

	PANIC(ERR_VAR_ALREADY_DEFINED, { .tok = token });
}

/*
Make an `LLVMValueRef` from `token`, checking for compatibility with `type`.
*/
LLVMValueRef llvm_parse_token_typed(const Type *const type, const Token *const token) {
	if (type == token_type_to_type(token)) {
		return llvm_parse_token(token);
	}

	PANIC(ERR_TYPE_MISMATCH, {
		.tok = token,
		.type = type
	});
}

/*
Make an `LLVMValueRef` from `token`.
*/
LLVMValueRef llvm_parse_token(const Token *const token) {
	if (token->token_type == TOKEN_INT_CONST) {
		return LLVM_INT(eval_integer(token));
	}
	if (token->token_type == TOKEN_FLOAT_CONST) {
		return LLVM_FLOAT(eval_float(token));
	}
	if (token->token_type == TOKEN_BOOL_CONST) {
		return LLVM_BOOL(eval_bool(token));
	}
	if (token->token_type == TOKEN_RUNE_CONST) {
		return LLVM_RUNE(eval_rune(token));
	}
	if (token->token_type == TOKEN_STR_CONST) {
		SkullStr str = eval_str(token);
		char *const mbs = c32stombs(str);

		LLVMValueRef ret = LLVMBuildBitCast(
			BUILDER,
			LLVMBuildGlobalString(BUILDER, mbs, ""),
			TYPE_STR.llvm_type(),
			""
		);

		free(mbs);
		free(str);

		return ret;
	}

	return NULL;
}
