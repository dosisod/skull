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

Expr llvm_token_get_value(const Token *const token, Variable **variable) {
	if (token->token_type == TOKEN_IDENTIFIER) {
		Variable *var_found = scope_find_var(token);

		if (variable) {
			*variable = var_found;
		}

		if (var_found->is_const && !(var_found->is_global && !var_found->is_const_lit)) {
			return (Expr){
				.llvm_value = var_found->llvm_value,
				.type = var_found->type
			};
		}

		return (Expr) {
			.llvm_value = LLVMBuildLoad2(
				BUILDER,
				var_found->type->llvm_type(),
				var_found->llvm_value,
				""
			),
			.type = var_found->type
		};
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
		const TokenType token_type = node->next->token->token_type;
		const NodeType node_type = node->next->node_type;
		const Type *type = NULL;

		if (token_type == TOKEN_INT_CONST) {
			type = &TYPE_INT;
		}
		else if (token_type == TOKEN_FLOAT_CONST) {
			type = &TYPE_FLOAT;
		}
		else if (token_type == TOKEN_RUNE_CONST) {
			type = &TYPE_RUNE;
		}
		else if (token_type == TOKEN_BOOL_CONST) {
			type = &TYPE_BOOL;
		}
		else if (token_type == TOKEN_STR_CONST) {
			type = &TYPE_STR;
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
		else if (node_type == AST_NODE_IDENTIFIER || (
			(node_type == AST_NODE_ADD ||
			node_type == AST_NODE_SUB ||
			node_type == AST_NODE_MULT ||
			node_type == AST_NODE_DIV) &&
			token_type == TOKEN_IDENTIFIER)
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
Make an expression from `token`, checking for compatibility with `type`.
*/
Expr llvm_parse_token_typed(const Type *const type, const Token *const token) {
	Expr expr = llvm_parse_token(token);

	if (!expr.type) {
		PANIC(ERR_TYPE_MISMATCH, {
			.tok = token,
			.type = type
		});
	}

	return expr;
}

/*
Make an expression from `token`.
*/
Expr llvm_parse_token(const Token *const token) {
	if (token->token_type == TOKEN_INT_CONST) {
		return (Expr){
			.llvm_value = LLVM_INT(eval_integer(token)),
			.type = &TYPE_INT
		};
	}
	if (token->token_type == TOKEN_FLOAT_CONST) {
		return (Expr){
			.llvm_value = LLVM_FLOAT(eval_float(token)),
			.type = &TYPE_FLOAT
		};
	}
	if (token->token_type == TOKEN_BOOL_CONST) {
		return (Expr){
			.llvm_value = LLVM_BOOL(eval_bool(token)),
			.type = &TYPE_BOOL
		};
	}
	if (token->token_type == TOKEN_RUNE_CONST) {
		return (Expr){
			.llvm_value = LLVM_RUNE(eval_rune(token)),
			.type = &TYPE_RUNE
		};
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

		return (Expr){
			.llvm_value = ret,
			.type = &TYPE_STR
		};
	}

	return (Expr){0};
}
