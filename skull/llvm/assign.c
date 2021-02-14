#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/eval/scope.h"
#include "skull/eval/types/defs.h"
#include "skull/llvm/aliases.h"
#include "skull/llvm/func.h"
#include "skull/llvm/oper.h"
#include "skull/llvm/scope.h"
#include "skull/llvm/shared.h"

#include "skull/llvm/assign.h"

void llvm_assign_value_to_var(Variable *const, LLVMValueRef);

/*
Create a type alias from `node`.
*/
void llvm_make_type_alias(AstNode **node) {
	char *type_name = token_mbs_str((*node)->token->next->next);

	const bool added = add_alias(
		(Type *)find_type(type_name),
		token_mbs_str((*node)->token)
	);

	free(type_name);

	if (!added) {
		PANIC(ERR_ALIAS_ALREADY_DEFINED, {
			.tok = (*node)->token
		});
	}
}

/*
Builds a variable from `node`.
*/
void llvm_make_var_def(AstNode **node) {
	Variable *var = node_make_var(*node);

	llvm_assign_value_to_var(
		var,
		node_to_expr(var->type, (*node)->next, var).llvm_value
	);

	*node = (*node)->next;
}

/*
Build a LLVM `load` operation from `node`.
*/
void llvm_make_var_assign(AstNode **node) {
	Variable *found_var = scope_find_var((*node)->token);

	if (found_var->is_const) {
		PANIC(ERR_REASSIGN_CONST, {
			.tok = (*node)->token
		});
	}

	llvm_assign_value_to_var(
		found_var,
		node_to_expr(found_var->type, (*node)->next, found_var).llvm_value
	);

	*node = (*node)->next;
}

Expr llvm_make_bool_expr(const AstNode *const);

/*
Create an expression from `node` with type `type`.

Optionally pass `var` if expression is going to be assigned to a variable.
*/
Expr node_to_expr(
	const Type *const type,
	const AstNode *const node,
	const Variable *const var
) {
	Expr expr;

	if (node->type == AST_NODE_IDENTIFIER) {
		expr = llvm_assign_identifier(type, node, var);
	}
	else if (node->type == AST_NODE_ADD) {
		expr = llvm_make_oper(type, node, &llvm_make_add);
	}
	else if (node->type == AST_NODE_SUB) {
		expr = llvm_make_oper(type, node, &llvm_make_sub);
	}
	else if (node->type == AST_NODE_MULT) {
		expr = llvm_make_oper(type, node, &llvm_make_mult);
	}
	else if (node->type == AST_NODE_DIV) {
		expr = llvm_make_oper(type, node, &llvm_make_div);
	}
	else if (node->type == AST_NODE_MOD) {
		expr = llvm_make_oper(type, node, &llvm_make_mod);
	}
	else if (node->type == AST_NODE_LSHIFT) {
		expr = llvm_make_oper(type, node, &llvm_make_lshift);
	}
	else if (node->type == AST_NODE_POW) {
		expr = llvm_make_oper(type, node, &llvm_make_pow);
	}
	else if (node->type == AST_NODE_RSHIFT) {
		expr = llvm_make_oper(type, node, &llvm_make_rshift);
	}
	else if (node->type == AST_NODE_BOOL_EXPR) {
		expr = llvm_make_bool_expr(node);
	}
	else if (node->type == AST_NODE_FUNCTION) {
		expr = llvm_make_function_call(node);

		if (type && expr.type != type) {
			PANIC(ERR_TYPE_MISMATCH, {
				.tok = node->token,
				.type = type
			});
		}
	}
	else {
		expr = token_to_simple_expr_typed(type, node->token);
	}

	if (!expr.llvm_value) {
		PANIC(ERR_INVALID_EXPR, { .tok = node->token });
	}

	return expr;
}

/*
Build LLVM code to handle boolean expressions from `node`.
*/
Expr llvm_make_bool_expr(const AstNode *const node) {
	const Token *const lhs = ATTR(AstNodeBoolExpr, node, lhs);
	const TokenType oper = ATTR(AstNodeBoolExpr, node, oper);
	const Token *const rhs = ATTR(AstNodeBoolExpr, node, rhs);

	LLVMValueRef value = NULL;

	if (oper == TOKEN_OPER_NOT) {
		const Expr rhs_expr = token_to_expr(rhs, NULL);

		if (rhs_expr.type != &TYPE_BOOL) {
			PANIC(ERR_NON_BOOL_EXPR, { .tok = rhs });
		}

		value = LLVMBuildNot(
			SKULL_STATE.builder,
			rhs_expr.llvm_value,
			""
		);
	}
	else if (oper == TOKEN_OPER_IS ||
		oper == TOKEN_OPER_ISNT ||
		oper == TOKEN_OPER_LESS_THAN ||
		oper == TOKEN_OPER_GTR_THAN ||
		oper == TOKEN_OPER_LESS_THAN_EQ ||
		oper == TOKEN_OPER_GTR_THAN_EQ ||
		oper == TOKEN_OPER_AND ||
		oper == TOKEN_OPER_OR ||
		oper == TOKEN_OPER_XOR
	) {
		const Expr lhs_expr = token_to_expr(lhs, NULL);
		const Expr rhs_expr = token_to_expr(rhs, NULL);

		if (lhs_expr.type != rhs_expr.type) {
			PANIC(ERR_TYPE_MISMATCH, {
				.tok = rhs,
				.type = lhs_expr.type
			});
		}

		Operation *func = NULL;

		if (oper == TOKEN_OPER_IS) {
			func = llvm_make_is;
		}
		else if (oper == TOKEN_OPER_ISNT) {
			func = llvm_make_is_not;
		}
		else if (oper == TOKEN_OPER_LESS_THAN) {
			func = llvm_make_less_than;
		}
		else if (oper == TOKEN_OPER_GTR_THAN) {
			func = llvm_make_gtr_than;
		}
		else if (oper == TOKEN_OPER_LESS_THAN_EQ) {
			func = llvm_make_less_than_eq;
		}
		else if (oper == TOKEN_OPER_GTR_THAN_EQ) {
			func = llvm_make_gtr_than_eq;
		}
		else {
			if (lhs_expr.type != &TYPE_BOOL) {
				PANIC(ERR_TYPE_MISMATCH, {
					.tok = lhs,
					.type = &TYPE_BOOL
				});
			}

			if (oper == TOKEN_OPER_AND) {
				func = llvm_make_and;
			}
			else if (oper == TOKEN_OPER_OR) {
				func = llvm_make_or;
			}
			else if (oper == TOKEN_OPER_XOR) {
				func = llvm_make_xor;
			}
		}

		value = func(
			lhs_expr.type,
			lhs_expr.llvm_value,
			rhs_expr.llvm_value
		).llvm_value;

		if (!value) {
			PANIC(ERR_NOT_COMPARIBLE, { .tok = lhs });
		}
	}

	return (Expr){
		.llvm_value = value,
		.type = &TYPE_BOOL
	};
}

/*
Assign `value` to `var`.
*/
void llvm_assign_value_to_var(Variable *const var, LLVMValueRef value) {
	const bool is_first_assign = !var->llvm_value;
	const bool is_const_literal = LLVMIsConstant(value);

	const bool is_global = is_first_assign ?
		(SKULL_STATE.current_func == SKULL_STATE.main_func) :
		var->is_global;

	if (is_first_assign) {
		if (is_global && (!var->is_const || !is_const_literal)) {
			var->llvm_value = LLVMAddGlobal(
				SKULL_STATE.module,
				var->type->llvm_type(),
				var->name
			);

			LLVMSetLinkage(var->llvm_value, LLVMPrivateLinkage);

			LLVMSetInitializer(
				var->llvm_value,
				LLVMConstNull(var->type->llvm_type())
			);
		}
		else if (!is_global && !var->is_const) {
			var->llvm_value = LLVMBuildAlloca(
				SKULL_STATE.builder,
				var->type->llvm_type(),
				var->name
			);
		}

		var->is_global = is_global;
		var->is_const_lit = is_const_literal;
	}

	if (var->is_const && !(is_global && !is_const_literal)) {
		var->llvm_value = value;
	}
	else {
		LLVMBuildStore(
			SKULL_STATE.builder,
			value,
			var->llvm_value
		);
	}
}

/*
Return expression for assigning existing identifier `node` with type `type`.

Optionally pass `var` if result is expected to be assigned to a variable.

If `type` is not set, the expression type will not be checked.
*/
Expr llvm_assign_identifier(
	const Type *const type,
	const AstNode *const node,
	const Variable *const var
) {
	Variable *var_found = NULL;
	const Expr expr = token_to_expr(node->token, &var_found);

	if (type && var_found->type != type) {
		PANIC(ERR_TYPE_MISMATCH, {
			.tok = node->token,
			.type = type
		});
	}
	if (var == var_found) {
		PANIC(ERR_REDUNDANT_REASSIGN, {
			.tok = node->token,
			.var = var
		});
	}

	return expr;
}
