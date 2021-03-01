#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/common/errors.h"
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
#include "skull/llvm/shared.h"

#include "skull/llvm/var.h"

Expr token_to_simple_expr(const Token *const);

/*
Convert `token` to an expression.

If `variable` is and `token` is a variable, store the found variable there.
*/
Expr token_to_expr(const Token *const token, Variable **variable) {
	if (token->type == TOKEN_IDENTIFIER) {
		Variable *const var_found = scope_find_var(token);

		if (variable) {
			*variable = var_found;
		}

		if (var_found->is_const &&
			!(var_found->is_global &&
			!var_found->is_const_lit)
		) {
			return (Expr){
				.llvm_value = var_found->llvm_value,
				.type = var_found->type
			};
		}

		return (Expr) {
			.llvm_value = LLVMBuildLoad2(
				SKULL_STATE.builder,
				var_found->type->llvm_type(),
				var_found->llvm_value,
				""
			),
			.type = var_found->type
		};
	}

	return token_to_simple_expr(token);
}

const Type *var_def_node_to_type(const AstNode *);

/*
Make and add a variable from `node` to Skull state.
*/
Variable *node_to_var(const AstNode *const node) {
	const Token *token = ATTR(AstNodeVarDef, node, name_tok);
	const Type *type = NULL;

	if (ATTR(AstNodeVarDef, node, is_implicit)) {
		type = var_def_node_to_type(node);
	}
	else {
		char *const type_name = token_mbs_str(token->next);

		type = find_type(type_name);
		free(type_name);

		if (!type) {
			PANIC(ERR_TYPE_NOT_FOUND, { .tok = token->next });
		}
	}

	char32_t *const name = token_str(token);

	Variable *var = make_variable(
		type,
		name,
		ATTR(AstNodeVarDef, node, is_const)
	);

	if (scope_add_var(SKULL_STATE.scope, var)) {
		free(name);
		return var;
	}
	free_variable(var);

	PANIC(ERR_VAR_ALREADY_DEFINED, { .tok = token });
}

/*
Return a variable type based on `node`.
*/
const Type *var_def_node_to_type(const AstNode *node) {
	const TokenType token_type = node->next->token->type;
	const NodeType node_type = node->next->type;

	if (node_type == AST_NODE_OPER_EXPR) {
		const TokenType oper = ATTR(AstNodeOper, node->next, oper);

		if (oper == TOKEN_OPER_NOT ||
			oper == TOKEN_OPER_IS ||
			oper == TOKEN_OPER_ISNT ||
			oper == TOKEN_OPER_LESS_THAN ||
			oper == TOKEN_OPER_GTR_THAN ||
			oper == TOKEN_OPER_LESS_THAN_EQ ||
			oper == TOKEN_OPER_GTR_THAN_EQ ||
			oper == TOKEN_OPER_AND ||
			oper == TOKEN_OPER_OR ||
			oper == TOKEN_OPER_XOR
		) {
			return &TYPE_BOOL;
		}
	}

	if (token_type == TOKEN_BOOL_CONST) {
		return &TYPE_BOOL;
	}
	if (token_type == TOKEN_INT_CONST) {
		return &TYPE_INT;
	}
	if (token_type == TOKEN_FLOAT_CONST) {
		return &TYPE_FLOAT;
	}
	if (token_type == TOKEN_RUNE_CONST) {
		return &TYPE_RUNE;
	}
	if (token_type == TOKEN_STR_CONST) {
		return &TYPE_STR;
	}
	if (node_type == AST_NODE_FUNCTION) {
		char *const func_name = token_mbs_str(node->next->token);

		const FunctionDeclaration *const function = ht_get(
			SKULL_STATE.function_decls,
			func_name
		);
		free(func_name);

		if (!function) {
			PANIC(ERR_MISSING_DECLARATION, { .tok = node->next->token });
		}

		const Type *type = function->return_type;
		if (!type) {
			PANIC(ERR_NO_VOID_ASSIGN, {
				.tok = node->next->token,
				.real = token_mbs_str(node->token)
			});
		}

		return type;
	}
	if (token_type == TOKEN_IDENTIFIER) {
		return scope_find_var(node->next->token)->type;
	}

	PANIC(ERR_INVALID_INPUT, { .tok = node->next->token });
}

/*
Make an expression from `token`, checking for compatibility with `type`.
*/
Expr token_to_simple_expr_typed(
	const Type *const type,
	const Token *const token
) {
	const Expr expr = token_to_simple_expr(token);

	if (!expr.type) {
		PANIC(ERR_TYPE_MISMATCH, {
			.tok = token,
			.type = type
		});
	}

	return expr;
}

/*
Make a simple expression (const literal) from `token`.
*/
Expr token_to_simple_expr(const Token *const token) {
	LLVMValueRef llvm_value = NULL;
	const Type *type = NULL;

	if (token->type == TOKEN_INT_CONST) {
		llvm_value = LLVM_INT(eval_integer(token));
		type = &TYPE_INT;
	}
	else if (token->type == TOKEN_FLOAT_CONST) {
		llvm_value = LLVM_FLOAT(eval_float(token));
		type = &TYPE_FLOAT;
	}
	else if (token->type == TOKEN_BOOL_CONST) {
		llvm_value = LLVM_BOOL(eval_bool(token));
		type = &TYPE_BOOL;
	}
	else if (token->type == TOKEN_RUNE_CONST) {
		llvm_value = LLVM_RUNE(eval_rune(token));
		type = &TYPE_RUNE;
	}
	else if (token->type == TOKEN_STR_CONST) {
		SkullStr str = eval_str(token);
		char *const mbs = c32stombs(str);

		llvm_value = LLVMBuildBitCast(
			SKULL_STATE.builder,
			LLVMBuildGlobalString(SKULL_STATE.builder, mbs, ""),
			TYPE_STR.llvm_type(),
			""
		);

		free(mbs);
		free(str);

		type = &TYPE_STR;
	}

	return (Expr){
		.llvm_value = llvm_value,
		.type = type
	};
}
